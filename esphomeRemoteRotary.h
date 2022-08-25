#include "esphomeRemote.h"

#ifndef REMOTEROTARY
#define REMOTEROTARY

void debounceUpdateDisplay() {
  if (id(display_update_tick).state != id(rotary).state) {
    id(display_update_tick).publish_state(id(rotary).state);
  }
}

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

void buttonPressNext() {
  switch (activeMenuState) {
  case tvNowPlayingMenu:
  case speakerNowPlayingMenu:
    speakerGroup -> increaseSpeakerVolume();
    optionMenu = volumeOptionMenu;
    debounceUpdateDisplay();
    return;
  default:
    break;
  }
  if (menuIndex < activeMenuTitleCount - 1) {
    menuIndex++;
  } else {
    menuIndex = 0;
  }
  debounceUpdateDisplay();
}

void buttonPressPrevious() {
  switch (activeMenuState) {
  case tvNowPlayingMenu:
  case speakerNowPlayingMenu:
    speakerGroup -> decreaseSpeakerVolume();
    optionMenu = volumeOptionMenu;
    debounceUpdateDisplay();
    return;
  default:
    break;
  }
  if (menuIndex > 0) {
    menuIndex--;
  }
  debounceUpdateDisplay();
}

void buttonPressUp() {
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
  case tvNowPlayingMenu:
    if (optionMenu == tvOptionMenu) {
      optionMenu = noOptionMenu;
      topMenu();
      displayUpdate.updateDisplay(true);
    } else {
      speakerGroup -> tv -> tvRemoteCommand("up");
    }
    break;
  default:
    if (optionMenu == speakerOptionMenu) {
      speakerGroup -> toggleShuffle();
      optionMenu = noOptionMenu;
      displayUpdate.updateDisplay(true);
      return;
    } else if(speakerGroup->newSpeakerGroupParent != NULL) {
      speakerGroup->newSpeakerGroupParent = NULL;
      displayUpdate.updateDisplay(true);
      return;
    }
    topMenu();
    displayUpdate.updateDisplay(true);
    break;
  }
}

void buttonPressDown() {
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
  case tvNowPlayingMenu:
    if (optionMenu == tvOptionMenu) {
      optionMenu = noOptionMenu;
      speakerGroup -> tv -> tvRemoteCommand("play");
      displayUpdate.updateDisplay(true);
    } else {
      speakerGroup -> tv -> tvRemoteCommand("down");
    }
    break;
  case speakerNowPlayingMenu:
    if (optionMenu == speakerOptionMenu) {
      optionMenu = noOptionMenu;
      activeMenuState = groupMenu;
      displayUpdate.updateDisplay(true);
      return;
    }
    speakerGroup -> activePlayer -> playPause();
    return;
  default:
    break;
  }
}

void buttonPressLeft() {
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
  case tvNowPlayingMenu:
    if (optionMenu == tvOptionMenu) {
      optionMenu = noOptionMenu;
      speakerGroup -> tv -> tvRemoteCommand("back");
      displayUpdate.updateDisplay(true);
    } else {
      speakerGroup -> tv -> tvRemoteCommand("left");
    }
    break;
  case speakerNowPlayingMenu:
    // speakerGroup->activePlayer->back();
    return;
  default:
    break;
  }
}

void buttonPressRight() {
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
  case tvNowPlayingMenu:
    if (optionMenu == tvOptionMenu) {
      optionMenu = noOptionMenu;
      speakerGroup -> tv -> tvRemoteCommand("menu");
      displayUpdate.updateDisplay(true);
    } else {
      speakerGroup -> tv -> tvRemoteCommand("right");
    }
    break;
  case speakerNowPlayingMenu:
    if (optionMenu == speakerOptionMenu) {
      speakerGroup -> toggleMute();
      optionMenu = noOptionMenu;
      displayUpdate.updateDisplay(true);
      return;
    } else {
      speakerGroup -> activePlayer -> nextTrack();
    }
    return;
  default:
    break;
  }
}

void buttonReleaseScreenLeft() {
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
  case tvNowPlayingMenu:
    displayUpdate.updateDisplay(true);
    break;
  default:
    break;
  }
}

void buttonPressScreenLeft() {
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
  case tvNowPlayingMenu:
    if (optionMenu == tvOptionMenu) {
      optionMenu = noOptionMenu;
    } else {
      optionMenu = tvOptionMenu;
    }
    displayUpdate.updateDisplay(true);
    break;
  case speakerNowPlayingMenu:
    if (optionMenu == speakerOptionMenu) {
      optionMenu = noOptionMenu;
    } else {
      optionMenu = speakerOptionMenu;
    }
    displayUpdate.updateDisplay(true);
    break;
  default:
    break;
  }
}

void buttonPressScreenRight() {
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  optionMenu = noOptionMenu;
  switch (activeMenuState) {
  case tvNowPlayingMenu:
    topMenu();
    displayUpdate.updateDisplay(true);
    break;
  default:
    topMenu();
    displayUpdate.updateDisplay(true);
    break;
  }
}

#endif