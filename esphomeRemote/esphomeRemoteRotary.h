#include "esphomeRemote.h"

#pragma once

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
        speakerGroup->sendActivePlayerRemoteCommand("power");
        displayUpdate.updateDisplay(true);
        return;
      }

      switch (speakerGroup->activePlayer->playerType) {
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

void buttonPressSelectHold() {
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  if (selectMenuHold()) {
    displayUpdate.updateDisplay(true);
  }
}

void buttonPressNext() {
  resetMarquee();
  switch (activeMenuState) {
    case nowPlayingMenu:
      speakerGroup->increaseSpeakerVolume();
      optionMenu = volumeOptionMenu;
      debounceUpdateDisplay();
      return;
    case lightsDetailMenu:
      if (lightGroup->lightDetailSelected && menuIndex == 0 && lightGroup->getActiveLight() != NULL) {
        lightGroup->getActiveLight()->incBrightness();
        return;
      } else if (lightGroup->lightDetailSelected && menuIndex == 1 && lightGroup->getActiveLight() != NULL) {
        lightGroup->getActiveLight()->incTemperature();
        return;
      }
    default:
      break;
  }
  if (menuIndex < activeMenuTitleCount - 1) {
    menuIndex++;
  } else if (id(menu_rollover_on) && menuIndex == activeMenuTitleCount - 1) {
    menuIndex = 0;
  }
  debounceUpdateDisplay();
}

void buttonPressPrevious() {
  resetMarquee();
  switch (activeMenuState) {
    case nowPlayingMenu:
      speakerGroup->decreaseSpeakerVolume();
      optionMenu = volumeOptionMenu;
      debounceUpdateDisplay();
      return;
    case lightsDetailMenu:
      if (lightGroup->lightDetailSelected && menuIndex == 0 && lightGroup->getActiveLight() != NULL) {
        lightGroup->getActiveLight()->decBrightness();
        return;
      } else if (lightGroup->lightDetailSelected && menuIndex == 1 && lightGroup->getActiveLight() != NULL) {
        lightGroup->getActiveLight()->decTemperature();
        return;
      }
    default:
      break;
  }
  if (menuIndex > 0) {
    menuIndex--;
  } else if (id(menu_rollover_on) && menuIndex == 0) {
    menuIndex = activeMenuTitleCount - 1;
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

      switch (speakerGroup->activePlayer->playerType) {
        case TVRemotePlayerType:
          speakerGroup->sendActivePlayerRemoteCommand("up");
          return;
        case SpeakerRemotePlayerType:
          break;
      }
      break;
    case groupMenu:
      menuIndex = 0;
      if (speakerGroup->newSpeakerGroupParent != NULL) {
        speakerGroup->newSpeakerGroupParent = NULL;
      } else {
        activeMenuState = nowPlayingMenu;
        optionMenu = noOptionMenu;
      }
      displayUpdate.updateDisplay(true);
      return;
    case lightsDetailMenu:
      if (lightGroup->lightDetailSelected) {
        // deselect ligh if selected and stay in lightsDetailMenu
        lightGroup->lightDetailSelected = false;
        displayUpdate.updateDisplay(true);
        return;
      } else {
        // if no light is selected go back to lightsMenu
        activeMenuState = lightsMenu;
        displayUpdate.updateDisplay(true);
        return;
      }
      break;
    default:
      break;
  }
  if (optionMenu == speakerOptionMenu) {
    speakerGroup->toggleShuffle();
    optionMenu = noOptionMenu;
    displayUpdate.updateDisplay(true);
    return;
  }
  optionMenu = noOptionMenu;
  lightGroup->clearActiveLight();
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

      switch (speakerGroup->activePlayer->playerType) {
        case TVRemotePlayerType:
          speakerGroup->sendActivePlayerRemoteCommand("down");
          break;
        case SpeakerRemotePlayerType:
          if (optionMenu == speakerOptionMenu) {
            activeMenuState = groupMenu;
            displayUpdate.updateDisplay(true);
          } else {
            speakerGroup->activePlayer->playPause();
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

      switch (speakerGroup->activePlayer->playerType) {
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
      switch (speakerGroup->activePlayer->playerType) {
        case TVRemotePlayerType:
          speakerGroup->sendActivePlayerRemoteCommand("right");
          break;
        case SpeakerRemotePlayerType:
          if (optionMenu == speakerOptionMenu) {
            speakerGroup->toggleMute();
            displayUpdate.updateDisplay(true);
          } else {
            speakerGroup->activePlayer->nextTrack();
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
      switch (speakerGroup->activePlayer->playerType) {
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
      switch (speakerGroup->activePlayer->playerType) {
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
    case lightsDetailMenu:
    case sensorsMenu:
    case bootMenu:
    case switchesMenu:
      break;
  }
}

void displayUpdateDebounced() {
  if (idleTime < 2) {
    displayUpdate.updateDisplay(true);
  }
}
