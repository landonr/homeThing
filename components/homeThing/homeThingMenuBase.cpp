#include "homeThingMenuBase.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_menu_base {

void HomeThingMenuBase::setup() {}

void HomeThingMenuBase::drawMenu() {
  menu_display_->drawMenu(activeMenu(), menuIndex);
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
  activeMenuState = MenuStates::rootMenu;
}

bool HomeThingMenuBase::selectMenu() {
  int menuIndexForSource = menuIndex;
  switch (activeMenuState) {
    case rootMenu:
      return selectRootMenu();
    case nowPlayingMenu:
      activeMenuState = MenuStates::nowPlayingMenu;
      break;
    case sourcesMenu: {
      auto sourceTitleState =
          std::static_pointer_cast<MenuTitleSource>(activeMenuTitle);
      idleMenu(true);
      speakerGroup->playSource(*sourceTitleState->media_source_);
      optionMenu = playingNewSourceMenu;
      menu_display_->updateDisplay(true);
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
      ESP_LOGD("WARNING", "menu is bad  %d", menuIndex);
      return false;
  }
  menuIndex = 0;
  return true;
}

std::shared_ptr<MenuTitleBase> HomeThingMenuBase::menuTitleForType(
    MenuStates stringType) {
  switch (stringType) {
    case nowPlayingMenu:
      return std::make_shared<MenuTitleBase>("Now Playing", "",
                                             ArrowMenuTitleRightIcon);
    case sourcesMenu:
      return std::make_shared<MenuTitleBase>("Sources", "",
                                             ArrowMenuTitleRightIcon);
    case backlightMenu:
      return std::make_shared<MenuTitleBase>("Backlight", "",
                                             NoMenuTitleRightIcon);
    case sleepMenu:
      return std::make_shared<MenuTitleBase>("Sleep", "", NoMenuTitleRightIcon);
    case mediaPlayersMenu:
      return std::make_shared<MenuTitleBase>("Media Players", "",
                                             ArrowMenuTitleRightIcon);
    case lightsMenu:
      return std::make_shared<MenuTitleBase>("Lights", "",
                                             ArrowMenuTitleRightIcon);
    case lightsDetailMenu:
      return std::make_shared<MenuTitleBase>("Light Detail", "",
                                             ArrowMenuTitleRightIcon);
    case switchesMenu:
      return std::make_shared<MenuTitleBase>("Switches", "",
                                             ArrowMenuTitleRightIcon);
    case scenesMenu:
      return std::make_shared<MenuTitleBase>("Scenes and Actions", "",
                                             ArrowMenuTitleRightIcon);
    case rootMenu:
      return std::make_shared<MenuTitleBase>("Home", "", NoMenuTitleRightIcon);
    case groupMenu:
      return std::make_shared<MenuTitleBase>("Speaker Group", "",
                                             ArrowMenuTitleRightIcon);
    case sensorsMenu:
      return std::make_shared<MenuTitleBase>("Sensors", "",
                                             ArrowMenuTitleRightIcon);
    case bootMenu:
      return std::make_shared<MenuTitleBase>("Boot", "", NoMenuTitleRightIcon);
  }
  return std::make_shared<MenuTitleBase>("", "", NoMenuTitleRightIcon);
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
    default:
      ESP_LOGW("WARNING", "menu is bad  %d", x);
      std::vector<std::shared_ptr<MenuTitleBase>> out;
      return out;
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
        menu_display_->updateDisplay(true);
        return;
      }
      break;
    case nowPlayingMenu:
      if (optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
        speakerGroup->sendActivePlayerRemoteCommand("power");
        menu_display_->updateDisplay(true);
        return;
      }

      switch (speakerGroup->activePlayer->get_player_type()) {
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
    menu_display_->updateDisplay(true);
  }
}

bool HomeThingMenuBase::buttonPressWakeUpDisplay() {
  if (idleTime != -1) {
    idleTime = 0;
  }
  if (speakerGroup != NULL && !speakerGroup->playerSearchFinished) {
    speakerGroup->findActivePlayer();
  }
  if (!backlight_->state) {
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
    menu_display_->updateDisplay(true);
  }
}

