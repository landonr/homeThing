#include "esphome.h"
#include "DisplayUpdateInterface.h"
#include "MenuTitle.h"
#include "FriendlyNameEntity.h"

#pragma once

#define MAX_BRIGHTNESS 255.0f

enum ColorModeType {
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

class LightService : public CustomAPIDevice, public Component {
 public:
  LightService(std::string newFriendlyName, std::string newEntityId, DisplayUpdateInterface &newCallback)
      : friendlyName(newFriendlyName), entityId(newEntityId), display(newCallback) {
    onState = false;
    subscribe_homeassistant_state(&LightService::state_changed, newEntityId.c_str());
    subscribe_homeassistant_state(&LightService::min_mireds_changed, newEntityId.c_str(), "min_mireds");
    subscribe_homeassistant_state(&LightService::max_mireds_changed, newEntityId.c_str(), "max_mireds");
    subscribe_homeassistant_state(&LightService::brightness_changed, newEntityId.c_str(), "brightness");
    subscribe_homeassistant_state(&LightService::color_temp_changed, newEntityId.c_str(), "color_temp");
    subscribe_homeassistant_state(&LightService::color_mode_changed, newEntityId.c_str(), "color_mode");
  }
  std::string friendlyName;
  std::string entityId;
  DisplayUpdateInterface &display;
  int localBrightness = -1;
  int localColorTemp = -1;
  bool isBrightnessInSync = false;
  bool isColorTempInSync = false;
  ColorModeType colorMode = unknown_type;
  bool onState;
  int minMireds = 0;
  int maxMireds = 0;

  void decTemperature() {
    if (id(keep_states_in_sync)) {
      // only send update to home assistant if
      // we have received a confirmation for the last
      // state change (state changed from ha)
      if (!isColorTempInSync) {
        return;
      }
      isColorTempInSync = false;
    }
    localColorTemp -= id(dec_color_temperature_step);
    const std::map<std::string, std::string> data = {{"entity_id", entityId.c_str()},
                                                     {"color_temp", to_string(localColorTemp)}};
    setAttribute(data);
  }

  void incTemperature() {
    if (id(keep_states_in_sync)) {
      if (!isColorTempInSync) {
        return;
      }
      isColorTempInSync = false;
    }
    localColorTemp += id(inc_color_temperature_step);
    const std::map<std::string, std::string> data = {
        {"entity_id", entityId.c_str()},
        {"color_temp", to_string(localColorTemp)},
    };
    setAttribute(data);
  }

  void decBrightness() {
    if (id(keep_states_in_sync)) {
      if (!isBrightnessInSync) {
        return;
      }
      isBrightnessInSync = false;
    }
    localBrightness -= id(dec_brightness_step);
    const std::map<std::string, std::string> data = {
        {"entity_id", entityId.c_str()},
        {"brightness", to_string(localBrightness)},
    };
    setAttribute(data);
  }

  void incBrightness() {
    if (id(keep_states_in_sync)) {
      if (!isBrightnessInSync) {
        return;
      }
      isBrightnessInSync = false;
    }
    localBrightness += id(inc_brightness_step);
    const std::map<std::string, std::string> data = {
        {"entity_id", entityId.c_str()},
        {"brightness", to_string(localBrightness)},
    };
    setAttribute(data);
  }

  void setAttribute(const std::map<std::string, std::string> &data) {
    call_homeassistant_service("light.turn_on", data);
  }

  void toggleLight() { call_homeassistant_service("light.toggle", {{"entity_id", entityId.c_str()}}); }

  bool supportsBrightness() { return colorMode != onoff_type && colorMode != unknown_type; }

  bool supportsColorTemperature() { return colorMode == color_temp_type || colorMode == rgbww_type; }

  std::vector<std::shared_ptr<MenuTitleBase>> lightTitleItems() {
    std::vector<std::shared_ptr<MenuTitleBase>> out;
    out.push_back(
        std::make_shared<MenuTitleBase>(friendlyName, entityId, onState ? OnMenuTitleState : OffMenuTitleState));

    std::string s = "Brightness";
    int width_available = id(display_size_x) - 2 * id(slider_margin_size);
    if (supportsBrightness()) {
      float slider_factor = 1;
      if (localBrightness > 0) {
        float percent = ((float) localBrightness / 255.0);
        int percentInt = (int) (percent * 100);
        s += " - " + to_string(percentInt) + " %%";
        slider_factor = width_available / MAX_BRIGHTNESS;
      } else {
      }
      out.push_back(std::make_shared<MenuTitleSlider>("Brightness", s.c_str(), entityId,
                                                      onState ? OnMenuTitleState : OffMenuTitleState,
                                                      (int) (localBrightness * slider_factor)));
    }

    s = "Temperature";
    if (supportsColorTemperature()) {
      float slider_factor = 1;
      if (localColorTemp > 0) {
        s += " - " + to_string(1000000 / localColorTemp) + " K ";
        slider_factor = width_available / (maxMireds - minMireds);
      } else {
      }
      out.push_back(std::make_shared<MenuTitleSlider>("Temperature", s.c_str(), entityId,
                                                      onState ? OnMenuTitleState : OffMenuTitleState,
                                                      (int) (localColorTemp - minMireds * slider_factor)));
    }
    return out;
  }

