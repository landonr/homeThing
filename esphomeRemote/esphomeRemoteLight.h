#include "esphome.h"
#include <algorithm>
#include "DisplayUpdateInterface.h"
#include "MenuTitle.h"
#include "FriendlyNameEntity.h"
#include "esphomeRemoteCommon.h"

#pragma once

#define MAX_BRIGHTNESS 255.0f

enum ColorModeType {
  // unknown_type, <- we only need supported modes
  // onoff_type, <- we only need supported modes
  brightness_type,
  color_temp_type,
  hs_type,
  rgb_type,
  rgbw_type,
  rgbww_type,
  white_type,
  xy_type
};

class LightComponent : public CustomAPIDevice, public Component {
 public:
  LightComponent(std::string newFriendlyName, std::string newEntityId, DisplayUpdateInterface &newCallback)
      : friendlyName(newFriendlyName), entityId(newEntityId), display(newCallback) {
    onState = false;
    subscribe_homeassistant_state(&LightComponent::state_changed, newEntityId.c_str());
    subscribe_homeassistant_state(&LightComponent::min_mireds_changed, newEntityId.c_str(), "min_mireds");
    subscribe_homeassistant_state(&LightComponent::max_mireds_changed, newEntityId.c_str(), "max_mireds");
    subscribe_homeassistant_state(&LightComponent::brightness_changed, newEntityId.c_str(), "brightness");
    subscribe_homeassistant_state(&LightComponent::color_temp_changed, newEntityId.c_str(), "color_temp");
    subscribe_homeassistant_state(&LightComponent::supported_color_modes_changed, newEntityId.c_str(), "color_mode");
    subscribe_homeassistant_state(&LightComponent::supported_color_modes_changed, newEntityId.c_str(),
                                  "supported_color_modes");
  }
  std::string friendlyName;
  std::string entityId;
  DisplayUpdateInterface &display;
  int localBrightness = -1;
  int localColorTemp = -1;
  bool isBrightnessInSync = false;
  bool isColorTempInSync = false;
  std::vector<ColorModeType> supportedColorModes = {};
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
    if (localColorTemp <= minMireds) {
      localColorTemp = minMireds;
      return;
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
    if (localColorTemp >= maxMireds) {
      localColorTemp = maxMireds;
      return;
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
    if (localBrightness <= 0) {
      localBrightness = 0;
      return;
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
    if (localBrightness >= MAX_BRIGHTNESS) {
      localBrightness = MAX_BRIGHTNESS;
      return;
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

  bool supportsBrightness() {
    // as long as there is one mode inside of supportedColorModes brightness is supported
    return !supportedColorModes.empty();
  }
  bool supportsColorTemperature() {
    // TODO: I think the color_temp for rgbww cant be controlled if in white mode -> investigate and fix
    return (std::find(supportedColorModes.begin(), supportedColorModes.end(), color_temp_type) !=
                supportedColorModes.end() ||
            std::find(supportedColorModes.begin(), supportedColorModes.end(), rgbww_type) !=
                supportedColorModes.end()) &&
           !supportedColorModes.empty();
  }

  std::shared_ptr<MenuTitleSlider> makeSlider(int min, int max, int value, std::string title, std::string unit,
                                              int displayUnitMin, int displayUnitMax) {
    std::string sliderTitle = title;
    float oldRange = max - min;
    float valueMinusMin = value - min;
    if (value > 0) {
      float displayNewRange = displayUnitMax - displayUnitMin;
      int displayValue = (float) ((valueMinusMin * displayNewRange) / oldRange) + displayUnitMin;
      sliderTitle += " - " + to_string(displayValue) + " " + unit;
    }

    float newMin = id(slider_margin_size);
    float newRange = id(display_size_x) - 4 * newMin;
    int sliderValue = ((valueMinusMin * newRange) / oldRange) + newMin;
    return std::make_shared<MenuTitleSlider>(title.c_str(), sliderTitle.c_str(), entityId, NoMenuTitleLeftIcon,
                                             NoMenuTitleRightIcon, sliderValue);
  }

  std::vector<std::shared_ptr<MenuTitleBase>> lightTitleItems() {
    std::vector<std::shared_ptr<MenuTitleBase>> out;
    std::string s = "Brightness";
    int widthAvailable = id(display_size_x) - 2 * id(slider_margin_size);
    if (supportsBrightness()) {
      out.push_back(makeSlider(0, MAX_BRIGHTNESS, localBrightness, "Brightness", "%%", 0, 100));
    }

    s = "Temperature";
    if (supportsColorTemperature()) {
      out.push_back(makeSlider(minMireds, maxMireds, localColorTemp, "Temperature", "K", 1000000 / minMireds,
                               1000000 / maxMireds));
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
    ESP_LOGI("color_temp", "state changed to %s (%s)", newOnState.c_str(), friendlyName.c_str());
    if (id(keep_states_in_sync) || localColorTemp == -1) {
      localColorTemp = atoi(newOnState.c_str());
      isColorTempInSync = true;
    }
    display.updateDisplay(false);
  }

  void supported_color_modes_changed(std::string newOnState) {
    // add all modes for the light to supportedColorModes and then check
    // the vector if the light supports a particular mode when needed
    ESP_LOGD("supported_color_modes_changed", "state changed to %s (%s)", newOnState.c_str(), friendlyName.c_str());
    auto modes = StringUtils::split(newOnState, ",");
    for (auto cmode : modes) {
      ESP_LOGD("supported_color_modes_changed", "mode: %s (%s)", newOnState.c_str(), friendlyName.c_str());
      if (cmode.find("onoff") != std::string::npos) {
        // ignore onoff_type
        // supportedColorModes.push_back(onoff_type);
      } else if (newOnState.find("brightness") != std::string::npos) {
        supportedColorModes.push_back(brightness_type);
      } else if (cmode.find("color_temp") != std::string::npos) {
        supportedColorModes.push_back(color_temp_type);
      } else if (cmode.find("hs") != std::string::npos) {
        supportedColorModes.push_back(hs_type);
      } else if (cmode.find("rgb") != std::string::npos) {
        supportedColorModes.push_back(rgb_type);
      } else if (cmode.find("rgbw") != std::string::npos) {
        supportedColorModes.push_back(rgbw_type);
      } else if (cmode.find("rgbww") != std::string::npos) {
        supportedColorModes.push_back(rgbww_type);
      } else if (cmode.find("white") != std::string::npos) {
        supportedColorModes.push_back(white_type);
      } else if (cmode.find("xy") != std::string::npos) {
        supportedColorModes.push_back(xy_type);
      }
    }
    display.updateDisplay(false);
  }
};

class LightGroupComponent : public CustomAPIDevice, public Component {
 public:
  explicit LightGroupComponent(DisplayUpdateInterface &newCallback) : display(newCallback) {}
  std::vector<LightComponent *> lights;

  void setup(std::vector<FriendlyNameEntity> newLights) {
    for (auto &light : newLights) {
      LightComponent *newService = new LightComponent(light.friendlyName, light.entityId, display);
      lights.push_back(newService);
    }
  }

  std::vector<std::shared_ptr<MenuTitleBase>> lightTitleSwitches() {
    std::vector<std::shared_ptr<MenuTitleBase>> out;
    for (auto &light : lights) {
      ESP_LOGD("Light", "state %d (%s)", light->onState, light->friendlyName.c_str());
      MenuTitleLeftIcon state = light->onState ? OnMenuTitleLeftIcon : OffMenuTitleLeftIcon;
      MenuTitleRightIcon rightIcon = light->supportsBrightness() ? ArrowMenuTitleRightIcon : NoMenuTitleRightIcon;
      out.push_back(std::make_shared<MenuTitleBase>(light->friendlyName, light->entityId, state, rightIcon));
    }
    return out;
  }

  void selectLightAtIndex(int index) {
    if (lights.size() < index) {
      ESP_LOGE("light", "selecting out of bounds light");
      return;
    }
    _activeLight = lights[index];
  }

  void clearActiveLight() { _activeLight = NULL; }

  LightComponent *getActiveLight() { return _activeLight; }

  bool lightDetailSelected = false;

 private:
  DisplayUpdateInterface &display;
  LightComponent *_activeLight = NULL;
};