void HomeThingMenuBase::rotaryScrollClockwise() {
  animation_->resetAnimation();
  switch (activeMenuState) {
    case nowPlayingMenu:
      speakerGroup->increaseSpeakerVolume();
      optionMenu = volumeOptionMenu;
      debounceUpdateDisplay();
      return;
    case lightsDetailMenu:
      if (lightGroup->lightDetailSelected && menuIndex == 0 &&
          lightGroup->getActiveLight() != NULL) {
        lightGroup->getActiveLight()->incBrightness();
        debounceUpdateDisplay();
        return;
      } else if (lightGroup->lightDetailSelected && menuIndex == 1 &&
                 lightGroup->getActiveLight() != NULL) {
        lightGroup->getActiveLight()->incTemperature();
        debounceUpdateDisplay();
        return;
      } else if (lightGroup->lightDetailSelected && menuIndex == 2 &&
                 lightGroup->getActiveLight() != NULL) {
        lightGroup->getActiveLight()->incColor();
        debounceUpdateDisplay();
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
  debounceUpdateDisplay();
}

void HomeThingMenuBase::rotaryScrollCounterClockwise() {
  animation_->resetAnimation();
  switch (activeMenuState) {
    case nowPlayingMenu:
      speakerGroup->decreaseSpeakerVolume();
      optionMenu = volumeOptionMenu;
      debounceUpdateDisplay();
      return;
    case lightsDetailMenu:
      if (lightGroup->lightDetailSelected && menuIndex == 0 &&
          lightGroup->getActiveLight() != NULL) {
        lightGroup->getActiveLight()->decBrightness();
        debounceUpdateDisplay();
        return;
      } else if (lightGroup->lightDetailSelected && menuIndex == 1 &&
                 lightGroup->getActiveLight() != NULL) {
        lightGroup->getActiveLight()->decTemperature();
        debounceUpdateDisplay();
        return;
      } else if (lightGroup->lightDetailSelected && menuIndex == 2 &&
                 lightGroup->getActiveLight() != NULL) {
        lightGroup->getActiveLight()->decColor();
        debounceUpdateDisplay();
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
  debounceUpdateDisplay();
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
        menu_display_->updateDisplay(true);
        return;
      }

      switch (speakerGroup->activePlayer->get_player_type()) {
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
      menu_display_->updateDisplay(true);
      return;
    case lightsDetailMenu:
      if (lightGroup->lightDetailSelected) {
        // deselect light if selected and stay in lightsDetailMenu
        lightGroup->lightDetailSelected = false;
        menu_display_->updateDisplay(true);
        return;
      } else {
        // if no light is selected go back to lightsMenu
        activeMenuState = lightsMenu;
        menu_display_->updateDisplay(true);
        return;
      }
      break;
    default:
      break;
  }
  if (optionMenu == speakerOptionMenu) {
    speakerGroup->toggleShuffle();
    optionMenu = noOptionMenu;
    menu_display_->updateDisplay(true);
    return;
  }
  optionMenu = noOptionMenu;
  lightGroup->clearActiveLight();
  topMenu();
  menu_display_->updateDisplay(true);
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
        menu_display_->updateDisplay(true);
        return;
      }

      switch (speakerGroup->activePlayer->get_player_type()) {
        case TVRemotePlayerType:
          speakerGroup->sendActivePlayerRemoteCommand("down");
          break;
        case SpeakerRemotePlayerType:
          if (optionMenu == speakerOptionMenu) {
            activeMenuState = groupMenu;
            menu_display_->updateDisplay(true);
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
        menu_display_->updateDisplay(true);
        return;
      }

      switch (speakerGroup->activePlayer->get_player_type()) {
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

void HomeThingMenuBase::buttonPressRight() {
  animation_->resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
    case bootMenu:
      menu_display_->skipBootSequence();
      break;
    case nowPlayingMenu:
      if (optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
        speakerGroup->sendActivePlayerRemoteCommand("menu");
        menu_display_->updateDisplay(true);
        return;
      }
      switch (speakerGroup->activePlayer->get_player_type()) {
        case TVRemotePlayerType:
          speakerGroup->sendActivePlayerRemoteCommand("right");
          break;
        case SpeakerRemotePlayerType:
          if (optionMenu == speakerOptionMenu) {
            speakerGroup->toggleMute();
            menu_display_->updateDisplay(true);
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
        case TVRemotePlayerType:
          menu_display_->updateDisplay(true);
          break;
        case SpeakerRemotePlayerType:
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
        case TVRemotePlayerType:
          if (optionMenu == tvOptionMenu) {
            optionMenu = noOptionMenu;
          } else {
            optionMenu = tvOptionMenu;
          }
          menu_display_->updateDisplay(true);
          break;
        case SpeakerRemotePlayerType:
          if (optionMenu == speakerOptionMenu) {
            optionMenu = noOptionMenu;
          } else {
            optionMenu = speakerOptionMenu;
          }
          menu_display_->updateDisplay(true);
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
      menu_display_->updateDisplay(true);
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
    menu_display_->updateDisplay(true);
  }
}

void HomeThingMenuBase::debounceUpdateDisplay() {
  // if (id(display_update_tick).state != rotary_) {
  //   id(display_update_tick).publish_state(id(rotary).state);
  // }
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
    menu_display_->updateDisplay(true);
  } else if (idleTime == display_timeout_) {
    if (speakerGroup != NULL && speakerGroup->playerSearchFinished) {
      if (charging && activeMenuState != bootMenu) {
        idleTime++;
        return;
      }
      activeMenuState = MenuStates::rootMenu;
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
            menu_display_->updateDisplay(true);
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
    activeMenuState = MenuStates::nowPlayingMenu;
    if (speakerGroup != NULL) {
      speakerGroup->newSpeakerGroupParent = NULL;
    }
    if (force) {
      menu_display_->updateDisplay(true);
    }
  }
}
}  // namespace homething_menu_base
}  // namespace esphome
