#pragma once

#ifdef USE_LIGHT
#include <string>
#include <vector>
#include "esphome/components/homeThing/homeThingMenuTitleSlider.h"
#include "esphome/components/homeThing/homeThingMenuTitleToggle.h"
#include "esphome/components/homeassistant_component/LightExtensions.h"

namespace esphome {
namespace homething_menu_base {

static const char* const MENU_TITLE_LIGHT_TAG = "homething.menutitle.light";

// light
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

// static void lightTitleSwitches(const std::vector<light::LightState*>& lights,
//                                std::vector<MenuTitleBase*>* menu_titles) {
//   for (const auto& light : lights) {
//     auto output = static_cast<light::LightOutput*>(light->get_output());
//     ESP_LOGD(MENU_TITLE_LIGHT_TAG, "state %d (%s)",
//              light->remote_values.is_on(), light->get_name().c_str());
//     MenuTitleLeftIcon state = light->remote_values.is_on()
//                                   ? OnMenuTitleLeftIcon
//                                   : OffMenuTitleLeftIcon;
//     MenuTitleRightIcon rightIcon = supportsBrightness(light)
//                                        ? ArrowMenuTitleRightIcon
//                                        : NoMenuTitleRightIcon;
//     const auto color = rgbLightColor(light);
//     menu_titles->push_back(
//         new MenuTitleLight(light->get_name(), "", state, rightIcon, color));
//   }
// }

static void lightTitleItems(light::LightState* light,
                            std::vector<MenuTitleBase*>* menu_titles) {
  auto output = light->get_output();
  if (supportsBrightness(light)) {
    auto is_on = light->remote_values.is_on();
    int brightness =
        !is_on ? 0
               : static_cast<int>(light->remote_values.get_brightness() * 255);
    menu_titles->push_back(
        new MenuTitleSlider("Brightness", "%%", light->get_object_id(), 0,
                            MAX_BRIGHTNESS, brightness, 0, 100));
  }
  if (supportsColorTemperature(light)) {
    auto max_mireds = light->get_traits().get_max_mireds();
    auto min_mireds = light->get_traits().get_min_mireds();
    menu_titles->push_back(new MenuTitleSlider(
        "Temperature", "K", light->get_object_id(), min_mireds, max_mireds,
        light->remote_values.get_color_temperature(), 1000000 / min_mireds,
        1000000 / max_mireds));
  }
  if (supportsColor(light)) {
    menu_titles->push_back(new MenuTitleSlider("Color", "",
                                               light->get_object_id(), 0, 360,
                                               get_hsv_color(light), 0, 360));
  }
  if (light->supports_effects()) {
    menu_titles->push_back(
        new MenuTitleToggle("Effect", light->get_effect_name(),
                            NoMenuTitleLeftIcon, ArrowMenuTitleRightIcon));
    if (light->get_effects().size() > 0) {
      for (const auto& effect : light->get_effects()) {
        menu_titles->push_back(
            new MenuTitleToggle(effect->get_name(), effect->get_name(),
                                NoMenuTitleLeftIcon, ArrowMenuTitleRightIcon));
      }
    }
  }
}

}  // namespace homething_menu_base
}  // namespace esphome

#endif
