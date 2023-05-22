#pragma once
#include <string>
#include <vector>

namespace esphome {
namespace homething_menu_base {

struct PositionCoordinate {
  double x;
  double y;
};

enum CircleOptionMenuPosition : int {
  RIGHT = 0,
  BOTTOM = 1,
  LEFT = 2,
  TOP = 3,
  CENTER = 4
};

struct CircleOptionMenuItem {
  CircleOptionMenuPosition position;
#ifdef USE_MEDIA_PLAYER_GROUP
  homeassistant_media_player::MediaPlayerSupportedFeature feature;
#endif
};

enum HomeThingOptionMenuType {
  noOptionMenu,
  volumeOptionMenu,
  tvOptionMenu,
  speakerOptionMenu,
  playingNewSourceMenu
};

struct HomeThingOptionMenu {
  HomeThingOptionMenuType type;
  std::vector<CircleOptionMenuItem> circle_options;
  HomeThingOptionMenu(HomeThingOptionMenuType type) : type(type) {
    circle_options = std::vector<CircleOptionMenuItem>();
  }
};
}  // namespace homething_menu_base
}  // namespace esphome
