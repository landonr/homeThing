#pragma once

#include <string>

#include "esphome/components/homeassistant_component/HomeAssistantComponent.h"
#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

namespace esphome {
namespace homeassistant_switch {

class HomeAssistantSwitch
    : public switch_::Switch,
      public homeassistant_component::HomeAssistantComponent {
 public:
  void set_attribute(const std::string& attribute) { attribute_ = attribute; }
  void setup() override;
  void write_state(bool state) override;
  void toggleSwitch();

 private:
  optional<std::string> attribute_;
  void state_changed(std::string newOnState);
};

}  // namespace homeassistant_switch
}  // namespace esphome
