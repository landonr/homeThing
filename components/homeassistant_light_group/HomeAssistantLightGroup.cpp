#include "HomeAssistantLightGroup.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_light_group {

std::vector<std::shared_ptr<MenuTitleBase>>
HomeAssistantLightGroup::lightTitleSwitches() {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  for (auto& light : lights) {
    auto output = static_cast<homeassistant_light::HomeAssistantLight*>(
        light->get_output());
    ESP_LOGD("Light", "state %d (%s)", output->onState,
             light->get_name().c_str());
    MenuTitleLeftIcon state =
        output->get_state() ? OnMenuTitleLeftIcon : OffMenuTitleLeftIcon;
    MenuTitleRightIcon rightIcon = output->supportsBrightness()
                                       ? ArrowMenuTitleRightIcon
                                       : NoMenuTitleRightIcon;
    out.push_back(std::make_shared<MenuTitleLight>(
        light->get_name(), "", state, rightIcon, output->rgbLightColor()));
  }
  return out;
}

void HomeAssistantLightGroup::selectLightAtIndex(int index) {
  if (lights.size() < index) {
    ESP_LOGE("light", "selecting out of bounds light");
    return;
  }
  _activeLight = lights[index];
}

void HomeAssistantLightGroup::register_light(
    homeassistant_light::HomeAssistantLightState* newLight) {
  lights.push_back(newLight);
  auto output = static_cast<homeassistant_light::HomeAssistantLight*>(
      newLight->get_output());
  output->add_on_state_callback([this, output]() {
    if (this->display != NULL) {
      this->display->updateDisplay(false);
    }
  });
}

void HomeAssistantLightGroup::toggleLight(int index) {
  if (lights.size() < index) {
    return;
  }
  auto light = lights[index];
  auto output = static_cast<homeassistant_light::HomeAssistantLight*>(light->get_output());
  if(output == NULL) {
    return;
  }
  output->next_api_publish_ = true;
  auto call = light->toggle();
  call.perform();
}

}  // namespace homeassistant_light_group
}  // namespace esphome
