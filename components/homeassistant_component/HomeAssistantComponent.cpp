#include "HomeAssistantComponent.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_component {

static const char* const TAG = "homeassistant.component";

bool HomeAssistantComponent::can_update_from_api() {
  if (ignore_update_until_ == 0) {
    ESP_LOGI(TAG, "can_update_from_api: true %@ %d", get_entity_id().c_str(),
             ignore_update_until_);
    return true;
  } else {
    ESP_LOGI(TAG, "can_update_from_api: %@ %d", get_entity_id().c_str(),
             micros() > ignore_update_until_);
    return micros() > ignore_update_until_;
  }
}

void HomeAssistantComponent::ignore_api_updates_with_seconds(int seconds) {
  ESP_LOGI(TAG, "ignore_api_updates_with_seconds: %@ %d",
           get_entity_id().c_str(), seconds);
  uint32_t delayTime = seconds * 1000000;
  ignore_update_until_ = micros() + delayTime;
}
}  // namespace homeassistant_component
}  // namespace esphome