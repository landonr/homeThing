#pragma once
#include <memory>
#include <vector>

#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/homeassistant_switch_group/switch/HomeAssistantSwitch.h"
#include "esphome/core/component.h"

namespace esphome {
namespace homeassistant_switch_group {
class HomeAssistantSwitchGroup : public api::CustomAPIDevice, public Component {
 public:
  std::vector<esphome::homeassistant_switch::HomeAssistantSwitch*> switches;
  float get_setup_priority() const override { return setup_priority::LATE; }
  bool selectSwitch(int index);
  void register_switch(homeassistant_switch::HomeAssistantSwitch* newSwitch);
  // void set_display(DisplayUpdateInterface* newDisplay) { display = newDisplay; }

 private:
  // DisplayUpdateInterface* display;
};

}  // namespace homeassistant_switch_group
}  // namespace esphome
