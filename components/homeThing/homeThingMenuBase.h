#pragma once

#include <memory>
#include <string>
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
#include "esphome/core/component.h"

namespace esphome {
namespace homething_menu_base {

class HomeThingMenuBase : public Component {
 public:
  HomeThingMenuBase(
      HomeThingMenuDisplay* menu_display,
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          new_speaker_group,
      homeassistant_light_group::HomeAssistantLightGroup* new_light_group)
      : menu_display_(menu_display),
        speakerGroup(new_speaker_group),
        lightGroup(new_light_group) {
    animation_ = new HomeThingMenuAnimation();
    menu_display_->set_animation(animation_);
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
  void rotaryScrollClockwise();
  void rotaryScrollCounterClockwise();
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
  homeassistant_service_group::HomeAssistantServiceGroup* serviceGroup;
  homeassistant_media_player::HomeAssistantMediaPlayerGroup* speakerGroup;
  homeassistant_light_group::HomeAssistantLightGroup* lightGroup;
  homeassistant_switch_group::HomeAssistantSwitchGroup* switchGroup;
  homeassistant_sensor_group::HomeAssistantSensorGroup* sensorGroup;

  void displayUpdateDebounced();
  void debounceUpdateDisplay();

  bool draw_now_playing_menu = true;
  int display_update_tick = 0;
  int rotary = 0;
};
}  // namespace homething_menu_base
}  // namespace esphome
