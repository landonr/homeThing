#pragma once
#include <string>
#include <vector>
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/time/real_time_clock.h"

namespace esphome {
namespace homething_menu_base {

static std::vector<MenuStates> rootMenuTitles(bool includeSpeaker,
                                              bool includeScene,
                                              bool includeSensor,
                                              bool includeLight,
                                              bool includeSwitch) {
  std::vector<MenuStates> out;
  if (includeSpeaker) {
    out.insert(out.end(), {nowPlayingMenu, sourcesMenu, mediaPlayersMenu});
  }
  if (includeScene) {
    out.push_back(scenesMenu);
  }
  if (includeSensor) {
    out.push_back(sensorsMenu);
  }
  if (includeLight) {
    out.push_back(lightsMenu);
  }
  if (includeSwitch) {
    out.push_back(switchesMenu);
  }
  out.push_back(sleepMenu);
  return out;
}

enum option_menuType {
  noOptionMenu,
  volumeOptionMenu,
  tvOptionMenu,
  speakerOptionMenu,
  playingNewSourceMenu
};
}  // namespace homething_menu_base
}  // namespace esphome
