#pragma once
#include <string>

namespace esphome {
namespace homething_menu_now_playing {
enum NowPlayingMenuState : int {
  NOW_PLAYING_MENU_STATE_NOW_PLAYING = 0,
  NOW_PLAYING_MENU_STATE_SOURCE = 1,
  NOW_PLAYING_MENU_STATE_MEDIA_PLAYERS = 2,
  NOW_PLAYING_MENU_STATE_GROUPING = 3,
};

static std::string get_menu_title(NowPlayingMenuState menu_state) {
  switch (menu_state) {
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
  return "xx2";
}
}  // namespace homething_menu_now_playing
}  // namespace esphome