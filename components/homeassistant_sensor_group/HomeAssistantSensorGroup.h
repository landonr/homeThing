#pragma once
#include <memory>
#include <vector>

#include "DisplayUpdateInterface.h"
#include "MenuTitle.h"
#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/homeassistant/text_sensor/homeassistant_text_sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace homeassistant_sensor_group {
class HomeAssistantSensorGroup : public api::CustomAPIDevice, public Component {
 public:
  std::vector<esphome::homeassistant::HomeassistantTextSensor*> sensors;
  float get_setup_priority() const override { return setup_priority::LATE; }
  std::vector<std::shared_ptr<MenuTitleBase>> sensorTitles();
  void register_sensor(homeassistant::HomeassistantTextSensor* newSensor);
  void set_display(DisplayUpdateInterface* newDisplay) { display = newDisplay; }

 private:
  DisplayUpdateInterface* display;
};

}  // namespace homeassistant_sensor_group
}  // namespace esphome
