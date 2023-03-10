#include "HomeAssistantSensorGroup.h"
#include <string>
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_sensor_group {
void HomeAssistantSensorGroup::register_sensor(
    homeassistant::HomeassistantTextSensor* newSensor) {
  sensors.push_back(newSensor);
  newSensor->add_on_state_callback(
      [this, newSensor](std::string state) { this->publish_state(0); });
}
}  // namespace homeassistant_sensor_group
}  // namespace esphome
