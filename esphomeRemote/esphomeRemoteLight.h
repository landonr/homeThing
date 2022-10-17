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
 * | BRIGHTNESS|          x          |          x          |
 * | COLOR_TEMP|          x          |          -          |
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
      ESP_LOGI("inital color_mode", "state changed to %s", color_mode.c_str());
      // subscribe_homeassistant_state(&LightService::, newEntityId.c_str());
    }
    std::string friendlyName;
    std::string entityId;
    DisplayUpdateInterface& display;
    int brightness = 0;
    int color_temp = 0;
    std::string color_mode = "";
    bool onState;

    // TODO:
    // * change slider design to match overall design
    // * only show sliders if the light supports them 
    // * reduce number of functions (maybe just one which is handed a struct
    // * cleanup
    // * only show lightDetailMenu if light supports it. Otherwise just toggle it
    // * only enable scrolling menu if light is turned on
    // * if light is off show sliders as inactive

    void decTemperature() {
        std::stringstream ss;
        ss << (color_temp - 10);
        const std::map< std::string, std::string > data = {
                                                              {"entity_id",entityId.c_str()},
                                                              {"color_temp", ss.str()} ,
                                                          };
        setAttribute(data);
    }

    void incTemperature() {
        std::stringstream ss;
        ss << (color_temp + 10);
        const std::map< std::string, std::string > data = {
                                                              {"entity_id",entityId.c_str()},
                                                              {"color_temp", ss.str()} ,
                                                          };
        setAttribute(data);
    }

    void decBrightness() {
        std::stringstream ss;
        ss << (brightness - 10);
        const std::map< std::string, std::string > data = {
                                                              {"entity_id",entityId.c_str()},
                                                              {"brightness", ss.str()} ,
                                                          };
        setAttribute(data);
    }

    void incBrightness() {
        std::stringstream ss;
        ss << (brightness + 10);
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
        return color_mode.compare("onoff") == 0 || color_mode.compare("unknown") == 0;
    }

    bool supportsColorTemperature(){
        return false;
    }

  private:
    void state_changed(std::string newOnState) {
      ESP_LOGI("brightness", " changed to %s", newOnState.c_str());
      ESP_LOGI("color_mode", "state changed to %s", color_mode.c_str());
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
  private:
    DisplayUpdateInterface& display;
};
