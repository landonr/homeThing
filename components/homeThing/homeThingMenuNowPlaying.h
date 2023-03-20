
#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/homeThing/homeThingMenuDisplayState.h"
#include "esphome/components/homeThing/homeThingMenuGlobals.h"
#include "esphome/components/homeThing/homeThingMenuTextHelpers.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"

namespace esphome {
namespace homething_menu_base {

struct PositionCoordinate {
  double x;
  double y;
};

static PositionCoordinate get_coordinate(double radius, double angle) {
  PositionCoordinate coordinate;
  coordinate.x = radius * cos(angle);
  coordinate.y = radius * sin(angle);
  return coordinate;
}

class HomeThingMenuNowPlaying {
 public:
  HomeThingMenuNowPlaying(display::DisplayBuffer* new_display_buffer,
                          HomeThingMenuDisplayState* new_display_state,
                          HomeThingMenuTextHelpers* new_text_helpers)
      : display_buffer_(new_display_buffer),
        display_state_(new_display_state),
        text_helpers_(new_text_helpers) {}
  void drawNowPlaying(int menuIndex, const option_menuType option_menu,
                      std::vector<std::shared_ptr<MenuTitleBase>> active_menu);
  void drawCircleOptionMenu();
  void drawVolumeOptionMenu();

  void set_media_player_group(
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          media_player_group) {
    media_player_group_ = media_player_group;
  }

 private:
  display::DisplayBuffer* display_buffer_{nullptr};
  HomeThingMenuDisplayState* display_state_{nullptr};
  HomeThingMenuTextHelpers* text_helpers_{nullptr};
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
      media_player_group_{nullptr};

  std::string secondsToString(int seconds);
  void drawMediaDuration();
  bool drawOptionMenuAndStop(const option_menuType option_menu);
  std::string stringForNowPlayingMenuState(NowPlayingMenuState state);
  void drawNowPlayingSelectMenu(
      std::vector<std::shared_ptr<MenuTitleBase>> menu_titles, int menu_index);
  std::vector<std::string>* getWrappedTitles(int xPos, int fontSize,
                                             display::TextAlign alignment,
                                             std::string text);
  int drawTextWrapped(int xPos, int yPos, int fontSize, display::Font* font,
                      Color color, display::TextAlign alignment,
                      std::vector<std::string> wrappedTitles, int maxLines);

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
}  // namespace homething_menu_base
}  // namespace esphome
