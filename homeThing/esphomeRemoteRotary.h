#include "esphomeRemote.h"

#pragma once

void debounceUpdateDisplay() {
  if (id(display_update_tick).state != id(rotary).state) {
    id(display_update_tick).publish_state(id(rotary).state);
  }
}

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
        speaker_group_->sendActivePlayerRemoteCommand("power");
        displayUpdate.updateDisplay(true);
        return;
      }

      switch (speaker_group_->activePlayer->get_player_type()) {
        case TVRemotePlayerType:
          speaker_group_->sendActivePlayerRemoteCommand("select");
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

void rotaryScrollClockwise() {
  resetAnimation();
  switch (activeMenuState) {
    case nowPlayingMenu:
      speaker_group_->increaseSpeakerVolume();
      optionMenu = volumeOptionMenu;
      debounceUpdateDisplay();
      return;
    case lightsDetailMenu:
      if (light_group_->lightDetailSelected && menuIndex == 0 &&
          light_group_->getActiveLight() != NULL) {
        light_group_->getActiveLight()->incBrightness();
        debounceUpdateDisplay();
        return;
      } else if (light_group_->lightDetailSelected && menuIndex == 1 &&
                 light_group_->getActiveLight() != NULL) {
        light_group_->getActiveLight()->incTemperature();
        debounceUpdateDisplay();
        return;
      } else if (light_group_->lightDetailSelected && menuIndex == 2 &&
                 light_group_->getActiveLight() != NULL) {
        light_group_->getActiveLight()->incColor();
        debounceUpdateDisplay();
        return;
      }
    default:
      break;
  }
  if (menuIndex < activeMenuTitleCount - 1) {
    menuIndex++;
  } else if (id(display_state_->menu_rollover_on_) &&
             menuIndex == activeMenuTitleCount - 1) {
    menuIndex = 0;
  }
  debounceUpdateDisplay();
}

void rotaryScrollCounterClockwise() {
  resetAnimation();
  switch (activeMenuState) {
    case nowPlayingMenu:
      speaker_group_->decreaseSpeakerVolume();
      optionMenu = volumeOptionMenu;
      debounceUpdateDisplay();
      return;
    case lightsDetailMenu:
      if (light_group_->lightDetailSelected && menuIndex == 0 &&
          light_group_->getActiveLight() != NULL) {
        light_group_->getActiveLight()->decBrightness();
        debounceUpdateDisplay();
        return;
      } else if (light_group_->lightDetailSelected && menuIndex == 1 &&
                 light_group_->getActiveLight() != NULL) {
        light_group_->getActiveLight()->decTemperature();
        debounceUpdateDisplay();
        return;
      } else if (light_group_->lightDetailSelected && menuIndex == 2 &&
                 light_group_->getActiveLight() != NULL) {
        light_group_->getActiveLight()->decColor();
        debounceUpdateDisplay();
        return;
      }
    default:
      break;
  }
  if (menuIndex > 0) {
    menuIndex--;
  } else if (id(display_state_->menu_rollover_on_) && menuIndex == 0) {
    menuIndex = activeMenuTitleCount - 1;
  }
  debounceUpdateDisplay();
}

void buttonPressUp() {
  resetAnimation();
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

      switch (speaker_group_->activePlayer->get_player_type()) {
        case TVRemotePlayerType:
          speaker_group_->sendActivePlayerRemoteCommand("up");
          return;
        case SpeakerRemotePlayerType:
          break;
      }
      break;
    case groupMenu:
      menuIndex = 0;
      if (speaker_group_->newSpeakerGroupParent != NULL) {
        speaker_group_->newSpeakerGroupParent = NULL;
      } else {
        activeMenuState = nowPlayingMenu;
        optionMenu = noOptionMenu;
      }
      displayUpdate.updateDisplay(true);
      return;
    case lightsDetailMenu:
      if (light_group_->lightDetailSelected) {
        // deselect ligh if selected and stay in lightsDetailMenu
        light_group_->lightDetailSelected = false;
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
    speaker_group_->toggleShuffle();
    optionMenu = noOptionMenu;
    displayUpdate.updateDisplay(true);
    return;
  }
  optionMenu = noOptionMenu;
  light_group_->clearActiveLight();
  topMenu();
  displayUpdate.updateDisplay(true);
}

void buttonPressDown() {
  resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
    case nowPlayingMenu:
      if (optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
        speaker_group_->sendActivePlayerRemoteCommand("play");
        displayUpdate.updateDisplay(true);
        return;
      }

      switch (speaker_group_->activePlayer->get_player_type()) {
        case TVRemotePlayerType:
          speaker_group_->sendActivePlayerRemoteCommand("down");
          break;
        case SpeakerRemotePlayerType:
          if (optionMenu == speakerOptionMenu) {
            activeMenuState = groupMenu;
            displayUpdate.updateDisplay(true);
          } else {
            speaker_group_->activePlayer->playPause();
            optionMenu = noOptionMenu;
          }
          break;
      }
    default:
      break;
  }
}

void buttonPressLeft() {
  resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
    case nowPlayingMenu:
      if (optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
        speaker_group_->sendActivePlayerRemoteCommand("back");
        displayUpdate.updateDisplay(true);
        return;
      }

      switch (speaker_group_->activePlayer->get_player_type()) {
        case TVRemotePlayerType:
          speaker_group_->sendActivePlayerRemoteCommand("left");
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
  resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
    case bootMenu:
      skipBootSequence();
      break;
    case nowPlayingMenu:
      if (optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
        speaker_group_->sendActivePlayerRemoteCommand("menu");
        displayUpdate.updateDisplay(true);
        return;
      }
      switch (speaker_group_->activePlayer->get_player_type()) {
        case TVRemotePlayerType:
          speaker_group_->sendActivePlayerRemoteCommand("right");
          break;
        case SpeakerRemotePlayerType:
          if (optionMenu == speakerOptionMenu) {
            speaker_group_->toggleMute();
            displayUpdate.updateDisplay(true);
          } else {
            speaker_group_->activePlayer->nextTrack();
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
  resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
    case nowPlayingMenu:
      switch (speaker_group_->activePlayer->get_player_type()) {
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
  resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
    case nowPlayingMenu:
      switch (speaker_group_->activePlayer->get_player_type()) {
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
  resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  optionMenu = noOptionMenu;
  switch (activeMenuState) {
    case rootMenu:
    case backlightMenu:
    case sleepMenu:
    case nowPlayingMenu:
      speaker_group_->selectNextMediaPlayer();
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
