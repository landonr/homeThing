#pragma once

#include "esphome/core/automation.h"
#include "homeThingMenuScreen.h"
#include "homeThingMenuBase.h"

namespace esphome {
namespace homething_menu_base {

template<typename... Ts> class UpAction : public Action<Ts...> {
 public:
  explicit UpAction(HomeThingMenuBase *menu) : menu_(menu) {}

  void play(Ts... x) override { this->menu_->buttonPressUp(); }

 protected:
  HomeThingMenuBase *menu_;
};

template<typename... Ts> class DownAction : public Action<Ts...> {
 public:
  explicit DownAction(HomeThingMenuBase *menu) : menu_(menu) {}

  void play(Ts... x) override { this->menu_->buttonPressDown(); }

 protected:
  HomeThingMenuBase *menu_;
};

template<typename... Ts> class LeftAction : public Action<Ts...> {
 public:
  explicit LeftAction(HomeThingMenuBase *menu) : menu_(menu) {}

  void play(Ts... x) override { this->menu_->buttonPressLeft(); }

 protected:
  HomeThingMenuBase *menu_;
};

template<typename... Ts> class RightAction : public Action<Ts...> {
 public:
  explicit RightAction(HomeThingMenuBase *menu) : menu_(menu) {}

  void play(Ts... x) override { this->menu_->buttonPressRight(); }

 protected:
  HomeThingMenuBase *menu_;
};

template<typename... Ts> class SelectAction : public Action<Ts...> {
 public:
  explicit SelectAction(HomeThingMenuBase *menu) : menu_(menu) {}

  void play(Ts... x) override { this->menu_->buttonPressSelect(); }

 protected:
  HomeThingMenuBase *menu_;
};

class ServiceCalledTrigger : public Trigger<> {
 public:
  explicit ServiceCalledTrigger(MenuCommand* parent) {
    parent->add_on_command_callback([this, parent]() { trigger(); });
  }
};
}  // namespace homething_menu_base
}  // namespace esphome
