#pragma once
#include <memory>
#include <vector>

#include "esphome/components/homeassistant_light_group/light/HomeAssistantLight.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace homeassistant_light_group {

class HomeAssistantLightGroup : public Component, public sensor::Sensor {
 public:
  std::vector<homeassistant_light::HomeAssistantLightState*> lights;
  bool selectLightDetailAtIndex(int index);
  void clearActiveLight() { _activeLight = NULL; }
  homeassistant_light::HomeAssistantLight* getActiveLight() {
    return static_cast<homeassistant_light::HomeAssistantLight*>(
        _activeLight->get_output());
  }
  bool lightDetailSelected = false;
  void register_light(homeassistant_light::HomeAssistantLightState* newLight);
  void toggleLight(int index);
  float get_setup_priority() const override { return setup_priority::LATE; }

 private:
  homeassistant_light::HomeAssistantLightState* _activeLight = NULL;
};

}  // namespace homeassistant_light_group
}  // namespace esphome
