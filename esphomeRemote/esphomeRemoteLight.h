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
    subscribe_homeassistant_state(&LightComponent::color_changed, newEntityId.c_str(), "hs_color");
    subscribe_homeassistant_state(&LightComponent::supported_color_modes_changed, newEntityId.c_str(), "color_mode");
    subscribe_homeassistant_state(&LightComponent::supported_color_modes_changed, newEntityId.c_str(),
                                  "supported_color_modes");
  }
  std::string friendlyName;
  std::string entityId;
  DisplayUpdateInterface &display;
  int localBrightness = -1;
  int localColorTemp = -1;
  int localColor = -1;
  bool isBrightnessInSync = false;
  bool isColorTempInSync = false;
  std::vector<ColorModeType> supportedColorModes = {};
  bool onState;
  int minMireds = 0;
  int maxMireds = 0;

  void increaseProperty(int max, bool &inSync, int &localValue, int incStep, std::string serviceProperty,
                        bool wrapData = false) {
    if (id(keep_states_in_sync)) {
      if (!inSync) {
        return;
      }
      inSync = false;
    }
    if (localValue + incStep >= max) {
      localValue = max;
    } else {
      localValue += incStep;
    }
    if (wrapData) {
      const std::map<std::string, std::string> data = {
          {"entity_id", entityId.c_str()},
          {"hue", to_string(localValue).c_str()},
          {"saturation", "100"},
      };
      call_homeassistant_service("script.hs_light_set", data);
    } else {
      const std::map<std::string, std::string> data = {
          {"entity_id", entityId.c_str()},
          {serviceProperty, to_string(localValue).c_str()},
      };
      setAttribute(data);
    }
  }

  void decreaseProperty(int min, bool &inSync, int &localValue, int decStep, std::string serviceProperty,
                        bool wrapData = false) {
    if (id(keep_states_in_sync)) {
      if (!inSync) {
        return;
      }
      inSync = false;
    }
    if (localValue - decStep <= min) {
      localValue = min;
    } else {
      localValue -= decStep;
    }
    if (wrapData) {
      const std::map<std::string, std::string> data = {
          {"entity_id", entityId.c_str()},
          {"hue", to_string(localValue).c_str()},
          {"saturation", "100"},
      };
      call_homeassistant_service("script.hs_light_set", data);
    } else {
      const std::map<std::string, std::string> data = {
          {"entity_id", entityId.c_str()},
          {serviceProperty, to_string(localValue).c_str()},
      };
      setAttribute(data);
    }
  }

  void decTemperature() {
    decreaseProperty(minMireds, isColorTempInSync, localColorTemp, id(dec_color_temperature_step), "color_temp");
  }

  void incTemperature() {
    increaseProperty(maxMireds, isColorTempInSync, localColorTemp, id(inc_color_temperature_step), "color_temp");
  }

  void decBrightness() {
    decreaseProperty(0, isBrightnessInSync, localBrightness, id(dec_brightness_step), "brightness");
  }

  void incBrightness() {
    increaseProperty(MAX_BRIGHTNESS, isBrightnessInSync, localBrightness, id(inc_brightness_step), "brightness");
  }

  void decColor() {
    bool temp = true;
    decreaseProperty(0, temp, localColor, 10, "hs_color", true);
  }

  void incColor() {
    bool temp = true;
    increaseProperty(360, temp, localColor, 10, "hs_color", true);
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
  bool supportsColor() {
    return (std::find(supportedColorModes.begin(), supportedColorModes.end(), rgb_type) != supportedColorModes.end() ||
            std::find(supportedColorModes.begin(), supportedColorModes.end(), rgbw_type) != supportedColorModes.end() ||
            std::find(supportedColorModes.begin(), supportedColorModes.end(), rgbww_type) !=
                supportedColorModes.end() ||
            std::find(supportedColorModes.begin(), supportedColorModes.end(), hs_type) != supportedColorModes.end() ||
            std::find(supportedColorModes.begin(), supportedColorModes.end(), xy_type) != supportedColorModes.end()) &&
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
    return std::make_shared<MenuTitleSlider>(title.c_str(), sliderTitle.c_str(), entityId, NoMenuTitleRightIcon,
                                             sliderValue);
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

    s = "Color";
    if (supportsColor()) {
      out.push_back(makeSlider(0, 360, localColor, "Color", "", 0, 360));
    }
    return out;
  }

  Color hsvToRGB(double hue, double saturation, double value) {
    double r = 0, g = 0, b = 0;
    if (saturation == 0) {
      r = value;
      g = value;
      b = value;
    } else {
      int i;
      double f, p, q, t;

      if (hue == 360)
        hue = 0;
      else
        hue = hue / 60;

      i = (int) trunc(hue);
      f = hue - i;

      p = value * (1.0 - saturation);
      q = value * (1.0 - (saturation * f));
      t = value * (1.0 - (saturation * (1.0 - f)));

      switch (i) {
        case 0:
          r = value;
          g = t;
          b = p;
          break;
        case 1:
          r = q;
          g = value;
          b = p;
          break;
        case 2:
          r = p;
          g = value;
          b = t;
          break;
        case 3:
          r = p;
          g = q;
          b = value;
          break;
        case 4:
          r = t;
          g = p;
          b = value;
          break;
        default:
          r = value;
          g = p;
          b = q;
          break;
      }
    }
    return Color((unsigned char) (r * 255), (unsigned char) (g * 255), (unsigned char) (b * 255));
  }

  Color rgbLightColor() {
    if (!supportsBrightness()) {
      return Color(255, 255, 255);
    }

    return hsvToRGB((double) localColor, 1, 1);
    // return hsvToRGB((double) localColor, 1, ((double) localBrightness) / 100);
  }

  std::string icon() {
    if (onState != OnMenuTitleLeftIcon) {
      return "󰌵";
    } else {
      return "󰌶";
    }
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
  void color_changed(std::string newOnState) {
    ESP_LOGI("color", "state changed to %s (%s)", newOnState.c_str(), friendlyName.c_str());
    if (id(keep_states_in_sync) || localColor == -1) {
      localColor = TextHelpers::extractFirstNumber(newOnState);
    }
    display.updateDisplay(false);
  }

  void supported_color_modes_changed(std::string newOnState) {
    // add all modes for the light to supportedColorModes and then check
    // the vector if the light supports a particular mode when needed
    ESP_LOGI("supported_color_modes_changed", "state changed to %s (%s)", newOnState.c_str(), friendlyName.c_str());
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
      out.push_back(std::make_shared<MenuTitleLight>(light->friendlyName, light->entityId, state, rightIcon,
                                                     light->rgbLightColor()));
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
