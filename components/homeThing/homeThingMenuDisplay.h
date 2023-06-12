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

#ifdef USE_LIGHT
#include "esphome/components/homeThing/homeThingMenuTitleLight.h"
#endif

#ifdef USE_MEDIA_PLAYER_GROUP
#include "esphome/components/homeThing/homeThingMenuNowPlaying.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"
#endif

#ifdef USE_LIGHT
#include "esphome/components/light/light_output.h"
#include "esphome/components/light/light_state.h"
#endif

#include "esphome/core/component.h"

namespace esphome {
namespace homething_menu_base {

class HomeThingMenuDisplay {
 public:
  HomeThingMenuDisplay(HomeThingMenuBoot* boot,
                       display::DisplayBuffer* display_buffer,
                       HomeThingMenuDisplayState* display_state,
                       HomeThingMenuTextHelpers* text_helpers,
                       HomeThingMenuRefactor* refactor,
                       HomeThingMenuHeader* header)
      : boot_(boot),
        display_buffer_(display_buffer),
        display_state_(display_state),
        text_helpers_(text_helpers),
        refactor_(refactor),
        header_(header) {}
  void setup();
  void draw_lock_screen(int unlock_presses);
  bool draw_menu_screen(
      MenuStates* activeMenuState,
      const std::vector<std::shared_ptr<MenuTitleBase>>* active_menu,
      const int menuIndex, HomeThingOptionMenu* option_menu,
      bool editing_menu_item);
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

  HomeThingMenuBoot* boot_{nullptr};
  void set_active_menu_screen(HomeThingMenuScreen** active_menu_screen) {
    header_->set_active_menu_screen(active_menu_screen);
  }

 private:
  int scrollTop = 0;
  bool draw_menu_titles(
      const std::vector<std::shared_ptr<MenuTitleBase>>* menuTitles,
      const int menuIndex, bool editing_menu_item);
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
  HomeThingMenuRefactor* refactor_{nullptr};
  HomeThingMenuHeader* header_{nullptr};
  HomeThingMenuAnimation* animation_{nullptr};

#ifdef USE_MEDIA_PLAYER_GROUP
  void drawTitleImage(
      int characterCount, int yPos,
      const homeassistant_media_player::RemotePlayerState& titleState,
      bool selected);
  HomeThingMenuNowPlaying* now_playing_{nullptr};
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
      media_player_group_{nullptr};
#endif

  const char* const TAG = "homething.menu.display";
};

}  // namespace homething_menu_base
}  // namespace esphome
