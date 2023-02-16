#include "HomeAssistantSwitch.h"
#include <map>
#include "esphome/components/api/api_server.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_switch {

static const char* const TAG = "homeassistant.switch";

void HomeAssistantSwitch::setup() {
  ESP_LOGI(TAG, "'%s': Setup", get_name().c_str());
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, this->attribute_,
      std::bind(&HomeAssistantSwitch::state_changed, this,
                std::placeholders::_1));
}

void HomeAssistantSwitch::write_state(bool state) {
  // Acknowledge new state by publishing it
  publish_state(state);
}

void HomeAssistantSwitch::toggleSwitch() {
  std::map<std::string, std::string> data = {{"entity_id", entity_id_}};

  api::HomeassistantServiceResponse resp;
  resp.service = "switch.toggle";
  for (auto& it : data) {
    api::HomeassistantServiceMap kv;
    kv.key = it.first;
    kv.value = it.second;
    resp.data.push_back(kv);
  }
  // api::global_api_server->send_homeassistant_service_call(resp);
}

std::string HomeAssistantSwitch::get_entity_id() {
  return entity_id_;
}

void HomeAssistantSwitch::state_changed(std::string state) {
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

}  // namespace homeassistant_switch
}  // namespace esphome
