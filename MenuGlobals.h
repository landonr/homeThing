#ifndef MENUGLOBALS
#define MENUGLOBALS

enum MenuStates {
  rootMenu,
  sourcesMenu,
  groupMenu,
  mediaPlayersMenu,
  scenesMenu,
  lightsMenu,
  tvNowPlayingMenu,
  speakerNowPlayingMenu
};

enum MenuStringType {
  nowPlaying,
  sources,
  mediaPlayers,
  lights,
  scenes,
  backlightString,
  sleepString,
  back
};

enum OptionMenuType {
  noOptionMenu,
  volumeOptionMenu,
  tvOptionMenu,
  speakerOptionMenu,
  playingNewSourceMenu
};

std::vector <MenuStringType> rootMenuTitles() {
  return {
    nowPlaying,
    sources,
    mediaPlayers,
    scenes,
    lights,
    sleepString
  };
}

MenuStates activeMenuState = rootMenu;
int scrollTop = 0;
int menuIndex = 0;
int rotaryPosition = 0;
int batteryWidth = 24;
int activeMenuTitleCount = 0;
OptionMenuType optionMenu = noOptionMenu;

#endif