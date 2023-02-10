#include "HomeAssistantSwitch.h"

namespace esphome {
namespace home_assistant_switch {
HomeAssistantSwitch::HomeAssistantSwitch(std::string newFriendlyName,
                                         std::string newEntityId,
                                         DisplayUpdateInterface *newCallback)
    : display(newCallback) {
  set_entity_id(newEntityId);
  set_name(newFriendlyName);
  subscribe_homeassistant_state(&HomeAssistantSwitch::state_changed,
                                newEntityId.c_str());
  add_on_state_callback([this](bool state) {
    ESP_LOGI("Switch", "%s state changed to %d", this->get_name().c_str(),
             state);
    this->display->updateDisplay(false);
  });
}

void HomeAssistantSwitch::setup() {
  // This will be called by App.setup()
}

void HomeAssistantSwitch::write_state(bool state) {
  // Acknowledge new state by publishing it
  publish_state(state);
}

void HomeAssistantSwitch::toggleSwitch() {
  call_homeassistant_service("switch.toggle",
                             {{"entity_id", entity_id_.c_str()}});
}

std::string HomeAssistantSwitch::getEntityId() { return entity_id_; }

void HomeAssistantSwitch::state_changed(std::string newOnState) {
  ESP_LOGD("Switch", "%s state changed to %s", this->get_name().c_str(),
           newOnState);
  write_state(newOnState == "on");
}
}  // namespace home_assistant_switch
}  // namespace esphome
