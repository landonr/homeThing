#pragma once
#include <string>
#include "DisplayUpdateInterface.h"
#include "FriendlyNameEntity.h"
#include "esphome.h"
#include "esphome/components/homeassistant/binary_sensor/homeassistant_binary_sensor.h"

namespace esphome {
namespace home_assistant_switch {
class HomeAssistantSwitch : public api::CustomAPIDevice,
                            public Component,
                            public switch_::Switch {
 public:
  explicit HomeAssistantSwitch(std::string newFriendlyName,
                               std::string newEntityId,
                               DisplayUpdateInterface *newCallback);
  void set_entity_id(const std::string &entity_id) { entity_id_ = entity_id; }
  void set_attribute(const std::string &attribute) { attribute_ = attribute; }
  void setup() override;
  void write_state(bool state) override;
  void toggleSwitch();
  std::string getEntityId();

 private:
  std::string entity_id_;
  optional<std::string> attribute_;
  DisplayUpdateInterface *display;
  void state_changed(std::string newOnState);
};
}  // namespace home_assistant_switch
}  // namespace esphome
