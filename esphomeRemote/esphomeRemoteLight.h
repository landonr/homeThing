#include "esphome.h"
#include "DisplayUpdateInterface.h"
#include "MenuTitle.h"
#include "FriendlyNameEntity.h"
#include <sstream>

#pragma once

#define MAX_BRIGHTNESS 255.0f
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
 * | RGBWW     |          x          |          x          |
 * | WHITE     |          x          |          -          |
 * | XY        |          x          |          -          |
 */
enum ColorMode{
    UNKNOWN,
    ONOFF,
    BRIGHTNESS,
    COLOR_TEMP,
    HS,
    RGB,
    RGBW,
    RGBWW,
    WHITE,
    XY
};

class LightService: public CustomAPIDevice, public Component {
  public:
    LightService(std::string newFriendlyName, std::string newEntityId, DisplayUpdateInterface& newCallback) : friendlyName(newFriendlyName), entityId(newEntityId), display(newCallback) {
      onState = false;
      subscribe_homeassistant_state(&LightService::state_changed, newEntityId.c_str());
      subscribe_homeassistant_state(&LightService::min_mireds_changed, newEntityId.c_str(),"min_mireds");
      subscribe_homeassistant_state(&LightService::max_mireds_changed, newEntityId.c_str(),"max_mireds");
      subscribe_homeassistant_state(&LightService::brightness_changed, newEntityId.c_str(),"brightness");
      subscribe_homeassistant_state(&LightService::color_temp_changed, newEntityId.c_str(),"color_temp");
      subscribe_homeassistant_state(&LightService::color_mode_changed, newEntityId.c_str(),"color_mode");
    }
    std::string friendlyName;
    std::string entityId;
    DisplayUpdateInterface& display;
    int brightness = 0;
    int color_temp = 0;
    ColorMode color_mode = UNKNOWN;
    bool onState;
    int min_mireds = 0;
    int max_mireds = 0;

    void decTemperature() {
        const std::map< std::string, std::string > data = {
                                                              {"entity_id",entityId.c_str()},
                                                              {"color_temp", to_string(color_temp - id(inc_color_temperature_step))} ,
                                                          };
        setAttribute(data);
    }

    void incTemperature() {
        const std::map< std::string, std::string > data = {
                                                              {"entity_id",entityId.c_str()},
                                                              {"color_temp", to_string(color_temp + id(inc_color_temperature_step))} ,
                                                          };
        setAttribute(data);
    }

    void decBrightness() {
        const std::map< std::string, std::string > data = {
                                                              {"entity_id",entityId.c_str()},
                                                              {"brightness", to_string(brightness - id(inc_brightness_step))} ,
                                                          };
        setAttribute(data);
    }

    void incBrightness() {
        const std::map< std::string, std::string > data = {
                                                              {"entity_id",entityId.c_str()},
                                                              {"brightness", to_string(brightness + id(inc_brightness_step))} ,
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
        return color_mode != ONOFF && color_mode != UNKNOWN;
    }

    bool supportsColorTemperature(){
        return color_mode == COLOR_TEMP || color_mode == RGBWW;
    }

    std::vector<std::shared_ptr<MenuTitleBase>> lightTitleItems() {
      std::vector<std::shared_ptr<MenuTitleBase>> out;
        out.push_back(std::make_shared<MenuTitleBase>(friendlyName, entityId, onState ? OnMenuTitleState : OffMenuTitleState));

        std::stringstream ss;
        int width_available = id(display_size_x) - 2 * id(slider_margin_size);
        if(supportsBrightness()){
            float slider_factor = 1;
            if(brightness > 0){
                float percent = ((float)brightness/255.0);
                int percentInt = (int)(percent*100);
                ss << "Brightness - " << percentInt << " %%";
                slider_factor = width_available / MAX_BRIGHTNESS;
            }else{
                ss << "Brightness";
            }
            out.push_back(std::make_shared<MenuTitleSlider>("Brightness", ss.str(), entityId, onState ? OnMenuTitleState : OffMenuTitleState, (int)(brightness * slider_factor)));
            ss.str("");
        }
        if(supportsColorTemperature()){
            float slider_factor = 1;
            if(color_temp > 0){
                ss << "Temperature - " << 1000000/color_temp << " K ";
                slider_factor = width_available / (max_mireds - min_mireds);
            }else{
                ss << "Temperature";
            }
            out.push_back(std::make_shared<MenuTitleSlider>("Temperature", ss.str(), entityId, onState ? OnMenuTitleState : OffMenuTitleState, (int)(color_temp-min_mireds * slider_factor)));
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
    void min_mireds_changed(std::string newOnState) {
      min_mireds = atoi(newOnState.c_str());
      display.updateDisplay(false);
    }
    void max_mireds_changed(std::string newOnState) {
      max_mireds = atoi(newOnState.c_str());
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
      if (strcmp(newOnState.c_str(), "onoff") == 0){
          color_mode = ONOFF;
      } else if (strcmp(newOnState.c_str(), "brightness") == 0){
          color_mode = BRIGHTNESS;
      } else if (strcmp(newOnState.c_str(), "color_temp") == 0){
          color_mode = COLOR_TEMP;
      } else if (strcmp(newOnState.c_str(), "hs") == 0){
          color_mode = HS;
      } else if (strcmp(newOnState.c_str(), "rgb") == 0){
          color_mode = RGB;
      } else if (strcmp(newOnState.c_str(), "rgbw") == 0){
          color_mode = RGBW;
      } else if (strcmp(newOnState.c_str(), "rgbww") == 0){
          color_mode = RGBWW;
      } else if (strcmp(newOnState.c_str(), "white") == 0){
          color_mode = WHITE;
      } else if (strcmp(newOnState.c_str(), "xy") == 0){
          color_mode = XY;
      } else {
          color_mode = UNKNOWN;
      }
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
