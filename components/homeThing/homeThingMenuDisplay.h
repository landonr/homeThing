#pragma once

#include <memory>
#include <string>
#include <vector>
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/homeThing/homeThingMenuAnimation.h"
#include "esphome/components/homeThing/homeThingMenuBoot.h"
#include "esphome/components/homeThing/homeThingMenuDisplayState.h"
#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeThing/homeThingMenuRefactor.h"
#include "esphome/components/homeThing/homeThingMenuTextHelpers.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/components/homeThing/homeThingOptionMenu.h"

#ifdef USE_LIGHT_GROUP
#include "esphome/components/homeassistant_light_group/HomeAssistantLightGroup.h"
#endif

#ifdef USE_MEDIA_PLAYER_GROUP
#include "esphome/components/homeThing/homeThingMenuNowPlaying.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"
#endif

#ifdef USE_SENSOR_GROUP
#include "esphome/components/homeassistant_sensor_group/HomeAssistantSensorGroup.h"
#endif

#ifdef USE_SERVICE_GROUP
#include "esphome/components/homeassistant_service_group/HomeAssistantServiceGroup.h"
#endif

#ifdef USE_SWITCH_GROUP
#include "esphome/components/homeassistant_switch_group/HomeAssistantSwitchGroup.h"
#endif

#include "esphome/components/light/light_output.h"
#include "esphome/components/light/light_state.h"
#include "esphome/core/component.h"

namespace esphome {
namespace homething_menu_base {

class HomeThingMenuDisplay {
 public:
  HomeThingMenuDisplay(display::DisplayBuffer* display_buffer,
                       HomeThingMenuDisplayState* display_state,
                       HomeThingMenuTextHelpers* text_helpers,
                       HomeThingMenuRefactor* refactor,
                       HomeThingMenuHeader* header, HomeThingMenuBoot* boot)
      : display_buffer_(display_buffer),
        display_state_(display_state),
        text_helpers_(text_helpers),
        refactor_(refactor),
        header_(header),
        boot_(boot) {}
  void setup();
  void draw_lock_screen(int unlock_presses);
  bool draw_menu_screen(
      MenuStates* activeMenuState,
      const std::vector<std::shared_ptr<MenuTitleBase>>* active_menu,
      const int menuIndex, HomeThingOptionMenu* option_menu);
  void updateDisplay(bool force);

  void set_animation(HomeThingMenuAnimation* animation) {
    animation_ = animation;
    boot_->set_animation(animation);
  }

#ifdef USE_MEDIA_PLAYER_GROUP
  void set_media_player_group(
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          media_player_group) {
    media_player_group_ = media_player_group;
  }
  void set_now_playing(HomeThingMenuNowPlaying* now_playing) {
    now_playing_ = now_playing;
  }
#endif

#ifdef USE_LIGHT_GROUP
  void set_light_group(
      homeassistant_light_group::HomeAssistantLightGroup* light_group) {
    light_group_ = light_group;
  }
#endif
  HomeThingMenuBoot* boot_{nullptr};

 private:
  int scrollTop = 0;
  bool draw_menu_titles(
      const std::vector<std::shared_ptr<MenuTitleBase>>* menuTitles,
      const int menuIndex);
  bool draw_menu_title(int menuState, int i, std::string title, int yPos,
                       bool buttonSpace);
  void drawScrollBar(int menuTitlesCount, int headerHeight, int menuIndex);
  void scrollMenuPosition(int menuIndex);
  int maxItems();
  void drawLeftTitleIcon(int menuTitleSize,
                         std::shared_ptr<MenuTitleToggle> toggleTitle, int i,
                         int menuState, int yPos);
  void drawRightTitleIcon(int menuTitleSize, MenuTitleRightIcon iconState,
                          int i, int menuState, int yPos);

  display::DisplayBuffer* display_buffer_{nullptr};
  HomeThingMenuDisplayState* display_state_{nullptr};
  HomeThingMenuTextHelpers* text_helpers_{nullptr};
  HomeThingMenuAnimation* animation_{nullptr};
  HomeThingMenuHeader* header_{nullptr};
  HomeThingMenuRefactor* refactor_{nullptr};

#ifdef USE_MEDIA_PLAYER_GROUP
  void drawTitleImage(
      int characterCount, int yPos,
      const homeassistant_media_player::RemotePlayerState& titleState,
      bool selected);
  HomeThingMenuNowPlaying* now_playing_{nullptr};
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
      media_player_group_{nullptr};
#endif

#ifdef USE_LIGHT_GROUP
  homeassistant_light_group::HomeAssistantLightGroup* light_group_{nullptr};
#endif
  const char* const TAG = "homething.menu.display";
};

}  // namespace homething_menu_base
}  // namespace esphome
