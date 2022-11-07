#pragma once

enum MenuStates {
  bootMenu,
  rootMenu,
  sourcesMenu,
  groupMenu,
  mediaPlayersMenu,
  scenesMenu,
  lightsMenu,
  lightsDetailMenu,
  nowPlayingMenu,
  sensorsMenu,
  backlightMenu,
  sleepMenu
};

std::vector<MenuStates> rootMenuTitles(bool includeSpeaker, bool includeScene, bool includeSensor, bool includeLight) {
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

std::vector<NowPlayingMenuState> speakerNowPlayingMenuStates() {
  return {pauseNowPlayingMenuState, volumeUpNowPlayingMenuState, volumeDownNowPlayingMenuState,
          nextNowPlayingMenuState,  shuffleNowPlayingMenuState,  groupNowPlayingMenuState,
          menuNowPlayingMenuState};
}

std::vector<NowPlayingMenuState> TVNowPlayingMenuStates() {
  return {pauseNowPlayingMenuState, volumeUpNowPlayingMenuState, volumeDownNowPlayingMenuState,
          backNowPlayingMenuState,  TVPowerNowPlayingMenuState,  homeNowPlayingMenuState,
          menuNowPlayingMenuState};
}

enum OptionMenuType { noOptionMenu, volumeOptionMenu, tvOptionMenu, speakerOptionMenu, playingNewSourceMenu };

MenuStates activeMenuState = bootMenu;
int idleTime = -2;
bool charging = false;
int scrollTop = 0;
int menuIndex = 0;
int rotaryPosition = 0;
int activeMenuTitleCount = 0;
OptionMenuType optionMenu = noOptionMenu;
