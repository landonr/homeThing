#include "esphomeRemote.h"

#ifndef REMOTEFIRE
#define REMOTEFIRE

void selectNowPlayingMenu() {
  if(activeMenuTitleCount <= 0 && menuIndex < activeMenuTitleCount) {
    return;
  }
  auto menuTitle = getNowPlayingMenuStates()[menuIndex];
  switch(menuTitle) {
  case pauseNowPlayingMenuState:
    speakerGroup -> activePlayer -> playPause();
    break;
  case volumeUpNowPlayingMenuState:
    speakerGroup -> increaseSpeakerVolume();
    optionMenu = volumeOptionMenu;
    break;
  case volumeDownNowPlayingMenuState:
    speakerGroup -> decreaseSpeakerVolume();
    optionMenu = volumeOptionMenu;
    break;
  case nextNowPlayingMenuState:
    speakerGroup -> activePlayer -> nextTrack();
    break;
  case shuffleNowPlayingMenuState:
    speakerGroup -> toggleShuffle();
    break;
  case menuNowPlayingMenuState:
    topMenu();
    break;
  case TVPowerNowPlayingMenuState:
    speakerGroup -> tv -> tvRemoteCommand("power");
    break;
  case backNowPlayingMenuState:
    speakerGroup -> tv -> tvRemoteCommand("back");
    break;
  case homeNowPlayingMenuState:
    speakerGroup -> tv -> tvRemoteCommand("menu");
    break;
  }
  displayUpdate.updateDisplay(true);
}

void buttonPressSelect() {
  resetMarquee();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
  case nowPlayingMenu:
    if (optionMenu == tvOptionMenu) {
      optionMenu = noOptionMenu;
      speakerGroup -> tv -> tvRemoteCommand("power");
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
  } else if(activeMenuState == nowPlayingMenu) {
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

#endif