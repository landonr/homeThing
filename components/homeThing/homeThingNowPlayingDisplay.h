#pragma once

#ifdef USE_MEDIA_PLAYER_GROUP

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/homeThing/homeThingMenuTextHelpers.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/components/homeThing/homeThingOptionMenu.h"
#include "esphome/components/homeThingDisplayState/homeThingDisplayState.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"
namespace esphome {
namespace homething_menu_now_playing {
class HomeThingMenuNowPlaying {
 public:
  HomeThingMenuNowPlaying(
      display::DisplayBuffer* new_display_buffer,
      homething_display_state::HomeThingDisplayState* new_display_state,
      homething_menu_base::HomeThingMenuTextHelpers* new_text_helpers,
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          new_media_player_group)
      : display_buffer_(new_display_buffer),
        display_state_(new_display_state),
        text_helpers_(new_text_helpers),
        media_player_group_(new_media_player_group) {}
  PositionCoordinate get_coordinate(double radius, double angle);
  void drawNowPlaying(
      int menuIndex, HomeThingOptionMenu* option_menu,
      const std::vector<homething_menu_base::MenuTitleBase*>* active_menu);

 private:
  display::DisplayBuffer* display_buffer_{nullptr};
  homething_display_state::HomeThingDisplayState* display_state_{nullptr};
  homething_menu_base::HomeThingMenuTextHelpers* text_helpers_{nullptr};
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
      media_player_group_{nullptr};
  void drawCircleOptionMenu(
      std::vector<CircleOptionMenuItem> supported_features);
  void drawVolumeOptionMenu();
  std::string secondsToString(int seconds);
  void drawMediaDuration();
  bool drawOptionMenuAndStop(const HomeThingOptionMenu* option_menu);
  void drawNowPlayingSelectMenu(
      const std::vector<homething_menu_base::MenuTitleBase*>* menu_titles,
      int menu_index);
  std::vector<std::string>* getWrappedTitles(int xPos, int fontSize,
                                             display::TextAlign alignment,
                                             std::string text);
  int drawTextWrapped(int xPos, int yPos, int fontSize, font::Font* font,
                      Color color, display::TextAlign alignment,
                      std::vector<std::string> wrappedTitles, int maxLines);
  display::TextAlign text_align_for_circle_position(
      CircleOptionMenuPosition position);
  void tokenize(std::string const& str, std::string delim,
                std::vector<std::string>* out) {
    size_t start;
    size_t end = 0;

    while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
      end = str.find(delim, start);
      out->push_back(str.substr(start, end - start));
    }
  }

  const char* const TAG = "homething.menu.now_playing";
};
}  // namespace homething_menu_now_playing
}  // namespace esphome
#endif