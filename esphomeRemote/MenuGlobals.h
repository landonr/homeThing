#pragma once

enum MenuStates {
  bootMenu,
  rootMenu,
  sourcesMenu,
  groupMenu,
  mediaPlayersMenu,
  scenesMenu,
  lightsMenu,
  nowPlayingMenu,
  sensorsMenu,
  backlightMenu,
  sleepMenu
};

std::vector <MenuStates> rootMenuTitles(bool includeSpeaker) {
  if (includeSpeaker) {
    return {
      nowPlayingMenu,
      sourcesMenu,
      mediaPlayersMenu,
      scenesMenu,
      lightsMenu,
      sensorsMenu,
      sleepMenu
    };
  } else {
    return {
      scenesMenu,
      lightsMenu,
      sensorsMenu,
      sleepMenu
    };
  }
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

std::vector <NowPlayingMenuState> speakerNowPlayingMenuStates() {
  return {
    pauseNowPlayingMenuState,
    volumeUpNowPlayingMenuState,
    volumeDownNowPlayingMenuState,
    nextNowPlayingMenuState,
    shuffleNowPlayingMenuState,
    groupNowPlayingMenuState,
    menuNowPlayingMenuState
  };
}

std::vector <NowPlayingMenuState> TVNowPlayingMenuStates() {
  return {
    pauseNowPlayingMenuState,
    volumeUpNowPlayingMenuState,
    volumeDownNowPlayingMenuState,
    backNowPlayingMenuState,
    TVPowerNowPlayingMenuState,
    homeNowPlayingMenuState,
    menuNowPlayingMenuState
  };
}

enum OptionMenuType {
  noOptionMenu,
  volumeOptionMenu,
  tvOptionMenu,
  speakerOptionMenu,
  playingNewSourceMenu
};

MenuStates activeMenuState = bootMenu;
int idleTime = -2;
bool charging = false;
int scrollTop = 0;
int menuIndex = 0;
int rotaryPosition = 0;
int activeMenuTitleCount = 0;
OptionMenuType optionMenu = noOptionMenu;
