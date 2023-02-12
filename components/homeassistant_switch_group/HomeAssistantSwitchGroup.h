#pragma once
#include <memory>
#include <vector>
#include "DisplayUpdateInterface.h"
#include "MenuTitle.h"
#include "esphome/components/homeassistant_switch_group/switch/HomeAssistantSwitch.h"
#include "esphome/core/component.h"
#include "esphome/components/api/custom_api_device.h"

namespace esphome {
namespace homeassistant_switch_group {
class HomeAssistantSwitchGroup : public api::CustomAPIDevice, public Component {
 public:
  std::vector<esphome::homeassistant_switch::HomeAssistantSwitch *> switches;

  std::vector<std::shared_ptr<MenuTitleBase>> switchTitleSwitches();
  bool selectSwitch(int index);
  void register_switch(homeassistant_switch::HomeAssistantSwitch *newSwitch);

 private:
  DisplayUpdateInterface *display;
};

}  // namespace homeassistant_switch_group
}  // namespace esphome