#pragma once
#include "esphome.h"
#include "DisplayUpdateInterface.h"
#include "MenuTitle.h"
#include "FriendlyNameEntity.h"

namespace home_assistant_switch_group {
using namespace home_assistant_switch;

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
