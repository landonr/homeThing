#include "HomeAssistantSwitch.h"
namespace home_assistant_switch {
HomeAssistantSwitch::HomeAssistantSwitch(std::string newFriendlyName, std::string newEntityId,
                                         DisplayUpdateInterface &newCallback)
    : display(newCallback) {
  set_entity_id(newEntityId);
  set_name(newFriendlyName);
  add_on_state_callback([this](bool state) {
    ESP_LOGI("Switch", "%s state changed to %d", this->get_name(), state);
    this->display.updateDisplay(false);
  });
}

void HomeAssistantSwitch::toggleSwitch() {
  call_homeassistant_service("switch.toggle", {{"entity_id", entity_id_.c_str()}});
}

std::string HomeAssistantSwitch::getEntityId() { return entity_id_; }
}  // namespace home_assistant_switch
