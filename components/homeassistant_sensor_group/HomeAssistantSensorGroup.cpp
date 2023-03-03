#include "HomeAssistantSensorGroup.h"
#include <string>
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_sensor_group {
void HomeAssistantSensorGroup::register_sensor(
    homeassistant::HomeassistantTextSensor* newSensor) {
  sensors.push_back(newSensor);
  newSensor->add_on_state_callback([this, newSensor](std::string state) {
    if (this->display != NULL) {
      this->display->updateDisplay(false);
    }
  });
}
}  // namespace homeassistant_sensor_group
}  // namespace esphome
