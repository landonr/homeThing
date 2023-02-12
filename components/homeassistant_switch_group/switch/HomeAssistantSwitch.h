#pragma once

#include <string>

#include "DisplayUpdateInterface.h"
#include "FriendlyNameEntity.h"

#include "esphome/components/api/custom_api_device.h"
#include "esphome/core/component.h"

namespace esphome {
namespace homeassistant_switch {

class HomeAssistantSwitch : public switch_::Switch, public Component {
 public:
  void set_entity_id(const std::string& entity_id) { entity_id_ = entity_id; }
  void set_attribute(const std::string& attribute) { attribute_ = attribute; }
  void setup() override;
  void write_state(bool state) override;
  void toggleSwitch();
  std::string get_entity_id();
  float get_setup_priority() const override { return setup_priority::LATE; }

 private:
  std::string entity_id_;
  optional<std::string> attribute_;
  DisplayUpdateInterface* display;
  void state_changed(std::string newOnState);
};

}  // namespace homeassistant_switch
}  // namespace esphome