 private:
  void state_changed(std::string newOnState) {
    ESP_LOGD("state", " changed to %s (%s)", newOnState.c_str(), friendlyName.c_str());
    onState = newOnState == "on";
    // visualize that light is off by resetting brightness and color_temp
    if (id(keep_states_in_sync)) {
      if (!onState) {
        localBrightness = 0;
        localColorTemp = 0;
      }
    }
    display.updateDisplay(false);
  }
  void min_mireds_changed(std::string newOnState) {
    minMireds = atoi(newOnState.c_str());
    display.updateDisplay(false);
  }
  void max_mireds_changed(std::string newOnState) {
    maxMireds = atoi(newOnState.c_str());
    display.updateDisplay(false);
  }
  void brightness_changed(std::string newOnState) {
    ESP_LOGD("brightness", "state changed to %s (%s)", newOnState.c_str(), friendlyName.c_str());
    if (id(keep_states_in_sync) || localBrightness == -1) {
      localBrightness = atoi(newOnState.c_str());
      isBrightnessInSync = true;
    }
    display.updateDisplay(false);
  }
  void color_temp_changed(std::string newOnState) {
    ESP_LOGD("color_temp", "state changed to %s (%s)", newOnState.c_str(), friendlyName.c_str());
    if (id(keep_states_in_sync) || localColorTemp == -1) {
      localColorTemp = atoi(newOnState.c_str());
      isColorTempInSync = true;
    }
    display.updateDisplay(false);
  }
  void color_mode_changed(std::string newOnState) {
    ESP_LOGD("color_mode_changed", "state changed to %s (%s)", newOnState.c_str(), friendlyName.c_str());
    if (strcmp(newOnState.c_str(), "onoff") == 0) {
      colorMode = onoff_type;
    } else if (strcmp(newOnState.c_str(), "brightness") == 0) {
      colorMode = brightness_type;
    } else if (strcmp(newOnState.c_str(), "color_temp") == 0) {
      colorMode = color_temp_type;
    } else if (strcmp(newOnState.c_str(), "hs") == 0) {
      colorMode = hs_type;
    } else if (strcmp(newOnState.c_str(), "rgb") == 0) {
      colorMode = rgb_type;
    } else if (strcmp(newOnState.c_str(), "rgbw") == 0) {
      colorMode = rgbw_type;
    } else if (strcmp(newOnState.c_str(), "rgbww") == 0) {
      colorMode = rgbww_type;
    } else if (strcmp(newOnState.c_str(), "white") == 0) {
      colorMode = white_type;
    } else if (strcmp(newOnState.c_str(), "xy") == 0) {
      colorMode = xy_type;
    } else {
      colorMode = unknown_type;
    }
    display.updateDisplay(false);
  }
};

class LightGroupComponent : public CustomAPIDevice, public Component {
 public:
  explicit LightGroupComponent(DisplayUpdateInterface &newCallback) : display(newCallback) {}
  std::vector<LightService *> lights;

  void setup(std::vector<FriendlyNameEntity> newLights) {
    for (auto &light : newLights) {
      LightService *newService = new LightService(light.friendlyName, light.entityId, display);
      lights.push_back(newService);
    }
  }

  std::vector<std::shared_ptr<MenuTitleBase>> lightTitleSwitches() {
    std::vector<std::shared_ptr<MenuTitleBase>> out;
    for (auto &light : lights) {
      ESP_LOGD("Light", "state %d (%s)", light->onState, light->friendlyName.c_str());
      MenuTitleState state = light->onState ? OnMenuTitleState : OffMenuTitleState;
      out.push_back(std::make_shared<MenuTitleBase>(light->friendlyName, light->entityId, state));
    }
    return out;
  }

  bool selectLight(int index) {
    if (index >= 0 && index < lights.size()) {
      LightService *light = lights[index];
      light->toggleLight();
      return false;
    }
    return true;
  }

  int currentSelectedLight = -1;
  bool lightDetailSelected = false;

 private:
  DisplayUpdateInterface &display;
};
