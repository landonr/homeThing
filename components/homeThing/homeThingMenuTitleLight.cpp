#include "esphome/components/homeThing/homeThingMenuTitleLight.h"

namespace esphome {
namespace homething_menu_base {

void lightTitleSwitches(const std::vector<light::LightState*>& lights,
                        std::vector<MenuTitleBase*>* menu_titles) {
  for (const auto& light : lights) {
    auto output = static_cast<light::LightOutput*>(light->get_output());
    ESP_LOGD(MENU_TITLE_LIGHT_TAG, "state %d (%s)", light,
             light->get_name().c_str());
    MenuTitleLeftIcon state = light->remote_values.is_on()
                                  ? OnMenuTitleLeftIcon
                                  : OffMenuTitleLeftIcon;
    MenuTitleRightIcon rightIcon = light::supportsBrightness(light)
                                       ? ArrowMenuTitleRightIcon
                                       : NoMenuTitleRightIcon;
    const auto color = light::rgbLightColor(light);
    menu_titles->push_back(
        new MenuTitleLight(light->get_name(), "", state, rightIcon, color));
  }
}

void lightTitleItems(light::LightState* light,
                     std::vector<MenuTitleBase*>* menu_titles) {
  auto output = light->get_output();
  if (light::supportsBrightness(light)) {
    auto is_on = light->remote_values.is_on();
    int brightness =
        !is_on ? 0
               : static_cast<int>(light->remote_values.get_brightness() * 255);
    menu_titles->push_back(
        MenuTitleSlider::makeSlider("Brightness", "%%", light->get_object_id(),
                                    0, MAX_BRIGHTNESS, brightness, 0, 100));
  }
  if (light::supportsColorTemperature(light)) {
    auto max_mireds = light->get_traits().get_max_mireds();
    auto min_mireds = light->get_traits().get_min_mireds();
    menu_titles->push_back(MenuTitleSlider::makeSlider(
        "Temperature", "K", light->get_object_id(), min_mireds, max_mireds,
        light->remote_values.get_color_temperature(), 1000000 / min_mireds,
        1000000 / max_mireds));
  }
  if (light::supportsColor(light)) {
    menu_titles->push_back(
        MenuTitleSlider::makeSlider("Color", "", light->get_object_id(), 0, 360,
                                    get_hsv_color(light), 0, 360));
  }
}

}  // namespace homething_menu_base
}  // namespace esphome
