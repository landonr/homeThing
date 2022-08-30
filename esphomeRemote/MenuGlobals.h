#ifndef MENUGLOBALS
#define MENUGLOBALS

enum MenuStates {
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

enum OptionMenuType {
  noOptionMenu,
  volumeOptionMenu,
  tvOptionMenu,
  speakerOptionMenu,
  playingNewSourceMenu
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

MenuStates activeMenuState = rootMenu;
int idleTime = -2;
bool charging = false;
int scrollTop = 0;
int menuIndex = 0;
int rotaryPosition = 0;
int batteryWidth = 24;
int activeMenuTitleCount = 0;
OptionMenuType optionMenu = noOptionMenu;

#endif