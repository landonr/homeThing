#pragma once

#include "HomeAssistantServiceGroup.h"
#include "esphome/core/automation.h"

namespace esphome {
namespace homeassistant_service_group {

class ServiceCalledTrigger : public Trigger<> {
 public:
  explicit ServiceCalledTrigger(HomeAssistantServiceCommand* parent) {
    parent->add_on_command_callback([this, parent]() { trigger(); });
  }
};
}  // namespace homeassistant_service_group
}  // namespace esphome
