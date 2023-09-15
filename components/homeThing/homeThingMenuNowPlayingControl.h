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
  void idleTick(int idleTime, int display_timeout);
  int root_menu_size();
  void selectNowPlayingMenu();
  void reset_menu();

  void rotaryScrollClockwise(int rotary);
  void rotaryScrollCounterClockwise(int rotary);
  void buttonPressUp();
  void buttonPressDown();
  void buttonPressLeft();
  void buttonPressRight();
  void buttonPressSelect(int menuIndex);
  void buttonPressSelectHold();
  void buttonPressScreenLeft();
  void buttonReleaseScreenLeft();
  void buttonPressScreenRight();

  // controls
  bool select_media_player_feature(
      homeassistant_media_player::MediaPlayerFeatureCommand* command);
  bool button_press_now_playing_option_continue(
      CircleOptionMenuPosition position);

 protected:
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
      media_player_group_{nullptr};
  HomeThingMenuNowPlayingOptionMenu* circle_menu_ =
      new HomeThingMenuNowPlayingOptionMenu();

 private:
  const char* const TAG = "homething.nowplaying.control";
};
}  // namespace homething_menu_now_playing
}  // namespace esphome