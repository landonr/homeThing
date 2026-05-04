#include "HomeAssistantSwitch.h"
#include <map>
#include "esphome/components/api/api_server.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_switch {

static const char* const TAG = "homeassistant.switch";

void HomeAssistantSwitch::setup() {
  ESP_LOGI(TAG, "'%s': Setup", get_name().c_str());
  if (this->attribute_) {
    subscribe_homeassistant_state(&HomeAssistantSwitch::state_changed,
                                  this->entity_id_, *this->attribute_);
  } else {
    subscribe_homeassistant_state(&HomeAssistantSwitch::state_changed,
                                  this->entity_id_);
  }
}

void HomeAssistantSwitch::publish_api_state(bool state) {
  next_api_publish_ = false;
  ignore_api_updates_with_seconds(2);
  ESP_LOGI(TAG, "publish_api_state: '%s': state changed to %d",
           this->get_name().c_str(), state);
  std::map<std::string, std::string> data = {{"entity_id", entity_id_}};
  if (state) {
    call_homeassistant_service("switch.turn_on", data);
  } else {
    call_homeassistant_service("switch.turn_off", data);
  }
}

void HomeAssistantSwitch::write_state(bool state) {
  // Acknowledge new state by publishing it
  ESP_LOGI(TAG, "write_state: '%s': state changed to %d",
           this->get_name().c_str(), state);
  publish_state(state);
  if (next_api_publish_) {
    publish_api_state(state);
  }
  this->state_callback_.call(state);
}

void HomeAssistantSwitch::state_changed(std::string state) {
  next_api_publish_ = false;
  if (can_update_from_api()) {
    auto val = parse_on_off(state.c_str());
    ESP_LOGI(TAG, "'%s': state changed to %d", this->get_name().c_str(), val);
    switch (val) {
      case PARSE_NONE:
      case PARSE_TOGGLE:
        ESP_LOGW(TAG, "Can't convert '%s' to binary state!", state.c_str());
        break;
      case PARSE_ON:
      case PARSE_OFF:
        bool new_state = val == PARSE_ON;
        if (this->attribute_.has_value()) {
          ESP_LOGD(TAG, "'%s::%s': Got attribute state %s",
                   this->entity_id_.c_str(), this->attribute_.value().c_str(),
                   ONOFF(new_state));
        } else {
          ESP_LOGD(TAG, "'%s': Got state %s", this->entity_id_.c_str(),
                   ONOFF(new_state));
        }
        this->publish_state(new_state);
        break;
    }
  }
  next_api_publish_ = true;
}

}  // namespace homeassistant_switch
}  // namespace esphome
