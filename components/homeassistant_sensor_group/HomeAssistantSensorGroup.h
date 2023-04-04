#pragma once
#include <memory>
#include <vector>

#include "esphome/components/homeassistant/text_sensor/homeassistant_text_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace homeassistant_sensor_group {
class HomeAssistantSensorGroup : public Component, public sensor::Sensor {
 public:
  std::vector<esphome::homeassistant::HomeassistantTextSensor*> sensors;
  float get_setup_priority() const override { return setup_priority::LATE; }
  void register_sensor(homeassistant::HomeassistantTextSensor* newSensor);
};

}  // namespace homeassistant_sensor_group
}  // namespace esphome
