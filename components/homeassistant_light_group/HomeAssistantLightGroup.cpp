#include "HomeAssistantLightGroup.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_light_group {

static const char* const TAG = "homeassistant.light.group";

bool HomeAssistantLightGroup::selectLightDetailAtIndex(int index) {
  if (lights.size() < index) {
    ESP_LOGE(TAG, "selecting out of bounds light");
    return false;
  }
  auto new_active_light = lights[index];
  auto output = static_cast<homeassistant_light::HomeAssistantLight*>(
      new_active_light->get_output());
  if (output->supportsBrightness()) {
    _activeLight = new_active_light;
    return true;
  }
  return false;
}

void HomeAssistantLightGroup::register_light(
    homeassistant_light::HomeAssistantLightState* newLight) {
  lights.push_back(newLight);
  auto output = static_cast<homeassistant_light::HomeAssistantLight*>(
      newLight->get_output());
  output->add_on_state_callback([this, output]() {
    ESP_LOGI(TAG, "'%s': new output state", output->get_name().c_str());
    this->publish_state(0);
    // if (this->display != NULL) {
    //   this->display->updateDisplay(false);
    // }
  });
}

void HomeAssistantLightGroup::toggleLight(int index) {
  if (lights.size() < index) {
    return;
  }
  auto light = lights[index];
  auto output = static_cast<homeassistant_light::HomeAssistantLight*>(
      light->get_output());
  if (output == NULL) {
    return;
  }
  output->toggle();
}

}  // namespace homeassistant_light_group
}  // namespace esphome
