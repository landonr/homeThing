#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/homeThing/homeThingMenuAnimation.h"
#include "esphome/components/homeThing/homeThingMenuBoot.h"
#include "esphome/components/homeThing/homeThingMenuDisplay.h"
#include "esphome/components/homeThing/homeThingMenuDisplayState.h"
#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeThing/homeThingMenuNowPlaying.h"
#include "esphome/components/homeThing/homeThingMenuNowPlayingOptionMenu.h"
#include "esphome/components/homeThing/homeThingMenuSettings.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/components/homeThing/homeThingOptionMenu.h"
#include "esphome/components/homeassistant_light_group/HomeAssistantLightGroup.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"
#include "esphome/components/homeassistant_sensor_group/HomeAssistantSensorGroup.h"
#include "esphome/components/homeassistant_service_group/HomeAssistantServiceGroup.h"
#include "esphome/components/homeassistant_switch_group/HomeAssistantSwitchGroup.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome {
namespace homething_menu_base {

class HomeThingMenuBase : public PollingComponent {
 public:
  HomeThingMenuBase(HomeThingMenuSettings* menu_settings,
                    HomeThingMenuDisplay* menu_display)
      : menu_settings_(menu_settings), menu_display_(menu_display) {}
  void setup();

  void set_charging(binary_sensor::BinarySensor* charging) {
    charging_ = charging;
  }
  void set_battery_percent(sensor::Sensor* battery_percent) {
    battery_percent_ = battery_percent;
  }
  void set_sleep_switch(switch_::Switch* sleep_switch) {
    sleep_switch_ = sleep_switch;
  }
  void set_backlight(light::LightState* backlight) { backlight_ = backlight; }
  homeassistant_service_group::HomeAssistantServiceGroup* get_service_group() {
    return service_group_;
  }

  void set_service_group(
      homeassistant_service_group::HomeAssistantServiceGroup* service_group) {
    service_group_ = service_group;
  }
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
  get_media_player_group() {
    return media_player_group_;
  }
  void set_media_player_group(
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          media_player_group) {
    media_player_group_ = media_player_group;
  }
  homeassistant_light_group::HomeAssistantLightGroup* get_light_group() {
    return light_group_;
  }
  void set_light_group(
      homeassistant_light_group::HomeAssistantLightGroup* light_group) {
    light_group_ = light_group;
  }
  homeassistant_switch_group::HomeAssistantSwitchGroup* get_switch_group() {
    return switch_group_;
  }
  void set_switch_group(
      homeassistant_switch_group::HomeAssistantSwitchGroup* switch_group) {
    switch_group_ = switch_group;
  }
  homeassistant_sensor_group::HomeAssistantSensorGroup* get_sensor_group() {
    return sensor_group_;
  }
  void set_sensor_group(
      homeassistant_sensor_group::HomeAssistantSensorGroup* sensor_group) {
    sensor_group_ = sensor_group;
  }

  void draw_menu_screen();
  void topMenu();
  bool selectMenu();
  bool selectMenuHold();
  bool selectRootMenu();
  std::shared_ptr<MenuTitleBase> menuTitleForType(MenuStates stringType);
  void lockDevice();
  void idleTick();
  bool buttonPressWakeUpDisplay();
  void idleMenu(bool force);

  // controls
  bool select_media_player_feature(
      homeassistant_media_player::MediaPlayerSupportedFeature* feature);
  bool button_press_now_playing_option_continue(
      CircleOptionMenuPosition position);
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
  bool button_press_and_continue() {
    if (buttonPressWakeUpDisplay()) {
      return false;
    }
    if (activeMenuState != bootMenu) {
      if (device_locked_) {
        ESP_LOGI(TAG, "button_press_and_continue: locked");
        return idleTime >= 10;
      } else {
        ESP_LOGD(TAG, "button_press_and_continue: reset animation %d",
                 activeMenuState);
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
  bool display_can_sleep();

  int idleTime = -2;
  MenuStates activeMenuState = bootMenu;
  light::LightState* backlight_{nullptr};
  switch_::Switch* sleep_switch_{nullptr};
  HomeThingMenuDisplay* menu_display_{nullptr};
  HomeThingMenuSettings* menu_settings_{nullptr};
  HomeThingMenuNowPlayingOptionMenu* circle_menu_ =
      new HomeThingMenuNowPlayingOptionMenu();
  std::vector<std::shared_ptr<MenuTitleBase>> menuTypesToTitles(
      std::vector<MenuStates> menu);
  HomeThingMenuAnimation* animation_ = new HomeThingMenuAnimation();
  homeassistant_service_group::HomeAssistantServiceGroup* service_group_{
      nullptr};
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
      media_player_group_{nullptr};
  homeassistant_light_group::HomeAssistantLightGroup* light_group_{nullptr};
  homeassistant_switch_group::HomeAssistantSwitchGroup* switch_group_{nullptr};
  homeassistant_sensor_group::HomeAssistantSensorGroup* sensor_group_{nullptr};
  void update_display() { this->on_redraw_callbacks_.call(); }
  void debounceUpdateDisplay();
  void update();
  std::vector<std::shared_ptr<MenuTitleBase>> activeMenu();
  void selectNowPlayingMenu();
  std::vector<MenuStates> rootMenuTitles();
  void reset_menu() {
    menuIndex = 0;
    reload_menu_items_ = true;
    circle_menu_->clear_active_menu();
    if (activeMenuState != bootMenu) {
      ESP_LOGD(TAG, "reset_menu: reset animation %d", activeMenuState);
      animation_->resetAnimation();
    }
    if (media_player_group_) {
      media_player_group_->newSpeakerGroupParent = NULL;
      media_player_group_->set_active_player_source_index(-1);
    }
  }
  void turn_on_backlight();

  sensor::Sensor* display_update_tick_;
  int rotary_ = 0;
  int menuIndex = 0;
  int rotaryPosition = 0;
  std::vector<std::shared_ptr<MenuTitleBase>> menu_titles;
  CallbackManager<void()> on_redraw_callbacks_{};
  const char* const TAG = "homething.menu.base";
  bool menu_drawing_ = false;
  bool reload_menu_items_ = false;
  sensor::Sensor* battery_percent_{nullptr};
  binary_sensor::BinarySensor* charging_{nullptr};
  bool device_locked_ = false;
  int unlock_presses_ = 0;
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
