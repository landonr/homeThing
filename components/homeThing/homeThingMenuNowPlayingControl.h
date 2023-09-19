#pragma once

#include "esphome/components/homeThing/homeThingMenuApp.h"
#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeThing/homeThingMenuNowPlaying.h"
#include "esphome/components/homeThing/homeThingMenuNowPlayingOptionMenu.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/components/homeThing/homeThingOptionMenu.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"

#include "esphome/components/homeThing/HomeThingMenuNowPlayingHeader.h"
#include "esphome/components/homeThing/homeThingMenuDisplayState.h"
#include "esphome/components/homeThing/homeThingMenuScreen.h"
#include "esphome/components/homeThing/homeThingMenuTextHelpers.h"

namespace esphome {
namespace homething_menu_now_playing {

class HomeThingMenuNowPlayingControl
    : public homething_menu_app::HomeThingMenuApp {
 public:
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
  get_media_player_group();
  void set_media_player_group(
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          media_player_group);

  // menu titles
  void rootMenuTitles(
      std::vector<homething_menu_base::MenuTitleBase*>* menu_titles);
  void app_menu_titles(
      std::vector<homething_menu_base::MenuTitleBase*>* menu_titles);

  // menu screens
  homething_menu_app::NavigationCoordination app_menu_select(int index);
  bool should_draw_app();
  void draw_app(
      int menuIndex,
      const std::vector<homething_menu_base::MenuTitleBase*>* active_menu);
  void idleTick(int idleTime, int display_timeout);
  int root_menu_size();
  //   void selectNowPlayingMenu();
  void reset_menu();
  void set_app_menu_index(int app_menu_index);

  // buttons
  void rotaryScrollClockwise(int rotary);
  void rotaryScrollCounterClockwise(int rotary);
  homething_menu_app::NavigationCoordination buttonPressUp();
  homething_menu_app::NavigationCoordination buttonPressDown();
  homething_menu_app::NavigationCoordination buttonPressLeft();
  homething_menu_app::NavigationCoordination buttonPressRight();
  homething_menu_app::NavigationCoordination buttonPressSelect(int menuIndex);
  homething_menu_app::NavigationCoordination buttonPressSelectHold();
  homething_menu_app::NavigationCoordination buttonPressScreenLeft();
  homething_menu_app::NavigationCoordination buttonReleaseScreenLeft();
  homething_menu_app::NavigationCoordination buttonPressScreenRight();

  // controls
  bool select_media_player_feature(
      homeassistant_media_player::MediaPlayerFeatureCommand* command);
  homething_menu_app::NavigationCoordination button_press_now_playing_option(
      CircleOptionMenuPosition position);

  // display
  void set_now_playing_display(
      display::DisplayBuffer* new_display_buffer,
      homething_menu_base::HomeThingMenuDisplayState* new_display_state,
      homething_menu_base::HomeThingMenuTextHelpers* new_text_helpers,
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          new_media_player_group) {
    now_playing_display_ =
        new HomeThingMenuNowPlaying(new_display_buffer, new_display_state,
                                    new_text_helpers, new_media_player_group);
  }

  homething_menu_base::HomeThingMenuHeaderSource* get_header_source() {
    return header_source_;
  }
  HomeThingMenuHeaderSource* header_source_{nullptr};

 protected:
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
      media_player_group_{nullptr};
  HomeThingMenuNowPlayingOptionMenu* circle_menu_ =
      new HomeThingMenuNowPlayingOptionMenu();
  HomeThingMenuNowPlaying* now_playing_display_{nullptr};
  void select_source_menu(int index);
  int app_menu_index_ = 0;

 private:
  const char* const TAG = "homething.nowplaying.control";

  // menu titles

  void sourceMenuTitles(
      std::vector<homething_menu_base::MenuTitleBase*>* menu_titles);
  void media_player_menu_titles(
      std::vector<homething_menu_base::MenuTitleBase*>* menu_titles);
};
}  // namespace homething_menu_now_playing
}  // namespace esphome