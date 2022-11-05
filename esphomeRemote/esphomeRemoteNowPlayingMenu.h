#include "esphomeRemote.h"

#pragma once

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
  case groupNowPlayingMenuState:
    menuIndex = 0;
    activeMenuState = groupMenu;
    break;
  case TVPowerNowPlayingMenuState:
    speakerGroup->sendActivePlayerRemoteCommand("power");
    break;
  case backNowPlayingMenuState:
    speakerGroup->sendActivePlayerRemoteCommand("back");
    break;
  case homeNowPlayingMenuState:
    speakerGroup->sendActivePlayerRemoteCommand("menu");
    break;
  }
  displayUpdate.updateDisplay(true);
}
