#include "esphomeRemote.h"

#ifndef REMOTEFIRE
#define REMOTEFIRE

void buttonPressSelect() {
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
  case tvNowPlayingMenu:
    if (optionMenu == tvOptionMenu) {
      optionMenu = noOptionMenu;
      speakerGroup -> tv -> tvRemoteCommand("power");
      displayUpdate.updateDisplay(true);
    } else {
      speakerGroup -> tv -> tvRemoteCommand("select");
    }
    return;
  default:
    break;
  }
  if (selectMenu()) {
    displayUpdate.updateDisplay(true);
  }
}

void buttonPressLeft() {
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  if (menuIndex > 0) {
    menuIndex--;
  } else {
    topMenu();
  }
  displayUpdate.updateDisplay(true);
}

void buttonPressRight() {
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

#endif