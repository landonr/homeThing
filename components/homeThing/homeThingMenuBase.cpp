#include "homeThingMenuBase.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_menu_base {

void HomeThingMenuBase::setup() {}

void HomeThingMenuBase::drawMenu() {
  auto active_menu = activeMenu();
  activeMenuTitleCount = active_menu.size();
  menu_display_->drawMenu(&activeMenuState, active_menu, menuIndex);
}
void HomeThingMenuBase::topMenu() {
  if (activeMenuState == bootMenu) {
    return;
  }
  animation_->resetAnimation();
  menuIndex = 0;
  if (speakerGroup != NULL)
    speakerGroup->newSpeakerGroupParent = NULL;
  optionMenu = noOptionMenu;
  activeMenuState = rootMenu;
}

bool HomeThingMenuBase::selectMenu() {
  int menuIndexForSource = menuIndex;
  switch (activeMenuState) {
    case rootMenu:
      return selectRootMenu();
    case nowPlayingMenu:
      activeMenuState = nowPlayingMenu;
      break;
    case sourcesMenu: {
      auto sourceTitleState =
          std::static_pointer_cast<MenuTitleSource>(activeMenuTitle);
      idleMenu(true);
      speakerGroup->playSource(*sourceTitleState->media_source_);
      optionMenu = playingNewSourceMenu;
      update();
      break;
    }
    case groupMenu: {
      auto playerTitleState =
          std::static_pointer_cast<MenuTitlePlayer>(activeMenuTitle);
      speakerGroup->selectGroup(playerTitleState->media_player_, menuIndex);
      break;
    }
    case lightsMenu: {
      lightGroup->toggleLight(menuIndexForSource);
      return true;
    }
    case lightsDetailMenu:
      lightGroup->lightDetailSelected = true;
    case mediaPlayersMenu:
      selectMediaPlayers();
      break;
    case scenesMenu:
      if (serviceGroup->select_service(menuIndexForSource)) {
        topMenu();
      }
      break;
    case switchesMenu:
      if (switchGroup->selectSwitch(menuIndexForSource)) {
        topMenu();
      }
      break;
    default:
      ESP_LOGW("WARNING", "menu state is bad but its an enum");
      return false;
  }
  return true;
}

bool HomeThingMenuBase::selectMenuHold() {
  int menuIndexForSource = menuIndex;
  switch (activeMenuState) {
    case lightsMenu: {
      auto selectedLight = lightGroup->lights[menuIndexForSource];
      if (selectedLight == NULL) {
        break;
      }
      if (lightGroup->selectLightDetailAtIndex(menuIndexForSource)) {
        menuIndex = 0;
        activeMenuState = lightsDetailMenu;
      }
      return true;
    }
    default:
      break;
  }
  return false;
}

bool HomeThingMenuBase::selectRootMenu() {
  MenuStates currentMenu = rootMenuTitles(
      speakerGroup != NULL, serviceGroup != NULL, true, true, true)[menuIndex];
  switch (currentMenu) {
    case sourcesMenu:
      activeMenuState = sourcesMenu;
      break;
    case nowPlayingMenu:
      activeMenuState = nowPlayingMenu;
      break;
    case mediaPlayersMenu:
      activeMenuState = mediaPlayersMenu;
      break;
    case lightsMenu:
      activeMenuState = lightsMenu;
      break;
    case switchesMenu:
      activeMenuState = switchesMenu;
      break;
    case scenesMenu:
      activeMenuState = scenesMenu;
      break;
    case backlightMenu:
      topMenu();
      backlight_->turn_off();
      return false;
    case sleepMenu:
      sleep_toggle_->turn_on();
      return false;
    case sensorsMenu:
      activeMenuState = sensorsMenu;
      break;
    case lightsDetailMenu:
    case groupMenu:
    case rootMenu:
    case bootMenu:
      ESP_LOGD("WARNING", "selecting menu is bad %d %s", menuIndex,
               menuTitleForType(activeMenuState)->get_name().c_str());
      return false;
  }
  menuIndex = 0;
  return true;
}

