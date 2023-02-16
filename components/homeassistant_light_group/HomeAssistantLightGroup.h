#pragma once
#include <memory>
#include <vector>

#include "DisplayUpdateInterface.h"
#include "MenuTitle.h"
#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/homeassistant_light_group/light/HomeAssistantLight.h"
#include "esphome/core/component.h"

namespace esphome {
namespace homeassistant_light_group {

class HomeAssistantLightGroup : public api::CustomAPIDevice, public Component {
 public:
  std::vector<homeassistant_light::HomeAssistantLightState*> lights;
  std::vector<std::shared_ptr<MenuTitleBase>> lightTitleSwitches();
  bool selectLightDetailAtIndex(int index);
  void clearActiveLight() { _activeLight = NULL; }
  homeassistant_light::HomeAssistantLight* getActiveLight() {
    return static_cast<homeassistant_light::HomeAssistantLight*>(
        _activeLight->get_output());
  }
  bool lightDetailSelected = false;
  void register_light(homeassistant_light::HomeAssistantLightState* newLight);
  void set_display(DisplayUpdateInterface* newDisplay) { display = newDisplay; }
  void toggleLight(int index);

 private:
  DisplayUpdateInterface* display;
  homeassistant_light::HomeAssistantLightState* _activeLight = NULL;
};

}  // namespace homeassistant_light_group
}  // namespace esphome
