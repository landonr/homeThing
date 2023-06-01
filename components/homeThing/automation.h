#pragma once

#include "homeThingMenuScreen.h"
#include "esphome/core/automation.h"

namespace esphome {
namespace homething_menu_base {

class ServiceCalledTrigger : public Trigger<> {
 public:
  explicit ServiceCalledTrigger(MenuCommand* parent) {
    parent->add_on_command_callback([this, parent]() { trigger(); });
  }
};
}  // namespace homeassistant_service_group
}  // namespace esphome
