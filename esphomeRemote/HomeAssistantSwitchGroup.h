#pragma once
#include "HomeAssistantSwitch.h"
#include "MenuTitle.h"

using namespace esphome;
using namespace api;
using namespace home_assistant_switch;

namespace home_assistant_switch_group {

class HomeAssistantSwitchGroup : public CustomAPIDevice, public Component {
 public:
  explicit HomeAssistantSwitchGroup(DisplayUpdateInterface &newCallback);
  std::vector<HomeAssistantSwitch *> switches;

  void setup(std::vector<FriendlyNameEntity> newSwitches);
  std::vector<std::shared_ptr<MenuTitleBase>> switchTitleSwitches();
  bool selectSwitch(int index);

 private:
  DisplayUpdateInterface &display;
};

}  // namespace home_assistant_switch_group
