#include "HomeAssistantSwitchGroup.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_switch_group {
std::vector<std::shared_ptr<MenuTitleBase>> HomeAssistantSwitchGroup::
    switchTitleSwitches() {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  for (auto &switchObject : switches) {
    ESP_LOGI("Switch", "state %d", switchObject->state);
    MenuTitleLeftIcon state =
        switchObject->state ? OnMenuTitleLeftIcon : OffMenuTitleLeftIcon;
    out.push_back(std::make_shared<MenuTitleToggle>(
        switchObject->get_name(), switchObject->getEntityId(), state,
        NoMenuTitleRightIcon));
  }
  return out;
}

bool HomeAssistantSwitchGroup::selectSwitch(int index) {
  if (index >= 0 && index < switches.size()) {
    auto *switchObject = switches[index];
    switchObject->toggleSwitch();
    return false;
  }
  return true;
}

void HomeAssistantSwitchGroup::register_switch(homeassistant_switch::HomeAssistantSwitch* newSwitch) {
  switches.push_back(newSwitch);
}
}  // namespace homeassistant_switch_group
}  // namespace esphome
