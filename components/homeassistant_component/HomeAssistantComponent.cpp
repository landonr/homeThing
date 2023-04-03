#include "HomeAssistantComponent.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_component {

static const char* const TAG = "homeassistant.component";

bool HomeAssistantComponent::can_update_from_api() {
  return micros() > ignore_update_until_;
}

void HomeAssistantComponent::ignore_api_updates_with_seconds(int seconds) {
  uint32_t delayTime = seconds * 1000000;
  ignore_update_until_ = micros() + delayTime;
}
}  // namespace homeassistant_component
}  // namespace esphome