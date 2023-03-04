#include "esphomeRemote.h"
#include "esphomeRemoteNowPlayingMenu.h"

#pragma once

void buttonPressSelect() {
  resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
    case lightsDetailMenu:
      if (light_group_->lightDetailSelected) {
        // deselect ligh if selected and stay in lightsDetailMenu
        light_group_->lightDetailSelected = false;
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
  resetAnimation();
  optionMenu = noOptionMenu;
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  if (activeMenuState == lightsDetailMenu && menuIndex > 0 &&
      light_group_->lightDetailSelected) {
    if (menuIndex == 1) {
      light_group_->getActiveLight()->incBrightness();
    } else if (menuIndex == 2) {
      light_group_->getActiveLight()->incTemperature();
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
  resetAnimation();
  optionMenu = noOptionMenu;
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  if (activeMenuState == lightsDetailMenu && menuIndex > 0 &&
      light_group_->lightDetailSelected) {
    if (menuIndex == 1) {
      light_group_->getActiveLight()->decBrightness();
    } else if (menuIndex == 2) {
      light_group_->getActiveLight()->decTemperature();
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
