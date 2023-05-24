#pragma once

#ifdef USE_LIGHT_GROUP
#include "esphome/components/homeassistant_light_group/HomeAssistantLightGroup.h"
#include "esphome/components/homeassistant_light_group/LightExtensions.h"
#endif
#include "esphome/components/homeThing/homeThingMenuTitle.h"

namespace esphome {
namespace homething_menu_base {

static const char* const MENU_TITLE_LIGHT_TAG = "homething.menutitle.light";

// light
#ifdef USE_LIGHT_GROUP
#define MAX_BRIGHTNESS 255.0f

class MenuTitleLight : public MenuTitleToggle {

 public:
  Color lightColor;

  MenuTitleLight(std::string new_name, std::string newEntityId,
                 MenuTitleLeftIcon newLeftIconState,
                 MenuTitleRightIcon newRightIconState, Color newLightColor)
      : MenuTitleToggle{new_name, newEntityId, newLeftIconState,
                        newRightIconState, LightMenuTitleType},
        lightColor(newLightColor) {}
};

static std::vector<std::shared_ptr<MenuTitleBase>> lightTitleSwitches(
    const std::vector<light::LightState*>& lights) {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  for (auto& light : lights) {
    auto output = static_cast<light::LightOutput*>(light->get_output());
    ESP_LOGD(MENU_TITLE_LIGHT_TAG, "state %d (%s)", light,
             light->get_name().c_str());
    MenuTitleLeftIcon state = light->remote_values.is_on()
                                  ? OnMenuTitleLeftIcon
                                  : OffMenuTitleLeftIcon;
    MenuTitleRightIcon rightIcon = supportsBrightness(light)
                                       ? ArrowMenuTitleRightIcon
                                       : NoMenuTitleRightIcon;
    out.push_back(std::make_shared<MenuTitleLight>(
        light->get_name(), "", state, rightIcon, rgbLightColor(light)));
  }
  return out;
}

static std::shared_ptr<MenuTitleSlider> makeSlider(
    std::string title, std::string unit, std::string entity_id_, int min,
    int max, int value, int displayUnitMin, int displayUnitMax) {
  int displayValue = value;
  float oldRange = max - min;
  float valueMinusMin = value - min;
  if (value > 0) {
    float displayNewRange = displayUnitMax - displayUnitMin;
    displayValue =
        static_cast<float>(((valueMinusMin * displayNewRange) / oldRange)) +
        displayUnitMin;
  }

  // float newMin = display_state_->get_slider_margin_size();
  float newMin = 8;
  // float newRange = displayWidth - 4 * newMin;
  float newRange = 100;
  int sliderValue = ((valueMinusMin * newRange) / oldRange) + newMin;
  return std::make_shared<MenuTitleSlider>(title.c_str(), entity_id_,
                                           NoMenuTitleRightIcon, value,
                                           displayValue, unit, min, max);
}

static std::vector<std::shared_ptr<MenuTitleBase>> lightTitleItems(
    light::LightState* light) {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  auto output = light->get_output();
  if (supportsBrightness(light)) {
    auto is_on = light->remote_values.is_on();
    int brightness =
        !is_on ? 0
               : static_cast<int>(light->remote_values.get_brightness() * 255);
    out.push_back(makeSlider("Brightness", "%%", light->get_object_id(), 0,
                             MAX_BRIGHTNESS, brightness, 0, 100));
  }
  if (supportsColorTemperature(light)) {
    auto max_mireds = light->get_traits().get_max_mireds();
    auto min_mireds = light->get_traits().get_min_mireds();
    out.push_back(makeSlider("Temperature", "K", light->get_object_id(),
                             min_mireds, max_mireds,
                             light->remote_values.get_color_temperature(),
                             1000000 / min_mireds, 1000000 / max_mireds));
  }
  if (supportsColor(light)) {
    out.push_back(makeSlider("Color", "", light->get_object_id(), 0, 360,
                             get_hsv_color(light), 0, 360));
  }
  return out;
}

#endif  // light

}  // namespace homething_menu_base
}  // namespace esphome
