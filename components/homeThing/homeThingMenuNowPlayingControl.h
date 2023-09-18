#pragma once

#include "esphome/components/homeThing/homeThingMenuNowPlaying.h"
#include "esphome/components/homeThing/homeThingMenuNowPlayingOptionMenu.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/components/homeThing/homeThingOptionMenu.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"

namespace esphome {
namespace homething_menu_now_playing {

class HomeThingMenuNowPlayingControl {
 public:
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
  get_media_player_group();
  void set_media_player_group(
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          media_player_group);
  void rootMenuTitles(
      std::vector<homething_menu_base::MenuTitleBase*>* menu_titles);
  void app_menu_titles(
      std::vector<homething_menu_base::MenuTitleBase*>* menu_titles);
  void sourceMenuTitles(
      std::vector<homething_menu_base::MenuTitleBase*>* menu_titles);
  bool app_menu_select(int index);
  bool should_draw_app();
  void draw_app(
      int menuIndex,
      const std::vector<homething_menu_base::MenuTitleBase*>* active_menu);
  void idleTick(int idleTime, int display_timeout);
  int root_menu_size();
  void selectNowPlayingMenu();
  void reset_menu();
  void set_app_menu_index(int app_menu_index);
  void rotaryScrollClockwise(int rotary);
  void rotaryScrollCounterClockwise(int rotary);
  bool buttonPressUp();
  bool buttonPressDown();
  bool buttonPressLeft();
  bool buttonPressRight();
  bool buttonPressSelect(int menuIndex);
  bool buttonPressSelectHold();
  bool buttonPressScreenLeft();
  bool buttonReleaseScreenLeft();
  bool buttonPressScreenRight();

  // controls
  bool select_media_player_feature(
      homeassistant_media_player::MediaPlayerFeatureCommand* command);
  bool button_press_now_playing_option_continue(
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
};
}  // namespace homething_menu_now_playing
}  // namespace esphome