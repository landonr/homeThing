#pragma once
#include <memory>
#include <vector>
#include "DisplayUpdateInterface.h"
#include "MenuTitle.h"
#include "esphome/components/homeassistant_switch/HomeAssistantSwitch.h"
#include "esphome/core/component.h"

namespace esphome {
namespace homeassistant_switch_group {

class HomeAssistantSwitchGroup : public api::CustomAPIDevice, public Component {
 public:
  // std::vector<HomeAssistantSwitch *> switches;

  void setup(std::vector<FriendlyNameEntity> newSwitches);
  std::vector<std::shared_ptr<MenuTitleBase>> switchTitleSwitches();
  bool selectSwitch(int index);

 private:
  DisplayUpdateInterface *display;
};

}  // namespace homeassistant_switch
}  // namespace esphome