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

class HomeThingMenuBase : public Component {
 public:
  HomeThingMenuBase(
      HomeThingMenuDisplay* menu_display,
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          new_speaker_group,
      homeassistant_light_group::HomeAssistantLightGroup* new_light_group,
      homeassistant_service_group::HomeAssistantServiceGroup* new_service_group,
      homeassistant_sensor_group::HomeAssistantSensorGroup* new_sensor_group,
      homeassistant_switch_group::HomeAssistantSwitchGroup* new_switch_group)
      : menu_display_(menu_display),
        speaker_group_(new_speaker_group),
        light_group_(new_light_group),
        service_group_(new_service_group),
        sensor_group_(new_sensor_group),
        switch_group_(new_switch_group) {
    animation_ = new HomeThingMenuAnimation();
    menu_display_->set_animation(animation_);

    display_update_tick_ = new sensor::Sensor();
    auto filter = new sensor::DebounceFilter(17);
    display_update_tick_->add_filter(filter);
    this->display_update_tick_->add_on_state_callback(
        [this](float state) { this->displayUpdateDebounced(); });
    // menu_display_ = new HomeThingMenuDisplay(animation_);
  }
  void setup();
  void drawMenu();
  void topMenu();
  bool selectMenu();
  bool selectMenuHold();
  bool selectRootMenu();
  std::shared_ptr<MenuTitleBase> menuTitleForType(MenuStates stringType);
  std::vector<std::shared_ptr<MenuTitleBase>> activeMenu();
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

  // move to media players
  void selectMediaPlayers();
  // create service for this with input select options
  void goToScreenFromString(std::string screenName);
  void displayUpdateDebounced();
  void add_on_redraw_callback(std::function<void()>&& cb) {
    this->on_redraw_callbacks_.add(std::move(cb));
  }

 private:
  MenuStates activeMenuState = bootMenu;
  switch_::Switch* backlight_{nullptr};
  switch_::Switch* sleep_toggle_{nullptr};
  HomeThingMenuDisplay* menu_display_{nullptr};
  std::vector<std::shared_ptr<MenuTitleBase>> menuTypesToTitles(
      std::vector<MenuStates> menu);
  HomeThingMenuAnimation* animation_{nullptr};
  std::shared_ptr<MenuTitleBase> activeMenuTitle =
      std::make_shared<MenuTitleBase>("", "", NoMenuTitleRightIcon);
  homeassistant_service_group::HomeAssistantServiceGroup* service_group_;
  homeassistant_media_player::HomeAssistantMediaPlayerGroup* speaker_group_;
  homeassistant_light_group::HomeAssistantLightGroup* light_group_;
  homeassistant_switch_group::HomeAssistantSwitchGroup* switch_group_;
  homeassistant_sensor_group::HomeAssistantSensorGroup* sensor_group_;
  void update() { this->on_redraw_callbacks_.call(); }
  void debounceUpdateDisplay();

  sensor::Sensor* display_update_tick_;
  int rotary_ = 0;
  int menuIndex = 0;
  int rotaryPosition = 0;
  int activeMenuTitleCount = 0;
  CallbackManager<void()> on_redraw_callbacks_{};
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
