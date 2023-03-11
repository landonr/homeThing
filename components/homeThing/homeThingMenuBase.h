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
#include "esphome/components/homeThing/homeThingMenuGlobals.h"
#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeThing/homeThingMenuNowPlaying.h"
#include "esphome/components/homeThing/homeThingMenuSettings.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"
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
  HomeThingMenuBase(
      HomeThingMenuSettings* menu_settings, HomeThingMenuDisplay* menu_display,
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          new_speaker_group,
      homeassistant_light_group::HomeAssistantLightGroup* new_light_group,
      homeassistant_service_group::HomeAssistantServiceGroup* new_service_group,
      homeassistant_sensor_group::HomeAssistantSensorGroup* new_sensor_group,
      homeassistant_switch_group::HomeAssistantSwitchGroup* new_switch_group)
      : menu_settings_(menu_settings),
        menu_display_(menu_display),
        speaker_group_(new_speaker_group),
        light_group_(new_light_group),
        service_group_(new_service_group),
        sensor_group_(new_sensor_group),
        switch_group_(new_switch_group) {}
  void setup();

  bool get_charging() { return charging_->has_state() && charging_->state; }
  void set_charging(binary_sensor::BinarySensor* charging) {
    charging_ = charging;
  }

  float get_battery_percent() {
    return battery_percent_->has_state() && battery_percent_->state;
  }
  void set_battery_percent(sensor::Sensor* battery_percent) {
    battery_percent_ = battery_percent;
  }
  void set_sleep_switch(switch_::Switch* sleep_switch) {
    sleep_switch_ = sleep_switch;
  }
  void set_backlight(switch_::Switch* backlight) { backlight_ = backlight; }

  void draw_menu_screen();
  void topMenu();
  bool selectMenu();
  bool selectMenuHold();
  bool selectRootMenu();
  std::shared_ptr<MenuTitleBase> menuTitleForType(MenuStates stringType);
  void idleTick();
  bool buttonPressWakeUpDisplay();
  void idleMenu(bool force);

  // controls
  void rotaryScrollClockwise(int rotary);
  void rotaryScrollCounterClockwise(int rotary);
  void buttonPressUp();
  void buttonPressDown();
  void buttonPressLeft();
  void buttonPressRight();
  void buttonPressSelect();
  void buttonPressSelectHold();
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
  int idleTime = -2;
  MenuStates activeMenuState = bootMenu;
  switch_::Switch* backlight_{nullptr};
  switch_::Switch* sleep_switch_{nullptr};
  HomeThingMenuDisplay* menu_display_{nullptr};
  HomeThingMenuSettings* menu_settings_{nullptr};
  std::vector<std::shared_ptr<MenuTitleBase>> menuTypesToTitles(
      std::vector<MenuStates> menu);
  HomeThingMenuAnimation* animation_ = new HomeThingMenuAnimation();
  homeassistant_service_group::HomeAssistantServiceGroup* service_group_;
  homeassistant_media_player::HomeAssistantMediaPlayerGroup* speaker_group_;
  homeassistant_light_group::HomeAssistantLightGroup* light_group_;
  homeassistant_switch_group::HomeAssistantSwitchGroup* switch_group_;
  homeassistant_sensor_group::HomeAssistantSensorGroup* sensor_group_;
  void update_display() { this->on_redraw_callbacks_.call(); }
  void debounceUpdateDisplay();
  void update();
  std::vector<std::shared_ptr<MenuTitleBase>> activeMenu();
  void selectNowPlayingMenu();

  sensor::Sensor* display_update_tick_;
  int rotary_ = 0;
  int menuIndex = 0;
  int rotaryPosition = 0;
  std::vector<std::shared_ptr<MenuTitleBase>> menu_titles;
  CallbackManager<void()> on_redraw_callbacks_{};
  const char* const TAG = "homething.menu.base";
  bool menu_drawing_ = false;
  option_menuType option_menu_ = noOptionMenu;
  sensor::Sensor* battery_percent_{nullptr};
  binary_sensor::BinarySensor* charging_{nullptr};
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
