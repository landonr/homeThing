#include "HomeAssistantSwitchGroup.h"

#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_switch_group {
bool HomeAssistantSwitchGroup::selectSwitch(int index) {
  if (index >= 0 && index < switches.size()) {
    auto* switchObject = switches[index];
    switchObject->toggleSwitch();
    return false;
  }
  return true;
}

void HomeAssistantSwitchGroup::register_switch(
    homeassistant_switch::HomeAssistantSwitch* newSwitch) {
  switches.push_back(newSwitch);
  newSwitch->add_on_state_callback(
      [this, newSwitch](bool state) { this->publish_state(0); });
}
}  // namespace homeassistant_switch_group
}  // namespace esphome
