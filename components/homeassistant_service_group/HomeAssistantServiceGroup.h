#pragma once
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "DisplayUpdateInterface.h"
#include "MenuTitle.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome {
namespace homeassistant_service_group {

class HomeAssistantServiceCommand {
 public:
  void on_command() { this->on_command_callbacks_.call(); }
  void add_on_command_callback(std::function<void()>&& cb) {
    this->on_command_callbacks_.add(std::move(cb));
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
  TemplatableValue<std::string, const HomeAssistantServiceCommand*> text_;
};

class HomeAssistantServiceGroup : public Component {
 public:
  std::vector<HomeAssistantServiceCommand*> services;
  void register_service_call(HomeAssistantServiceCommand* new_service);

  std::vector<std::shared_ptr<MenuTitleBase>> sceneTitleStrings();
  bool select_service(int index);
};

}  // namespace homeassistant_service_group
}  // namespace esphome
