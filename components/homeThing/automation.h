#pragma once

#include "esphome/core/automation.h"
#include "homeThingMenuBase.h"
#include "homeThingMenuScreen.h"

namespace esphome {
namespace homething_menu_base {

template <typename... Ts>
class UpAction : public Action<Ts...> {
 public:
  explicit UpAction(HomeThingMenuBase* menu) : menu_(menu) {}

  void play(Ts... x) override { this->menu_->buttonPressUp(); }

 protected:
  HomeThingMenuBase* menu_;
};

template <typename... Ts>
class DownAction : public Action<Ts...> {
 public:
  explicit DownAction(HomeThingMenuBase* menu) : menu_(menu) {}

  void play(Ts... x) override { this->menu_->buttonPressDown(); }

 protected:
  HomeThingMenuBase* menu_;
};

template <typename... Ts>
class LeftAction : public Action<Ts...> {
 public:
  explicit LeftAction(HomeThingMenuBase* menu) : menu_(menu) {}

  void play(Ts... x) override { this->menu_->buttonPressLeft(); }

 protected:
  HomeThingMenuBase* menu_;
};

template <typename... Ts>
class RightAction : public Action<Ts...> {
 public:
  explicit RightAction(HomeThingMenuBase* menu) : menu_(menu) {}

  void play(Ts... x) override { this->menu_->buttonPressRight(); }

 protected:
  HomeThingMenuBase* menu_;
};

template <typename... Ts>
class SelectAction : public Action<Ts...> {
 public:
  explicit SelectAction(HomeThingMenuBase* menu) : menu_(menu) {}

  void play(Ts... x) override { this->menu_->buttonPressSelect(); }

 protected:
  HomeThingMenuBase* menu_;
};

template <typename... Ts>
class ScrollClockwiseAction : public Action<Ts...> {
 public:
  explicit ScrollClockwiseAction(HomeThingMenuBase* menu) : menu_(menu) {}

  void play(Ts... x) override { this->menu_->rotaryScrollClockwise(); }

 protected:
  HomeThingMenuBase* menu_;
};

template <typename... Ts>
class ScrollCounterClockwiseAction : public Action<Ts...> {
 public:
  explicit ScrollCounterClockwiseAction(HomeThingMenuBase* menu)
      : menu_(menu) {}

  void play(Ts... x) override { this->menu_->rotaryScrollCounterClockwise(); }

 protected:
  HomeThingMenuBase* menu_;
};

template <typename... Ts>
class BackAction : public Action<Ts...> {
 public:
  explicit BackAction(HomeThingMenuBase* menu) : menu_(menu) {}

  void play(Ts... x) override { this->menu_->upMenu(); }

 protected:
  HomeThingMenuBase* menu_;
};

template <typename... Ts>
class OptionAction : public Action<Ts...> {
 public:
  explicit OptionAction(HomeThingMenuBase* menu) : menu_(menu) {}

  void play(Ts... x) override { this->menu_->buttonPressOption(); }

 protected:
  HomeThingMenuBase* menu_;
};

template <typename... Ts>
class HomeAction : public Action<Ts...> {
 public:
  explicit HomeAction(HomeThingMenuBase* menu) : menu_(menu) {}

  void play(Ts... x) override { this->menu_->topMenu(); }

 protected:
  HomeThingMenuBase* menu_;
};

class ServiceCalledTrigger : public Trigger<> {
 public:
  explicit ServiceCalledTrigger(MenuCommand* parent) {
    parent->add_on_command_callback([this, parent]() { trigger(); });
  }
};
}  // namespace homething_menu_base
}  // namespace esphome
