#include "HomeAssistantSwitch.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_switch {
HomeAssistantSwitch::HomeAssistantSwitch() {}
// HomeAssistantSwitch::HomeAssistantSwitch(std::string newFriendlyName,
//                                          std::string newEntityId,
//                                          DisplayUpdateInterface *newCallback)
//     : display(newCallback) {
//   set_entity_id(newEntityId);
//   set_name(newFriendlyName);
//   // subscribe_homeassistant_state(&HomeAssistantSwitch::state_changed,
//   // newEntityId.c_str());
//   subscribeState();
//   add_on_state_callback([this](bool state) {
//     ESP_LOGI("HomeAssistantSwitch", "%s state changed to %d",
//              this->get_name().c_str(), state);
//     this->display->updateDisplay(false);
//   });
// }

void HomeAssistantSwitch::setup() {
  // This will be called by App.setup()
}

void HomeAssistantSwitch::write_state(bool state) {
  // Acknowledge new state by publishing it
  publish_state(state);
}

void HomeAssistantSwitch::toggleSwitch() {
  // call_homeassistant_service("switch.toggle",
  //                            {{"entity_id", entity_id_.c_str()}});
}

std::string HomeAssistantSwitch::getEntityId() { return entity_id_; }

void HomeAssistantSwitch::state_changed(std::string newOnState) {
  ESP_LOGD("HomeAssistantSwitch", "%s state changed to %s",
           this->get_name().c_str(), newOnState);
  write_state(newOnState == "on");
}

void HomeAssistantSwitch::subscribeState() {
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, this->attribute_, [this](const std::string &state) {
        auto val = parse_on_off(state.c_str());
        switch (val) {
          case PARSE_NONE:
          case PARSE_TOGGLE:
            ESP_LOGW("HomeAssistantSwitch",
                     "Can't convert '%s' to binary state!", state.c_str());
            break;
          case PARSE_ON:
          case PARSE_OFF:
            bool new_state = val == PARSE_ON;
            if (this->attribute_.has_value()) {
              ESP_LOGD("HomeAssistantSwitch",
                       "'%s::%s': Got attribute state %s",
                       this->entity_id_.c_str(),
                       this->attribute_.value().c_str(), ONOFF(new_state));
            } else {
              ESP_LOGD("HomeAssistantSwitch", "'%s': Got state %s",
                       this->entity_id_.c_str(), ONOFF(new_state));
            }
            this->publish_state(new_state);
            break;
        }
      });
}
}  // namespace homeassistant_switch
}  // namespace esphome
