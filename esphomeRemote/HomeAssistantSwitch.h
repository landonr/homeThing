#pragma once
#include "esphome.h"
#include "DisplayUpdateInterface.h"
#include "MenuTitle.h"
#include "FriendlyNameEntity.h"
#include "esphome/components/homeassistant/binary_sensor/homeassistant_binary_sensor.h"

namespace home_assistant_switch {
using namespace api;
class HomeAssistantSwitch : public CustomAPIDevice, public homeassistant::HomeassistantBinarySensor {
 public:
  explicit HomeAssistantSwitch(std::string newFriendlyName, std::string newEntityId,
                               DisplayUpdateInterface &newCallback);
  void toggleSwitch();
  std::string getEntityId();

 private:
  DisplayUpdateInterface &display;
};
}  // namespace home_assistant_switch
