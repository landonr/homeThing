#pragma once

#include <string>

#include "DisplayUpdateInterface.h"
#include "FriendlyNameEntity.h"
#include "esphome/components/api/api_server.h"
#include "esphome/core/component.h"

namespace esphome {
namespace homeassistant_switch {

class HomeAssistantSwitch : public switch_::Switch, public Component {
 public:
  HomeAssistantSwitch();
  void set_entity_id(const std::string& entity_id) { entity_id_ = entity_id; }
  void set_attribute(const std::string& attribute) { attribute_ = attribute; }
  void setup() override;
  void write_state(bool state) override;
  void toggleSwitch();
  std::string getEntityId();

 private:
  std::string entity_id_;
  optional<std::string> attribute_;
  DisplayUpdateInterface* display;
  void state_changed(std::string newOnState);
  void subscribeState();
};

}  // namespace homeassistant_switch
}  // namespace esphome
