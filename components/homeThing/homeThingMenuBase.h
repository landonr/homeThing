#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/homeThing/homeThingMenuAnimation.h"
#include "esphome/components/homeThing/homeThingMenuBoot.h"
#include "esphome/components/homeThing/homeThingMenuControls.h"
#include "esphome/components/homeThing/homeThingMenuDisplay.h"
#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeThing/homeThingMenuScreen.h"
#include "esphome/components/homeThing/homeThingMenuSettings.h"

#ifdef USE_LIGHT
#include "esphome/components/homeThing/homeThingMenuTitleLight.h"
#else
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#endif

#include "esphome/components/homeThingDisplayState/homeThingDisplayState.h"

#ifdef USE_HOMETHING_APP
#include "esphome/components/homeThingApp/homeThingApp.h"
#endif

#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome {
namespace homething_menu_base {

class HomeThingMenuBase : public PollingComponent {
 public:
  HomeThingMenuBase(HomeThingMenuSettings* menu_settings,
                    HomeThingMenuDisplay* menu_display)
      : menu_settings_(menu_settings), menu_display_(menu_display) {
    menu_display_->set_active_menu_screen(&active_menu_screen);
  }
  void setup();

#ifdef USE_BINARY_SENSOR
  void set_charging(binary_sensor::BinarySensor* charging) {
    charging_ = charging;
  }
#endif
  void set_battery_percent(sensor::Sensor* battery_percent) {
    battery_percent_ = battery_percent;
  }

#ifdef USE_SWITCH
  void set_sleep_switch(switch_::Switch* sleep_switch) {
    sleep_switch_ = sleep_switch;
  }
#endif

#ifdef USE_LIGHT
  void set_backlight(light::LightState* backlight) { backlight_ = backlight; }
#endif

#ifdef USE_HOMETHING_APP
  void register_app(homething_menu_app::HomeThingApp* newApp) {
    menu_apps_.push_back(newApp);
  }
#endif

  void register_screen(HomeThingMenuScreen* new_screen) {
    new_screen->set_index(menu_screens_.size());
    menu_screens_.push_back(new_screen);
  }

  void register_home_screen(HomeThingMenuScreen* new_screen) {
    home_screen_ = new_screen;
  }

  void draw_menu_screen();
  void topMenu();
  bool selectMenu();
  bool selectDetailMenu();
  bool selectRootMenu();
  MenuTitleBase* menuTitleForType(MenuStates stringType, int index);
  void lockDevice();
  void idleTick();
  bool buttonPressWakeUpDisplay();
  void idleMenu();
  bool selectLightEntity(
      const std::tuple<MenuItemType, EntityBase*>* menu_item);
  bool upMenu();
  void rotaryScrollClockwise();
  void rotaryScrollCounterClockwise();
  void buttonPressUp();
  void buttonPressDown();
  void buttonPressLeft();
  void buttonPressRight();
  void buttonPressSelect();
  void buttonPressOption();
  bool buttonPressUnlock();

  // create service for this with input select options
  void goToScreenFromString(std::string screenName);
  void displayUpdateDebounced();
  void add_on_redraw_callback(std::function<void()>&& cb) {
    this->on_redraw_callbacks_.add(std::move(cb));
  }

 private:
  bool skipBootPressed();
  bool button_press_and_continue() {
    if (buttonPressWakeUpDisplay()) {
      return false;
    }
    if (menuTree.front() != bootMenu) {
      if (device_locked_) {
        ESP_LOGI(TAG, "button_press_and_continue: locked");
        idleTime = 0;
        update_display();
        return false;
      } else {
        ESP_LOGD(TAG, "button_press_and_continue: reset animation %d",
                 menuTree.front());
        animation_->resetAnimation();
        return true;
      }
    } else {
      return false;
    }
    return true;
  }
  float get_battery_percent() {
    if (battery_percent_ != nullptr && battery_percent_->has_state()) {
      return battery_percent_->state;
    } else {
      return 0;
    }
  }
  bool get_charging() {
#ifdef USE_BINARY_SENSOR
    if (charging_ != nullptr && charging_->has_state()) {
      return charging_->state;
    } else {
      return charging_ == nullptr;
    }
#else
    return false;
#endif
  }
  void fade_out_display();
  void sleep_display();
  bool display_can_fade_out();
  bool display_can_sleep();

  int idleTime = -2;
  std::vector<MenuStates> menuTree = {bootMenu};
#ifdef USE_LIGHT
  light::LightState* backlight_{nullptr};
#endif

#ifdef USE_SWITCH
  switch_::Switch* sleep_switch_{nullptr};
#endif
  HomeThingMenuSettings* menu_settings_{nullptr};
  HomeThingMenuDisplay* menu_display_{nullptr};
  HomeThingMenuAnimation* animation_ = new HomeThingMenuAnimation();
  std::vector<HomeThingMenuScreen*> menu_screens_;
  HomeThingMenuScreen* home_screen_{nullptr};
  HomeThingMenuScreen* active_menu_screen{nullptr};

#ifdef USE_HOMETHING_APP
  std::vector<homething_menu_app::HomeThingApp*> menu_apps_;
  homething_menu_app::HomeThingApp* active_app_{nullptr};
#endif

  void update_display() { this->on_redraw_callbacks_.call(); }
  void debounceUpdateDisplay();
  void update();
  void activeMenu(std::vector<MenuTitleBase*>*);
  void reset_menu() {
    menuIndex = 0;
    if (active_menu_screen)
      active_menu_screen->set_selected_entity(nullptr);
    active_menu_screen = nullptr;
    reload_menu_items_ = true;
    editing_menu_item = false;
    if (menuTree.front() != bootMenu) {
      menuTree.assign(1, rootMenu);
      ESP_LOGD(TAG, "reset_menu: reset animation %d", menuTree.front());
      animation_->resetAnimation();
    }
#ifdef USE_HOMETHING_APP
    if (active_app_) {
      active_app_->reset_menu();
    }
    active_app_ = nullptr;
#endif
  }
  void turn_on_backlight();

  sensor::Sensor* display_update_tick_;
  int rotary_ = 0;
  int menuIndex = 0;
  int rotaryPosition = 0;
  std::vector<MenuTitleBase*> menu_titles;
  CallbackManager<void()> on_redraw_callbacks_{};
  const char* const TAG = "homething.menu.base";
  bool menu_drawing_ = false;
  bool reload_menu_items_ = false;
  sensor::Sensor* battery_percent_{nullptr};
#ifdef USE_BINARY_SENSOR
  binary_sensor::BinarySensor* charging_{nullptr};
#endif
  bool device_locked_ = false;
  int unlock_presses_ = 0;
  void finish_boot();
  bool editing_menu_item = false;
};  // namespace homething_menu_base

class HomeThingDisplayMenuOnRedrawTrigger
    : public Trigger<const HomeThingMenuBase*> {
 public:
  explicit HomeThingDisplayMenuOnRedrawTrigger(HomeThingMenuBase* parent) {
    parent->add_on_redraw_callback([this, parent]() { this->trigger(parent); });
  }
};
}  // namespace homething_menu_base
}  // namespace esphome
