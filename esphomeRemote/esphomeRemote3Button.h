#include "esphomeRemote.h"
#include "esphomeRemoteNowPlayingMenu.h"

#pragma once

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

void buttonPressLeft() {
  resetMarquee();
  optionMenu = noOptionMenu;
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  if (menuIndex > 0) {
    menuIndex--;
  } else if (activeMenuState == nowPlayingMenu) {
    menuIndex = activeMenuTitleCount - 1;
  } else {
    topMenu();
  }
  displayUpdate.updateDisplay(true);
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
    menuIndex = 0;
  } else {
    menuIndex = 0;
  }
  displayUpdate.updateDisplay(true);
}
