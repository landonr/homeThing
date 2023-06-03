#pragma once

#include "esphome/core/automation.h"
#include "homeThingMenuScreen.h"

namespace esphome {
namespace homething_menu_base {

class ServiceCalledTrigger : public Trigger<> {
 public:
  explicit ServiceCalledTrigger(MenuCommand* parent) {
    parent->add_on_command_callback([this, parent]() { trigger(); });
  }
};
}  // namespace homething_menu_base
}  // namespace esphome
