#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/homeThing/homeThingMenuAnimation.h"
#include "esphome/components/homeThing/homeThingMenuBoot.h"
#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeThing/homeThingMenuRefactor.h"
#include "esphome/components/homeThing/homeThingOptionMenu.h"
#include "esphome/components/homeThingDisplayState/homeThingDisplayState.h"
#include "esphome/components/homeThingDisplayState/homeThingMenuTextHelpers.h"

#include "esphome/components/homeThing/homeThingMenuTitle.h"
#ifdef USE_LIGHT
#include "esphome/components/homeThing/homeThingMenuTitleLight.h"
#endif

#ifdef USE_MEDIA_PLAYER_GROUP
#include "esphome/components/homeThing/homeThingMenuTitlePlayer.h"
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
  HomeThingMenuDisplay(
      display::DisplayBuffer* display_buffer,
      homething_display_state::HomeThingDisplayState* display_state,
      HomeThingMenuRefactor* refactor, HomeThingMenuHeader* header)
      : display_buffer_(display_buffer),
        display_state_(display_state),
        refactor_(refactor),
        header_(header) {}
  void draw_background();
  void draw_lock_screen(int unlock_presses);
  bool draw_menu_screen(
      MenuStates* activeMenuState,
      const std::vector<MenuTitleBase*>* active_menu, const int menuIndex,
      homething_menu_now_playing::HomeThingOptionMenu* option_menu,
      bool editing_menu_item);
  void draw_menu_header(HomeThingMenuHeaderSource* header_source);
  void updateDisplay(bool force);

  void set_boot(HomeThingMenuBoot* boot);
  void set_animation(HomeThingMenuAnimation* animation) {
    animation_ = animation;
    if (boot_) {
      boot_->set_animation(animation);
    }
  }

  void set_active_menu_screen(HomeThingMenuScreen** active_menu_screen) {
    header_->set_active_menu_screen(active_menu_screen);
  }

  void clearBoot() { boot_ = nullptr; }

  bool boot_complete();
  BootMenuSkipState bootSequenceCanSkip(const MenuStates activeMenuState);

  void add_on_state_callback(std::function<void()>&& callback) {
    this->callback_.add(std::move(callback));
  }

 private:
  HomeThingMenuBoot* boot_{nullptr};
  int scrollTop = 0;
  bool draw_menu_titles(const std::vector<MenuTitleBase*>* menuTitles,
                        const int menuIndex, bool editing_menu_item);
  bool draw_menu_title(int menuState, int i, std::string title, int yPos,
                       bool buttonSpace);
  void drawScrollBar(int menuTitlesCount, int headerHeight, int menuIndex);
  void scrollMenuPosition(int menuIndex);
  int maxItems();
  void drawLeftTitleIcon(int menuTitleSize, const MenuTitleToggle* toggleTitle,
                         int i, int menuState, int yPos);
  void drawRightTitleIcon(int menuTitleSize, MenuTitleRightIcon iconState,
                          int i, int menuState, int yPos);

  display::DisplayBuffer* display_buffer_{nullptr};
  homething_display_state::HomeThingDisplayState* display_state_{nullptr};
  HomeThingMenuRefactor* refactor_{nullptr};
  HomeThingMenuHeader* header_{nullptr};
  HomeThingMenuAnimation* animation_{nullptr};
  CallbackManager<void()> callback_;

#ifdef USE_MEDIA_PLAYER_GROUP
  void drawTitleImage(
      int characterCount, int yPos,
      const homeassistant_media_player::RemotePlayerState& titleState,
      bool selected);
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
      media_player_group_{nullptr};
#endif

  const char* const TAG = "homething.menu.display";
};

}  // namespace homething_menu_base
}  // namespace esphome
