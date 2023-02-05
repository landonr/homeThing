#include "HomeAssistantSwitchGroup.h"

namespace home_assistant_switch_group {
using namespace home_assistant_switch;
HomeAssistantSwitchGroup::HomeAssistantSwitchGroup(DisplayUpdateInterface &newCallback) : display(newCallback) {}

void HomeAssistantSwitchGroup::setup(std::vector<FriendlyNameEntity> newSwitches) {
  for (auto &switchObject : newSwitches) {
    HomeAssistantSwitch *newService =
        new HomeAssistantSwitch(switchObject.friendlyName, switchObject.entityId, display);
    App.register_component(newService);
    switches.push_back(newService);
  }
}

std::vector<std::shared_ptr<MenuTitleBase>> HomeAssistantSwitchGroup::switchTitleSwitches() {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  for (auto &switchObject : switches) {
    ESP_LOGI("Switch", "state %d", switchObject->state);
    MenuTitleLeftIcon state = switchObject->state ? OnMenuTitleLeftIcon : OffMenuTitleLeftIcon;
    out.push_back(std::make_shared<MenuTitleToggle>(switchObject->get_name(), switchObject->getEntityId(), state,
                                                    NoMenuTitleRightIcon));
  }
  return out;
}

bool HomeAssistantSwitchGroup::selectSwitch(int index) {
  if (index >= 0 && index < switches.size()) {
    HomeAssistantSwitch *switchObject = switches[index];
    switchObject->toggleSwitch();
    return false;
  }
  return true;
}
}  // namespace home_assistant_switch_group