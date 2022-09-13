#ifndef MENUGLOBALS
#define MENUGLOBALS

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

std::vector <MenuStates> rootMenuTitles() {
  return {
    nowPlayingMenu,
    sourcesMenu,
    mediaPlayersMenu,
    scenesMenu,
    lightsMenu,
    sensorsMenu,
    sleepMenu
  };
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
  homeNowPlayingMenuState
};

std::vector <NowPlayingMenuState> speakerNowPlayingMenuStates() {
  return {
    pauseNowPlayingMenuState,
    volumeUpNowPlayingMenuState,
    volumeDownNowPlayingMenuState,
    nextNowPlayingMenuState,
    shuffleNowPlayingMenuState,
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

#endif