std::shared_ptr<MenuTitleBase> HomeThingMenuBase::menuTitleForType(
    MenuStates stringType) {
  return std::make_shared<MenuTitleBase>(menu_state_title(stringType), "",
                                         menu_state_right_icon(stringType));
}

std::vector<std::shared_ptr<MenuTitleBase>>
HomeThingMenuBase::menuTypesToTitles(std::vector<MenuStates> menu) {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  for (auto& menuItem : menu) {
    out.push_back(menuTitleForType(menuItem));
  }
  return out;
}

std::vector<std::shared_ptr<MenuTitleBase>> HomeThingMenuBase::activeMenu() {
  int x = menuIndex;
  switch (activeMenuState) {
    case rootMenu:
      return menuTypesToTitles(rootMenuTitles(
          speakerGroup != NULL, serviceGroup != NULL, true, true, true));
    case sourcesMenu: {
      auto sources = speakerGroup->activePlayerSources();
      auto sourceTitles = activePlayerSourceTitles(sources);
      return {sourceTitles.begin(), sourceTitles.end()};
    }
    case mediaPlayersMenu: {
      auto mediaPlayersTitles =
          mediaPlayersTitleString(speakerGroup->media_players_);
      return {mediaPlayersTitles.begin(), mediaPlayersTitles.end()};
    }
    case scenesMenu:
      return sceneTitleStrings(serviceGroup->services);
    case sensorsMenu:
      return sensorTitles(sensorGroup->sensors);
    case nowPlayingMenu:
      return {};
    default:
      ESP_LOGW("WARNING", "menu is bad  %d, %s", x,
               menuTitleForType(activeMenuState)->get_name().c_str());
      return {};
  }
}

void HomeThingMenuBase::buttonPressSelect() {
  animation_->resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
    case lightsDetailMenu:
      if (lightGroup->lightDetailSelected) {
        // deselect light if selected and stay in lightsDetailMenu
        lightGroup->lightDetailSelected = false;
        update();
        return;
      }
      break;
    case nowPlayingMenu:
      if (optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
        speakerGroup->sendActivePlayerRemoteCommand("power");
        update();
        return;
      }

      switch (speakerGroup->activePlayer->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          speakerGroup->sendActivePlayerRemoteCommand("select");
          break;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
          break;
      }
      return;
    default:
      break;
  }
  if (selectMenu()) {
    update();
  }
}

bool HomeThingMenuBase::buttonPressWakeUpDisplay() {
  if (idleTime != -1) {
    idleTime = 0;
  }
  if (speakerGroup != NULL && !speakerGroup->playerSearchFinished) {
    speakerGroup->findActivePlayer();
  }
  if (backlight_ != NULL && !backlight_->state) {
    backlight_->turn_on();
    menu_display_->updateDisplay(false);
    return true;
  }
  return false;
}

void HomeThingMenuBase::buttonPressSelectHold() {
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  if (selectMenuHold()) {
    update();
  }
}

void HomeThingMenuBase::rotaryScrollClockwise(int rotary) {
  rotary_ = rotary;
  animation_->resetAnimation();
  switch (activeMenuState) {
    case nowPlayingMenu:
      speakerGroup->increaseSpeakerVolume();
      optionMenu = volumeOptionMenu;
      update();
      return;
    case lightsDetailMenu:
      if (lightGroup->lightDetailSelected && menuIndex == 0 &&
          lightGroup->getActiveLight() != NULL) {
        lightGroup->getActiveLight()->incBrightness();
        update();
        return;
      } else if (lightGroup->lightDetailSelected && menuIndex == 1 &&
                 lightGroup->getActiveLight() != NULL) {
        lightGroup->getActiveLight()->incTemperature();
        update();
        return;
      } else if (lightGroup->lightDetailSelected && menuIndex == 2 &&
                 lightGroup->getActiveLight() != NULL) {
        lightGroup->getActiveLight()->incColor();
        update();
        return;
      }
    default:
      break;
  }
  if (menuIndex < activeMenuTitleCount - 1) {
    menuIndex++;
  } else if (menu_rollover_on_ && menuIndex == activeMenuTitleCount - 1) {
    menuIndex = 0;
  }
  update();
}

