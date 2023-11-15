#pragma once

#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeThing/homeThingMenuScreen.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/components/homeThingAppNowPlaying/NowPlayingMenuState.h"
#include "esphome/components/homeThingDisplayState/homeThingDisplayState.h"
#include "esphome/components/homeThingDisplayState/homeThingMenuTextHelpers.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"

namespace esphome {
namespace homething_menu_now_playing {

class HomeThingMenuNowPlayingHeader
    : public homething_menu_base::HomeThingMenuHeaderSource {
 public:
  HomeThingMenuNowPlayingHeader(
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          media_player_group,
      NowPlayingMenuState* menu_state)
      : media_player_group_(media_player_group), menu_state_(menu_state) {}
  // header
  std::string get_header_title();

  int draw_header_details(
      int xPos, int yPos, display::DisplayBuffer* display_buffer,
      homething_display_state::HomeThingDisplayState* display_state);

 protected:
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
      media_player_group_{nullptr};

 private:
  int drawPlayPauseIcon(
      int oldXPos, int yPos, display::DisplayBuffer* display_buffer,
      homething_display_state::HomeThingDisplayState* display_state);
  int drawShuffle(
      int oldXPos, int yPos, display::DisplayBuffer* display_buffer,
      homething_display_state::HomeThingDisplayState* display_state);
  int drawRepeat(int oldXPos, int yPos, display::DisplayBuffer* display_buffer,
                 homething_display_state::HomeThingDisplayState* display_state);
  int drawHeaderVolumeLevel(
      int oldXPos, int yPos, display::DisplayBuffer* display_buffer,
      homething_display_state::HomeThingDisplayState* display_state);
  const char* const TAG = "homething.nowplaying.control.header";
  NowPlayingMenuState* menu_state_;
};
}  // namespace homething_menu_now_playing
}  // namespace esphome