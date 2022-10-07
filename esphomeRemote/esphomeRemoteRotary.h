#include "esphomeRemote.h"

#ifndef REMOTEROTARY
#define REMOTEROTARY

void debounceUpdateDisplay() {
  if (id(display_update_tick).state != id(rotary).state) {
    id(display_update_tick).publish_state(id(rotary).state);
  }
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
      speakerGroup->sendActivePlayerRemoteCommand("power");
      displayUpdate.updateDisplay(true);
      return;
    }

    switch (speakerGroup -> activePlayer -> playerType) {
    case TVRemotePlayerType:
      speakerGroup->sendActivePlayerRemoteCommand("select");
      break;
    case SpeakerRemotePlayerType:
      break;
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
  resetMarquee();
  switch (activeMenuState) {
  case nowPlayingMenu:
    speakerGroup -> increaseSpeakerVolume();
    optionMenu = volumeOptionMenu;
    debounceUpdateDisplay();
    return;
  default:
    break;
  }
  if (menuIndex < activeMenuTitleCount - 1) {
    menuIndex++;
  }
  debounceUpdateDisplay();
}

void buttonPressPrevious() {
  resetMarquee();
  switch (activeMenuState) {
  case nowPlayingMenu:
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
  resetMarquee();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
  case nowPlayingMenu:
    if (optionMenu == tvOptionMenu) {
      optionMenu = noOptionMenu;
      topMenu();
      displayUpdate.updateDisplay(true);
      return;
    }

    switch (speakerGroup -> activePlayer -> playerType) {
    case TVRemotePlayerType:
      speakerGroup->sendActivePlayerRemoteCommand("up");
      return;
    case SpeakerRemotePlayerType:
      break;
    }
    break;
  case groupMenu:
    menuIndex = 0;
    if(speakerGroup->newSpeakerGroupParent != NULL) {
      speakerGroup->newSpeakerGroupParent = NULL;
    } else {
      activeMenuState = nowPlayingMenu;
      optionMenu = noOptionMenu;
    }
    displayUpdate.updateDisplay(true);
    return;
  default:
    break;
  }
  if (optionMenu == speakerOptionMenu) {
    speakerGroup -> toggleShuffle();
    optionMenu = noOptionMenu;
    displayUpdate.updateDisplay(true);
    return;
  }
  optionMenu = noOptionMenu;
  topMenu();
  displayUpdate.updateDisplay(true);
}

void buttonPressDown() {
  resetMarquee();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
  case nowPlayingMenu:
    if (optionMenu == tvOptionMenu) {
      optionMenu = noOptionMenu;
      speakerGroup->sendActivePlayerRemoteCommand("play");
      displayUpdate.updateDisplay(true);
      return;
    }

    switch (speakerGroup -> activePlayer -> playerType) {
    case TVRemotePlayerType:
      speakerGroup->sendActivePlayerRemoteCommand("down");
      break;
    case SpeakerRemotePlayerType:
      if (optionMenu == speakerOptionMenu) {
        activeMenuState = groupMenu;
        displayUpdate.updateDisplay(true);
      } else {
        speakerGroup -> activePlayer -> playPause();
        optionMenu = noOptionMenu;
      }
      break;
    }
  default:
    break;
  }
}

void buttonPressLeft() {
  resetMarquee();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
  case nowPlayingMenu:
    if (optionMenu == tvOptionMenu) {
      optionMenu = noOptionMenu;
      speakerGroup->sendActivePlayerRemoteCommand("back");
      displayUpdate.updateDisplay(true);
      return;
    }

    switch (speakerGroup -> activePlayer -> playerType) {
    case TVRemotePlayerType:
      speakerGroup->sendActivePlayerRemoteCommand("left");
      break;
    case SpeakerRemotePlayerType:
      optionMenu = noOptionMenu;
      break;
    }
    break;
  default:
    break;
  }
}

void buttonPressRight() {
  resetMarquee();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
  case nowPlayingMenu:
    if (optionMenu == tvOptionMenu) {
      optionMenu = noOptionMenu;
      speakerGroup->sendActivePlayerRemoteCommand("menu");
      displayUpdate.updateDisplay(true);
      return;
    }
    switch (speakerGroup -> activePlayer -> playerType) {
    case TVRemotePlayerType:
      speakerGroup->sendActivePlayerRemoteCommand("right");
      break;
    case SpeakerRemotePlayerType:
      if (optionMenu == speakerOptionMenu) {
        speakerGroup -> toggleMute();
        displayUpdate.updateDisplay(true);
      } else {
        speakerGroup -> activePlayer -> nextTrack();
      }
      optionMenu = noOptionMenu;
      break;
    }
    break;
  default:
    break;
  }
}

void buttonReleaseScreenLeft() {
  resetMarquee();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
  case nowPlayingMenu:
    switch (speakerGroup -> activePlayer -> playerType) {
    case TVRemotePlayerType:
      displayUpdate.updateDisplay(true);
      break;
    case SpeakerRemotePlayerType:
      break;
    }
    break;
  default:
    break;
  }
}

void buttonPressScreenLeft() {
  resetMarquee();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
  case nowPlayingMenu:
    switch (speakerGroup -> activePlayer -> playerType) {
    case TVRemotePlayerType:
      if (optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
      } else {
        optionMenu = tvOptionMenu;
      }
      displayUpdate.updateDisplay(true);
      break;
    case SpeakerRemotePlayerType:
      if (optionMenu == speakerOptionMenu) {
        optionMenu = noOptionMenu;
      } else {
        optionMenu = speakerOptionMenu;
      }
      displayUpdate.updateDisplay(true);
      break;
    }
    break;
  default:
    break;
  }
}

void buttonPressScreenRight() {
  resetMarquee();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  optionMenu = noOptionMenu;
  switch (activeMenuState) {
  case rootMenu:
  case backlightMenu:
  case sleepMenu:
  case nowPlayingMenu:
    speakerGroup->selectNextMediaPlayer();
    displayUpdate.updateDisplay(true);
    break;
  case sourcesMenu:
  case groupMenu:
  case mediaPlayersMenu:
  case scenesMenu:
  case lightsMenu:
  case sensorsMenu:
  case bootMenu:
    break;
  }
}

void displayUpdateDebounced() {
  if(idleTime < 2) {
    displayUpdate.updateDisplay(true);
  }
}

#endif