void HomeThingMenuBase::rotaryScrollCounterClockwise(int rotary) {
  rotary_ = rotary;
  animation_->resetAnimation();
  switch (activeMenuState) {
    case nowPlayingMenu:
      speakerGroup->decreaseSpeakerVolume();
      optionMenu = volumeOptionMenu;
      update();
      return;
    case lightsDetailMenu:
      if (lightGroup->lightDetailSelected && menuIndex == 0 &&
          lightGroup->getActiveLight() != NULL) {
        lightGroup->getActiveLight()->decBrightness();
        update();
        return;
      } else if (lightGroup->lightDetailSelected && menuIndex == 1 &&
                 lightGroup->getActiveLight() != NULL) {
        lightGroup->getActiveLight()->decTemperature();
        update();
        return;
      } else if (lightGroup->lightDetailSelected && menuIndex == 2 &&
                 lightGroup->getActiveLight() != NULL) {
        lightGroup->getActiveLight()->decColor();
        update();
        return;
      }
    default:
      break;
  }
  if (menuIndex > 0) {
    menuIndex--;
  } else if (menu_rollover_on_ && menuIndex == 0) {
    menuIndex = activeMenuTitleCount - 1;
  }
  update();
}

void HomeThingMenuBase::buttonPressUp() {
  animation_->resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
    case nowPlayingMenu:
      if (optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
        topMenu();
        update();
        return;
      }

      switch (speakerGroup->activePlayer->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          speakerGroup->sendActivePlayerRemoteCommand("up");
          return;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
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
      update();
      return;
    case lightsDetailMenu:
      if (lightGroup->lightDetailSelected) {
        // deselect light if selected and stay in lightsDetailMenu
        lightGroup->lightDetailSelected = false;
        update();
        return;
      } else {
        // if no light is selected go back to lightsMenu
        activeMenuState = lightsMenu;
        update();
        return;
      }
      break;
    default:
      break;
  }
  if (optionMenu == speakerOptionMenu) {
    speakerGroup->toggleShuffle();
    optionMenu = noOptionMenu;
    update();
    return;
  }
  optionMenu = noOptionMenu;
  lightGroup->clearActiveLight();
  topMenu();
  update();
}

