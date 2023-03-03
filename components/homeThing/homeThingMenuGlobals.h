#pragma once
#include <vector>
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/time/real_time_clock.h"

namespace esphome {
namespace homething_menu_base {

enum MenuStates {
  bootMenu,
  rootMenu,
  sourcesMenu,
  groupMenu,
  mediaPlayersMenu,
  scenesMenu,
  lightsMenu,
  lightsDetailMenu,
  switchesMenu,
  nowPlayingMenu,
  sensorsMenu,
  backlightMenu,
  sleepMenu
};

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

enum NowPlayingMenuState {
  pauseNowPlayingMenuState,
  volumeUpNowPlayingMenuState,
  volumeDownNowPlayingMenuState,
  nextNowPlayingMenuState,
  menuNowPlayingMenuState,
  shuffleNowPlayingMenuState,
  backNowPlayingMenuState,
  TVPowerNowPlayingMenuState,
  homeNowPlayingMenuState,
  groupNowPlayingMenuState
};

static std::vector<NowPlayingMenuState> speakerNowPlayingMenuStates() {
  return {pauseNowPlayingMenuState,      volumeUpNowPlayingMenuState,
          volumeDownNowPlayingMenuState, nextNowPlayingMenuState,
          shuffleNowPlayingMenuState,    groupNowPlayingMenuState,
          menuNowPlayingMenuState};
}

static std::vector<NowPlayingMenuState> TVNowPlayingMenuStates() {
  return {pauseNowPlayingMenuState,      volumeUpNowPlayingMenuState,
          volumeDownNowPlayingMenuState, backNowPlayingMenuState,
          TVPowerNowPlayingMenuState,    homeNowPlayingMenuState,
          menuNowPlayingMenuState};
}

enum OptionMenuType {
  noOptionMenu,
  volumeOptionMenu,
  tvOptionMenu,
  speakerOptionMenu,
  playingNewSourceMenu
};

static MenuStates activeMenuState = bootMenu;
static int display_timeout_ = 16;
static int sleep_after_ = 3600;
static int idleTime = -2;
static bool charging = false;
static int scrollTop = 0;
static int menuIndex = 0;
static int rotaryPosition = 0;
static int activeMenuTitleCount = 0;
static bool menuDrawing = false;
static bool menu_rollover_on_ = true;
static OptionMenuType optionMenu = noOptionMenu;

}  // namespace homething_menu_base
}  // namespace esphome
