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
enum ColorModeType{
    unknown_type,
    onoff_type,
    brightness_type,
    color_temp_type,
    hs_type,
    rgb_type,
    rgbw_type,
    rgbww_type,
    white_type,
    xy_type
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
    int local_brightness = -1;
    int local_color_temp = -1;
    bool is_brightness_in_sync = false;
    bool is_color_temp_in_sync = false;
    ColorModeType color_mode = unknown_type;
    bool onState;
    int min_mireds = 0;
    int max_mireds = 0;

    void decTemperature() {
        // only send update to home assistant if
        // we have received a confirmation for the last
        // state change (state changed from ha)
        if(!is_color_temp_in_sync){
            return;
        }
        // is_color_temp_in_sync = false;
        local_color_temp = local_color_temp + id(inc_color_temperature_step);
        const std::map< std::string, std::string > data = {
            {"entity_id",entityId.c_str()},
            {"color_temp", to_string(local_color_temp)}
        };
        setAttribute(data);
    }

    void incTemperature() {
        if(!is_color_temp_in_sync){
            return;
        }
        // is_color_temp_in_sync = false;
        local_color_temp = local_color_temp + id(inc_color_temperature_step);
        const std::map< std::string, std::string > data = {
            {"entity_id",entityId.c_str()},
            {"color_temp", to_string(local_color_temp)} ,
        };
        setAttribute(data);
    }

    void decBrightness() {
        // if(!is_brightness_in_sync){
        //     return;
        // }
        // is_brightness_in_sync = false;
        local_brightness = local_brightness - id(inc_brightness_step);
        const std::map< std::string, std::string > data = {
            {"entity_id",entityId.c_str()},
            {"brightness", to_string(local_brightness)},
        };
        setAttribute(data);
    }

    void incBrightness() {
        // if(!is_brightness_in_sync){
        //     return;
        // }
        // is_brightness_in_sync = false;
        local_brightness = local_brightness + id(inc_brightness_step);
        const std::map< std::string, std::string > data = {
            {"entity_id",entityId.c_str()},
            {"brightness", to_string(local_brightness)},
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
        return color_mode != onoff_type && color_mode != unknown_type;
    }

    bool supportsColorTemperature(){
        return color_mode == color_temp_type || color_mode == rgbww_type;
    }

    std::vector<std::shared_ptr<MenuTitleBase>> lightTitleItems() {
      std::vector<std::shared_ptr<MenuTitleBase>> out;
        out.push_back(std::make_shared<MenuTitleBase>(friendlyName, entityId, onState ? OnMenuTitleState : OffMenuTitleState));

        std::stringstream ss;
        int width_available = id(display_size_x) - 2 * id(slider_margin_size);
        if(supportsBrightness()){
            float slider_factor = 1;
            if(local_brightness > 0){
                float percent = ((float)local_brightness/255.0);
                int percentInt = (int)(percent*100);
                ss << "Brightness - " << percentInt << " %%";
                slider_factor = width_available / MAX_BRIGHTNESS;
            }else{
                ss << "Brightness";
            }
            out.push_back(std::make_shared<MenuTitleSlider>("Brightness", ss.str(), entityId, onState ? OnMenuTitleState : OffMenuTitleState, (int)(local_brightness * slider_factor)));
            ss.str("");
        }
        if(supportsColorTemperature()){
            float slider_factor = 1;
            if(local_color_temp > 0){
                ss << "Temperature - " << 1000000/local_color_temp << " K ";
                slider_factor = width_available / (max_mireds - min_mireds);
            }else{
                ss << "Temperature";
            }
            out.push_back(std::make_shared<MenuTitleSlider>("Temperature", ss.str(), entityId, onState ? OnMenuTitleState : OffMenuTitleState, (int)(local_color_temp-min_mireds * slider_factor)));
            ss.str("");
        }
      return out;
    }

  private:
    void state_changed(std::string newOnState) {
      ESP_LOGI("state", " changed to %s", newOnState.c_str());
      onState = newOnState == "on";
      // visualize that light is off by resetting brightness and color_temp
      // if (!onState){
      //   local_brightness = 0;
      //   local_color_temp = 0;
      // }
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
      if(local_brightness == -1) {
        local_brightness = atoi(newOnState.c_str());
      }
      // is_brightness_in_sync = true;
      display.updateDisplay(false);
    }
    void color_temp_changed(std::string newOnState){
      ESP_LOGI("color_temp", "state changed to %s", newOnState.c_str());
      local_color_temp = atoi(newOnState.c_str());
      is_color_temp_in_sync = true;
      display.updateDisplay(false);
    }
    void color_mode_changed(std::string newOnState){
      ESP_LOGI("color_mode_changed", "state changed to %s", newOnState.c_str());
      if (strcmp(newOnState.c_str(), "onoff") == 0){
          color_mode = onoff_type;
      } else if (strcmp(newOnState.c_str(), "brightness") == 0){
          color_mode = brightness_type;
      } else if (strcmp(newOnState.c_str(), "color_temp") == 0){
          color_mode = color_temp_type;
      } else if (strcmp(newOnState.c_str(), "hs") == 0){
          color_mode = hs_type;
      } else if (strcmp(newOnState.c_str(), "rgb") == 0){
          color_mode = rgb_type;
      } else if (strcmp(newOnState.c_str(), "rgbw") == 0){
          color_mode = rgbw_type;
      } else if (strcmp(newOnState.c_str(), "rgbww") == 0){
          color_mode = rgbww_type;
      } else if (strcmp(newOnState.c_str(), "white") == 0){
          color_mode = white_type;
      } else if (strcmp(newOnState.c_str(), "xy") == 0){
          color_mode = xy_type;
      } else {
          color_mode = unknown_type;
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
