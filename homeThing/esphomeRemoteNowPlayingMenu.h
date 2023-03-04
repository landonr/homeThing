#include "esphomeRemote.h"

#pragma once

void selectNowPlayingMenu() {
  if (activeMenuTitleCount <= 0 && menuIndex < activeMenuTitleCount) {
    return;
  }
  auto menuTitle = getNowPlayingMenuStates()[menuIndex];
  switch (menuTitle) {
    case pauseNowPlayingMenuState:
      speaker_group_->activePlayer->playPause();
      break;
    case volumeUpNowPlayingMenuState:
      speaker_group_->increaseSpeakerVolume();
      optionMenu = volumeOptionMenu;
      break;
    case volumeDownNowPlayingMenuState:
      speaker_group_->decreaseSpeakerVolume();
      optionMenu = volumeOptionMenu;
      break;
    case nextNowPlayingMenuState:
      speaker_group_->activePlayer->nextTrack();
      break;
    case shuffleNowPlayingMenuState:
      speaker_group_->toggleShuffle();
      break;
    case menuNowPlayingMenuState:
      topMenu();
      break;
    case groupNowPlayingMenuState:
      menuIndex = 0;
      activeMenuState = groupMenu;
      break;
    case TVPowerNowPlayingMenuState:
      speaker_group_->sendActivePlayerRemoteCommand("power");
      break;
    case backNowPlayingMenuState:
      speaker_group_->sendActivePlayerRemoteCommand("back");
      break;
    case homeNowPlayingMenuState:
      speaker_group_->sendActivePlayerRemoteCommand("menu");
      break;
  }
  displayUpdate.updateDisplay(true);
}