void HomeThingMenuBase::buttonPressDown() {
  animation_->resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
    case nowPlayingMenu:
      if (optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
        speakerGroup->sendActivePlayerRemoteCommand("play");
        update();
        return;
      }

      switch (speakerGroup->activePlayer->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          speakerGroup->sendActivePlayerRemoteCommand("down");
          break;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
          if (optionMenu == speakerOptionMenu) {
            activeMenuState = groupMenu;
            update();
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

void HomeThingMenuBase::buttonPressLeft() {
  animation_->resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
    case nowPlayingMenu:
      if (optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
        speakerGroup->sendActivePlayerRemoteCommand("back");
        update();
        return;
      }

      switch (speakerGroup->activePlayer->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          speakerGroup->sendActivePlayerRemoteCommand("left");
          break;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
          optionMenu = noOptionMenu;
          break;
      }
      break;
    default:
      break;
  }
}

void HomeThingMenuBase::buttonPressRight() {
  animation_->resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
    case bootMenu:
      menu_display_->skipBootSequence(activeMenuState);
      break;
    case nowPlayingMenu:
      if (optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
        speakerGroup->sendActivePlayerRemoteCommand("menu");
        update();
        return;
      }
      switch (speakerGroup->activePlayer->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          speakerGroup->sendActivePlayerRemoteCommand("right");
          break;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
          if (optionMenu == speakerOptionMenu) {
            speakerGroup->toggleMute();
            update();
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

void HomeThingMenuBase::buttonReleaseScreenLeft() {
  animation_->resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
    case nowPlayingMenu:
      switch (speakerGroup->activePlayer->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          update();
          break;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
          break;
      }
      break;
    default:
      break;
  }
}

void HomeThingMenuBase::buttonPressScreenLeft() {
  animation_->resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
    case nowPlayingMenu:
      switch (speakerGroup->activePlayer->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          if (optionMenu == tvOptionMenu) {
            optionMenu = noOptionMenu;
          } else {
            optionMenu = tvOptionMenu;
          }
          update();
          break;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
          if (optionMenu == speakerOptionMenu) {
            optionMenu = noOptionMenu;
          } else {
            optionMenu = speakerOptionMenu;
          }
          update();
          break;
      }
      break;
    default:
      break;
  }
}

void HomeThingMenuBase::buttonPressScreenRight() {
  animation_->resetAnimation();
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
      update();
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

void HomeThingMenuBase::displayUpdateDebounced() {
  if (idleTime < 2) {
    update();
  }
}

void HomeThingMenuBase::debounceUpdateDisplay() {
  if (display_update_tick_ != rotary_) {
    display_update_tick_ = rotary_;
  }
}

void HomeThingMenuBase::idleTick() {
  if (activeMenuState == bootMenu) {
    if (idleTime == display_timeout_ && !charging) {
      ESP_LOGD("idle", "turning off display");
      // backlight_->turn_off();
    }
    idleTime++;
    return;
  }
  if (idleTime == 3) {
    optionMenu = noOptionMenu;
    update();
  } else if (idleTime == display_timeout_) {
    if (speakerGroup != NULL && speakerGroup->playerSearchFinished) {
      if (charging && activeMenuState != bootMenu) {
        idleTime++;
        return;
      }
      activeMenuState = rootMenu;
      animation_->resetAnimation();
      idleMenu(false);
      menu_display_->updateDisplay(false);
    }
    if (!charging) {
      ESP_LOGD("idle", "turning off display");
      backlight_->turn_off();
    }
    idleTime++;
    return;
  } else if (idleTime == 180 && charging) {
    idleMenu(true);
    idleTime++;
    return;
  } else if (idleTime > sleep_after_) {
    if (!charging) {
      ESP_LOGI("idle", "night night");
      // sleep_toggle_->turn_on();
      return;
    }
  }
  if (speakerGroup != NULL) {
    bool updatedMediaPositions = speakerGroup->updateMediaPosition();
    if (updatedMediaPositions) {
      switch (activeMenuState) {
        case nowPlayingMenu:
          if (idleTime < 16 || charging) {
            update();
          }
          break;
        default:
          break;
      }
    }
  }
  idleTime++;
}

void HomeThingMenuBase::selectMediaPlayers() {
  for (auto& media_player : speakerGroup->media_players_) {
    if (media_player->entity_id_ == activeMenuTitle->entity_id_) {
      speakerGroup->activePlayer = media_player;
      topMenu();
      return;
    }
  }
}

void HomeThingMenuBase::goToScreenFromString(std::string screenName) {
  if (screenName == "nowPlaying") {
    activeMenuState = nowPlayingMenu;
  } else if (screenName == "sensors") {
    activeMenuState = sensorsMenu;
  }
  menuIndex = 0;
  // displayUpdate.updateDisplay(true);
}

void HomeThingMenuBase::idleMenu(bool force) {
  if (activeMenuState == bootMenu) {
    return;
  }
  if (!charging || force) {
    lightGroup->clearActiveLight();
    menuIndex = 0;
    animation_->resetAnimation();
    optionMenu = noOptionMenu;
    activeMenuState = nowPlayingMenu;
    if (speakerGroup != NULL) {
      speakerGroup->newSpeakerGroupParent = NULL;
    }
    if (force) {
      update();
    }
  }
}
}  // namespace homething_menu_base
}  // namespace esphome
