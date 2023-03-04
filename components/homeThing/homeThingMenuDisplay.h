#pragma once

#include <memory>
#include <string>
#include <vector>
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/homeThing/homeThingMenuAnimation.h"
#include "esphome/components/homeThing/homeThingMenuBoot.h"
#include "esphome/components/homeThing/homeThingMenuDisplayState.h"
#include "esphome/components/homeThing/homeThingMenuGlobals.h"
#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeThing/homeThingMenuNowPlaying.h"
#include "esphome/components/homeThing/homeThingMenuRefactor.h"
#include "esphome/components/homeThing/homeThingMenuTextHelpers.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/components/homeassistant_light_group/HomeAssistantLightGroup.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"
#include "esphome/components/homeassistant_sensor_group/HomeAssistantSensorGroup.h"
#include "esphome/components/homeassistant_service_group/HomeAssistantServiceGroup.h"
#include "esphome/components/homeassistant_switch_group/HomeAssistantSwitchGroup.h"
#include "esphome/core/component.h"

namespace esphome {
namespace homething_menu_base {

class DisplayUpdateInterface {
 public:
  virtual void updateDisplay(bool force) = 0;
};

class DisplayUpdateImpl : public DisplayUpdateInterface {
 public:
  virtual void updateDisplay(bool force) {
    // if (menuDrawing && backlight_->state) {
    //   ESP_LOGW("WARNING", "menu already drawing");
    //   return;
    // }
    // if (force) {
    //   menuDrawing = true;
    //   display_buffer_->update();
    //   if (!backlight_->state) {
    //     backlight_->turn_on();
    //   }
    //   return;
    // }

    // if (backlight_->state) {
    //   switch (activeMenuState) {
    //     case sourcesMenu:
    //       return;
    //     default:
    //       menuDrawing = true;
    //       display_buffer_->update();
    //       break;
    //   }
    // }
  }
};

class HomeThingMenuDisplay {
 public:
  HomeThingMenuDisplay(
      display::DisplayBuffer* display_buffer,
      HomeThingMenuDisplayState* display_state,
      HomeThingMenuTextHelpers* text_helpers, HomeThingMenuRefactor* refactor,
      HomeThingMenuNowPlaying* now_playing, HomeThingMenuHeader* header,
      HomeThingMenuBoot* boot,
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          new_speaker_group,
      homeassistant_light_group::HomeAssistantLightGroup* new_light_group,
      homeassistant_service_group::HomeAssistantServiceGroup* new_service_group,
      homeassistant_sensor_group::HomeAssistantSensorGroup* new_sensor_group,
      homeassistant_switch_group::HomeAssistantSwitchGroup* new_switch_group)
      : display_buffer_(display_buffer),
        display_state_(display_state),
        text_helpers_(text_helpers),
        refactor_(refactor),
        now_playing_(now_playing),
        header_(header),
        boot_(boot),
        speaker_group_(new_speaker_group),
        light_group_(new_light_group),
        service_group_(new_service_group),
        sensor_group_(new_sensor_group),
        switch_group_(new_switch_group) {}
  void setup();
  void drawMenu(MenuStates* activeMenuState,
                std::vector<std::shared_ptr<MenuTitleBase>> active_menu,
                const int menuIndex);
  void drawTitle(int menuState, int i, std::string title, int yPos,
                 bool buttonSpace);
  void drawScrollBar(int menuTitlesCount, int headerHeight, int menuIndex);
  void scrollMenuPosition(int menuIndex);
  int maxItems();
  void drawLeftTitleIcon(std::vector<std::shared_ptr<MenuTitleBase>> menuTitles,
                         std::shared_ptr<MenuTitleToggle> toggleTitle, int i,
                         int menuState, int yPos);
  void drawRightTitleIcon(
      std::vector<std::shared_ptr<MenuTitleBase>> menuTitles, int i,
      int menuState, int yPos);
  void updateDisplay(bool force);
  void skipBootSequence(const MenuStates activeMenuState);
  // void set_display_state(HomeThingMenuDisplayState* new_display_state) {
  //   display_state_ = new_display_state;
  // }
  // void set_display_buffer(display::DisplayBuffer* new_display_buffer) {
  //   display_buffer_ = new_display_buffer;
  // }
  // void set_text_helpers(HomeThingMenuTextHelpers* new_text_helpers) {
  //   text_helpers_ = new_text_helpers;
  // }
  // void set_refactor(HomeThingMenuRefactor* new_refactor) {
  //   refactor_ = new_refactor;
  // }
  // void set_now_playing(HomeThingMenuNowPlaying* new_now_playing) {
  //   now_playing_ = new_now_playing;
  // }
  // void set_menu_header(HomeThingMenuHeader* new_menu_header) {
  //   header_ = new_menu_header;
  // }
  // void set_menu_boot(HomeThingMenuBoot* boot) { boot_ = boot; }
  void set_animation(HomeThingMenuAnimation* animation) {
    animation_ = animation;
    boot_->set_animation(animation);
  }

 private:
  int scrollTop = 0;
  int autoClearState = 0;
  void drawTitleImage(
      int characterCount, int yPos,
      const homeassistant_media_player::RemotePlayerState& titleState,
      bool selected);
  void draw_menu(std::vector<std::shared_ptr<MenuTitleBase>> menuTitles,
                 const int menuIndex);

  DisplayUpdateImpl* displayUpdate = new DisplayUpdateImpl();
  display::DisplayBuffer* display_buffer_{nullptr};
  HomeThingMenuDisplayState* display_state_{nullptr};
  HomeThingMenuTextHelpers* text_helpers_{nullptr};
  HomeThingMenuAnimation* animation_{nullptr};
  HomeThingMenuHeader* header_{nullptr};
  HomeThingMenuBoot* boot_{nullptr};
  HomeThingMenuNowPlaying* now_playing_{nullptr};
  HomeThingMenuRefactor* refactor_{nullptr};
  homeassistant_service_group::HomeAssistantServiceGroup* service_group_;
  homeassistant_media_player::HomeAssistantMediaPlayerGroup* speaker_group_;
  homeassistant_light_group::HomeAssistantLightGroup* light_group_;
  homeassistant_switch_group::HomeAssistantSwitchGroup* switch_group_;
  homeassistant_sensor_group::HomeAssistantSensorGroup* sensor_group_;
};

}  // namespace homething_menu_base
}  // namespace esphome
