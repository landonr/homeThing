#pragma once
#include <string>
#include "DisplayUpdateInterface.h"
#include "FriendlyNameEntity.h"
#include "esphome.h"
#include "esphome/components/homeassistant/binary_sensor/homeassistant_binary_sensor.h"

namespace esphome {
namespace api {
namespace home_assistant_switch {
class HomeAssistantSwitch : public CustomAPIDevice,
                            public homeassistant::HomeassistantBinarySensor {
 public:
  explicit HomeAssistantSwitch(std::string newFriendlyName,
                               std::string newEntityId,
                               DisplayUpdateInterface &newCallback);
  void toggleSwitch();
  std::string getEntityId();

 private:
  DisplayUpdateInterface &display;
};
}  // namespace home_assistant_switch
}  // namespace api
}  // namespace esphome
