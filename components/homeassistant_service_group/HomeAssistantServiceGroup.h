#pragma once
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "DisplayUpdateInterface.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome {
namespace homeassistant_service_group {

class HomeAssistantServiceCommand {
 public:
  void on_command() { this->on_command_callbacks_.call(); }
  void add_on_command_callback(std::function<void()>&& callback) {
    this->on_command_callbacks_.add(std::move(callback));
  }
  template <typename V>
  std::string get_text() const {
    return const_cast<HomeAssistantServiceCommand*>(this)->text_.value(this);
  }
  template <typename V>
  void set_text(V val) {
    this->text_ = val;
  }

 private:
  CallbackManager<void()> on_command_callbacks_{};
  // std::vector<ServiceCalledTrigger*> on_command_callbacks_;
  TemplatableValue<std::string, const HomeAssistantServiceCommand*> text_;
};

class HomeAssistantServiceGroup : public Component {
 public:
  std::vector<HomeAssistantServiceCommand*> services;
  void register_service_call(HomeAssistantServiceCommand* new_service);

  bool select_service(int index);
  float get_setup_priority() const override { return setup_priority::LATE; }
};

}  // namespace homeassistant_service_group
}  // namespace esphome
