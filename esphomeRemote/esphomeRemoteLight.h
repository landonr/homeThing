#include "esphome.h"
#include "DisplayUpdateInterface.h"
#include "MenuTitle.h"
#include "FriendlyNameEntity.h"
#include <sstream>

#pragma once

/* Home Assistant entities can have these modes in color_mode/supported_color_mode.
 * Depending on the mode the light supports we want to show sliders or not.
 *
 * |color_mode | supports brightness | supports color_temp |
 * |---------- | ------------------- | ------------------- |
 * | UNKNOWN   |          -          |          -          |
 * | ONOFF     |          -          |          -          |
 * | BRIGHTNESS|          x          |          -          |
 * | COLOR_TEMP|          x          |          x          |
 * | HS        |          x          |          -          |
 * | RGB       |          x          |          -          |
 * | RGBW      |          x          |          -          |
 * | RGBWW     |          x          |          -          |
 * | WHITE     |          x          |          -          |
 * | XY        |          x          |          -          |
 */

class LightService: public CustomAPIDevice, public Component {
  public:
    LightService(std::string newFriendlyName, std::string newEntityId, DisplayUpdateInterface& newCallback) : friendlyName(newFriendlyName), entityId(newEntityId), display(newCallback) {
      onState = false;
      subscribe_homeassistant_state(&LightService::state_changed, newEntityId.c_str());
      subscribe_homeassistant_state(&LightService::brightness_changed, newEntityId.c_str(),"brightness");
      subscribe_homeassistant_state(&LightService::color_temp_changed, newEntityId.c_str(),"color_temp");
      subscribe_homeassistant_state(&LightService::color_mode_changed, newEntityId.c_str(),"color_mode");
    }
    std::string friendlyName;
    std::string entityId;
    DisplayUpdateInterface& display;
    int brightness = 0;
    int color_temp = 0;
    std::string color_mode = "";
    bool onState;

    void decTemperature() {
        std::stringstream ss;
        ss << (color_temp - id(inc_color_temperature_step));
        const std::map< std::string, std::string > data = {
                                                              {"entity_id",entityId.c_str()},
                                                              {"color_temp", ss.str()} ,
                                                          };
        setAttribute(data);
    }

    void incTemperature() {
        std::stringstream ss;
        ss << (color_temp + id(dec_color_temperature_step));
        const std::map< std::string, std::string > data = {
                                                              {"entity_id",entityId.c_str()},
                                                              {"color_temp", ss.str()} ,
                                                          };
        setAttribute(data);
    }

    void decBrightness() {
        std::stringstream ss;
        ss << (brightness - id(dec_brightness_step));
        const std::map< std::string, std::string > data = {
                                                              {"entity_id",entityId.c_str()},
                                                              {"brightness", ss.str()} ,
                                                          };
        setAttribute(data);
    }

    void incBrightness() {
        std::stringstream ss;
        ss << (brightness + id(inc_brightness_step));
        const std::map< std::string, std::string > data = {
                                                              {"entity_id",entityId.c_str()},
                                                              {"brightness", ss.str()} ,
                                                          };
        setAttribute(data);
    }

    void setAttribute(const std::map< std::string, std::string> &data){
        call_homeassistant_service("light.turn_on", data);
    }

    void toggleLight() {
      call_homeassistant_service("light.toggle", {
        {"entity_id", entityId.c_str()}
      });
    }

    bool supportsBrightness(){
        return color_mode.compare("onoff") != 0 && color_mode.compare("unknown") != 0;
    }

    bool supportsColorTemperature(){
        // TODO: research if color lights support color temp as well
        // return color_mode.compare("color_temp") == 0;
        return true;
    }

    std::vector<std::shared_ptr<MenuTitleBase>> lightTitleItems() {
      std::vector<std::shared_ptr<MenuTitleBase>> out;
        out.push_back(std::make_shared<MenuTitleBase>(friendlyName, entityId, onState ? OnMenuTitleState : OffMenuTitleState));

        std::stringstream ss;
        if(supportsBrightness()){
            if(brightness > 0){
                float percent = ((float)brightness/255.0);
                int percentInt = (int)(percent*100);
                ss << "Brightness - " << percentInt << " %%";
            }else{
                ss << "Brightness";
            }
            out.push_back(std::make_shared<MenuTitleSlider>("Brightness", ss.str(), entityId, onState ? OnMenuTitleState : OffMenuTitleState, (int)(brightness*0.94)));
            ss.str("");
        }
        if(supportsColorTemperature()){
            if(color_temp > 0){
                ss << "Temperature - " << 1000000/color_temp << " K";
            }else{
                ss << "Temperature";
            }
            out.push_back(std::make_shared<MenuTitleSlider>("Temperature", ss.str(), entityId, onState ? OnMenuTitleState : OffMenuTitleState, (int)(color_temp *0.64)));
            ss.str("");
        }
      return out;
    }

  private:
    void state_changed(std::string newOnState) {
      ESP_LOGI("brightness", " changed to %s", newOnState.c_str());
      onState = newOnState == "on";
      // visualize that light is off by resetting brightness and color_temp
      if (!onState){
        brightness = 0;
        color_temp = 0;
      }
      display.updateDisplay(false);
    }
    void brightness_changed(std::string newOnState) {
      ESP_LOGI("brightness", "state changed to %s", newOnState.c_str());
      brightness = atoi(newOnState.c_str());
      display.updateDisplay(false);
    }
    void color_temp_changed(std::string newOnState){
      ESP_LOGI("color_temp", "state changed to %s", newOnState.c_str());
      color_temp = atoi(newOnState.c_str());
      display.updateDisplay(false);
    }
    void color_mode_changed(std::string newOnState){
      ESP_LOGI("color_mode_changed", "state changed to %s", newOnState.c_str());
      color_mode = newOnState.c_str();
      display.updateDisplay(false);
    }
};

class LightGroupComponent : public CustomAPIDevice, public Component {
  public:
    LightGroupComponent(DisplayUpdateInterface& newCallback) : display(newCallback) { }
    std::vector<LightService*> lights;

    void setup(std::vector<FriendlyNameEntity> newLights) {
      for (auto &light: newLights) {
        LightService *newService = new LightService(light.friendlyName, light.entityId, display);
        lights.push_back(newService);
      }
    }

    std::vector<std::shared_ptr<MenuTitleBase>> lightTitleSwitches() {
      std::vector<std::shared_ptr<MenuTitleBase>> out;
      for (auto &light: lights) {
        ESP_LOGI("Light", "state %d", light->onState);
        MenuTitleState state = light->onState ? OnMenuTitleState : OffMenuTitleState;
        out.push_back(std::make_shared<MenuTitleBase>(light->friendlyName, light->entityId, state));
      }
      return out;
    }

    bool selectLight(int index) {
     if(index >= 0 && index < lights.size()) {
        LightService *light = lights[index];
        light->toggleLight();
        return false;
      }
      return true;
    }

    int currentSelectedLight = -1;
    bool lightDetailSelected = false;

  private:
    DisplayUpdateInterface& display;
};
