#pragma once

#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeThing/homeThingMenuScreen.h"
#include "esphome/components/homeThing/homeThingMenuTextHelpers.h"
#include "esphome/components/homeThingDisplayState/homeThingDisplayState.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"

namespace esphome {
namespace homething_menu_now_playing {

class HomeThingMenuNowPlayingHeader
    : public homething_menu_base::HomeThingMenuHeaderSource {
 public:
  HomeThingMenuNowPlayingHeader(
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          media_player_group,
      int* app_menu_index)
      : media_player_group_(media_player_group),
        app_menu_index_(app_menu_index) {}
  // header
  std::string get_header_title();

  int draw_header_details(
      int xPos, int yPos, display::DisplayBuffer* display_buffer,
      homething_display_state::HomeThingDisplayState* display_state,
      homething_menu_base::HomeThingMenuTextHelpers* text_helpers);

 protected:
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
      media_player_group_{nullptr};

 private:
  int drawPlayPauseIcon(
      int oldXPos, int yPos, display::DisplayBuffer* display_buffer,
      homething_display_state::HomeThingDisplayState* display_state,
      homething_menu_base::HomeThingMenuTextHelpers* text_helpers);
  int drawShuffle(int oldXPos, int yPos, display::DisplayBuffer* display_buffer,
                  homething_display_state::HomeThingDisplayState* display_state,
                  homething_menu_base::HomeThingMenuTextHelpers* text_helpers);
  int drawRepeat(int oldXPos, int yPos, display::DisplayBuffer* display_buffer,
                 homething_display_state::HomeThingDisplayState* display_state,
                 homething_menu_base::HomeThingMenuTextHelpers* text_helpers);
  int drawHeaderVolumeLevel(
      int oldXPos, int yPos, display::DisplayBuffer* display_buffer,
      homething_display_state::HomeThingDisplayState* display_state,
      homething_menu_base::HomeThingMenuTextHelpers* text_helpers);
  const char* const TAG = "homething.nowplaying.control.header";
  int* app_menu_index_;
};
}  // namespace homething_menu_now_playing
}  // namespace esphome