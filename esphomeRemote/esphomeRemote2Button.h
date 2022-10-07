#include "esphomeRemote.h"
#include "esphomeRemoteNowPlayingMenu.h"

#ifndef REMOTETWOBUTTON
#define REMOTETWOBUTTON

void buttonPressSelect() {
  resetMarquee();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
  case nowPlayingMenu:
    if (optionMenu == tvOptionMenu) {
      optionMenu = noOptionMenu;
      displayUpdate.updateDisplay(true);
      return;
    }

    selectNowPlayingMenu();
    return;
  default:
    break;
  }
  if (selectMenu()) {
    displayUpdate.updateDisplay(true);
  }
}

void buttonPressRight() {
  resetMarquee();
  optionMenu = noOptionMenu;
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  if (menuIndex < activeMenuTitleCount - 1) {
    menuIndex++;
  } else if (menuIndex == activeMenuTitleCount - 1) {
    topMenu();
  } else {
    menuIndex = 0;
  }
  displayUpdate.updateDisplay(true);
}

#endif