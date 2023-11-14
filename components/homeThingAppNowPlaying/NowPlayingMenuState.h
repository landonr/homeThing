#pragma once
#include <string>

namespace esphome {
namespace homething_menu_now_playing {
enum NowPlayingMenuState : int {
  NOW_PLAYING_MENU_STATE_NONE = 0,
  NOW_PLAYING_MENU_STATE_NOW_PLAYING = 1,
  NOW_PLAYING_MENU_STATE_SOURCE = 2,
  NOW_PLAYING_MENU_STATE_MEDIA_PLAYERS = 3,
  NOW_PLAYING_MENU_STATE_GROUPING = 4,
};

static std::string get_menu_title(NowPlayingMenuState menu_state) {
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