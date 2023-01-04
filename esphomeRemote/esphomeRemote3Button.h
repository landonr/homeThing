#include "esphomeRemote.h"
#include "esphomeRemoteNowPlayingMenu.h"

#pragma once

void buttonPressSelect() {
  resetMarquee();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
    case lightsDetailMenu:
      if (lightGroup->lightDetailSelected) {
        // deselect ligh if selected and stay in lightsDetailMenu
        lightGroup->lightDetailSelected = false;
        displayUpdate.updateDisplay(true);
        return;
      }
      break;
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
  if (activeMenuState == lightsDetailMenu && menuIndex > 0 && lightGroup->lightDetailSelected) {
    if (menuIndex == 1) {
      lightGroup->getActiveLight()->incBrightness();
    } else if (menuIndex == 2) {
      lightGroup->getActiveLight()->incTemperature();
    }
    if (selectMenu()) {
      displayUpdate.updateDisplay(true);
    }
  } else if (menuIndex > 0) {
    menuIndex--;
  } else if (activeMenuState == nowPlayingMenu) {
    menuIndex = activeMenuTitleCount - 1;
  } else if (activeMenuState == lightsDetailMenu && menuIndex == 0) {
    activeMenuState = lightsMenu;
    displayUpdate.updateDisplay(true);
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
  if (activeMenuState == lightsDetailMenu && menuIndex > 0 && lightGroup->lightDetailSelected) {
    if (menuIndex == 1) {
      lightGroup->getActiveLight()->decBrightness();
    } else if (menuIndex == 2) {
      lightGroup->getActiveLight()->decTemperature();
    }
    if (selectMenu()) {
      displayUpdate.updateDisplay(true);
    }
  } else if (menuIndex < activeMenuTitleCount - 1) {
    menuIndex++;
  } else if (menuIndex == activeMenuTitleCount - 1) {
    menuIndex = 0;
  } else {
    menuIndex = 0;
  }
  displayUpdate.updateDisplay(true);
}
