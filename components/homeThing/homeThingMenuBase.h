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
#include "esphome/components/homeThing/homeThingMenuDisplayState.h"
#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeThing/homeThingMenuScreen.h"
#include "esphome/components/homeThing/homeThingMenuSettings.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"

#ifdef USE_MEDIA_PLAYER_GROUP
#include "esphome/components/homeThing/homeThingMenuNowPlaying.h"
#include "esphome/components/homeThing/homeThingMenuNowPlayingOptionMenu.h"
#include "esphome/components/homeThing/homeThingOptionMenu.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"
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

  void set_charging(binary_sensor::BinarySensor* charging) {
    charging_ = charging;
  }
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

  void register_screen(HomeThingMenuScreen* new_screen) {
    new_screen->set_index(menu_screens_.size());
    menu_screens_.push_back(new_screen);
  }

  void register_home_screen(HomeThingMenuScreen* new_screen) {
    home_sceen_ = new_screen;
  }

#ifdef USE_MEDIA_PLAYER_GROUP
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
  get_media_player_group() {
    return media_player_group_;
  }
  void set_media_player_group(
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          media_player_group) {
    media_player_group_ = media_player_group;
  }
#endif

  void draw_menu_screen();
  void topMenu();
  bool selectMenu();
  bool selectMenuHold();
  bool selectRootMenu();
  MenuTitleBase* menuTitleForType(MenuStates stringType, int index);
  void lockDevice();
  void idleTick();
  bool buttonPressWakeUpDisplay();
  void idleMenu(bool force);

  // controls
#ifdef USE_MEDIA_PLAYER_GROUP
  bool select_media_player_feature(
      homeassistant_media_player::MediaPlayerFeatureCommand* command);
  bool button_press_now_playing_option_continue(
      CircleOptionMenuPosition position);
#endif
  bool selectLightEntity(
      const std::tuple<MenuItemType, EntityBase*>* menu_item);
  bool upMenu();
  void rotaryScrollClockwise(int rotary);
  void rotaryScrollCounterClockwise(int rotary);
  void buttonPressUp();
  void buttonPressDown();
  void buttonPressLeft();
  void buttonPressRight();
  void buttonPressSelect();
  void buttonPressSelectHold();
  bool buttonPressUnlock();
  void buttonReleaseScreenLeft();
  void buttonPressScreenLeft();
  void buttonPressScreenRight();

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
    if (charging_ != nullptr && charging_->has_state()) {
      return charging_->state;
    } else {
      return charging_ == nullptr;
    }
  }
  void fade_out_display();
  void sleep_display();
  bool display_can_fade_out();
  bool display_can_sleep();

  int idleTime = -2;
  int static_menu_titles = 0;
  std::vector<MenuStates> menuTree = {bootMenu};
#ifdef USE_LIGHT
  light::LightState* backlight_{nullptr};
#endif

#ifdef USE_SWITCH
  switch_::Switch* sleep_switch_{nullptr};
#endif
  HomeThingMenuSettings* menu_settings_{nullptr};
  HomeThingMenuDisplay* menu_display_{nullptr};
  void menuTypesToTitles(std::vector<MenuStates> menu,
                         std::vector<MenuTitleBase*>* menu_titles);
  HomeThingMenuAnimation* animation_ = new HomeThingMenuAnimation();
  std::vector<HomeThingMenuScreen*> menu_screens_;
  HomeThingMenuScreen* home_sceen_{nullptr};
  HomeThingMenuScreen* active_menu_screen{nullptr};

#ifdef USE_MEDIA_PLAYER_GROUP
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
      media_player_group_{nullptr};
  void selectNowPlayingMenu();
  HomeThingMenuNowPlayingOptionMenu* circle_menu_ =
      new HomeThingMenuNowPlayingOptionMenu();
#endif

  void update_display() { this->on_redraw_callbacks_.call(); }
  void debounceUpdateDisplay();
  void update();
  void activeMenu(std::vector<MenuTitleBase*>*);
  std::vector<MenuStates> rootMenuTitles();
  void reset_menu() {
    menuIndex = 0;
    active_menu_screen = nullptr;
    reload_menu_items_ = true;
    editing_menu_item = false;
#ifdef USE_MEDIA_PLAYER_GROUP
    circle_menu_->clear_active_menu();
#endif
    if (menuTree.front() != bootMenu) {
      menuTree.assign(1, rootMenu);
      ESP_LOGD(TAG, "reset_menu: reset animation %d", menuTree.front());
      animation_->resetAnimation();
    }
#ifdef USE_MEDIA_PLAYER_GROUP
    if (media_player_group_) {
      media_player_group_->newSpeakerGroupParent = NULL;
      media_player_group_->set_active_player_source_index(-1);
    }
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
  binary_sensor::BinarySensor* charging_{nullptr};
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
