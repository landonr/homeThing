#include "NowPlayingMenuState.h"

namespace esphome {
namespace homething_menu_now_playing {
std::string get_menu_title(NowPlayingMenuState menu_state) {
  switch (menu_state) {
    case NOW_PLAYING_MENU_STATE_NONE:
      return "None";
    case NOW_PLAYING_MENU_STATE_NOW_PLAYING:
      return "Now Playing";
    case NOW_PLAYING_MENU_STATE_SOURCE:
      return "Sources";
    case NOW_PLAYING_MENU_STATE_MEDIA_PLAYERS:
      return "Media Players";
    case NOW_PLAYING_MENU_STATE_GROUPING:
      return "Grouping";
    default:
      break;
  }
  return "Unknown";
}
}  // namespace homething_menu_now_playing
}  // namespace esphome