#include "HomeAssistantSensorGroup.h"
#include <string>
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_sensor_group {
std::vector<std::shared_ptr<MenuTitleBase>>
HomeAssistantSensorGroup::sensorTitles() {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  for (auto& sensor : sensors) {
    if (sensor->get_name() != "") {
      out.push_back(std::make_shared<MenuTitleBase>(
          sensor->get_name() + " " + sensor->get_state(), "",
          NoMenuTitleRightIcon));
    } else {
      out.push_back(std::make_shared<MenuTitleBase>(sensor->state, "",
                                                    NoMenuTitleRightIcon));
    }
  }
  return out;
}

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
