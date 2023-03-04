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
  if (speaker_group_ != NULL)
    speaker_group_->newSpeakerGroupParent = NULL;
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
      speaker_group_->playSource(*sourceTitleState->media_source_);
      optionMenu = playingNewSourceMenu;
      update();
      break;
    }
    case groupMenu: {
      auto playerTitleState =
          std::static_pointer_cast<MenuTitlePlayer>(activeMenuTitle);
      speaker_group_->selectGroup(playerTitleState->media_player_, menuIndex);
      break;
    }
    case lightsMenu: {
      light_group_->toggleLight(menuIndexForSource);
      return true;
    }
    case lightsDetailMenu:
      light_group_->lightDetailSelected = true;
    case mediaPlayersMenu:
      selectMediaPlayers();
      break;
    case scenesMenu:
      if (service_group_->select_service(menuIndexForSource)) {
        topMenu();
      }
      break;
    case switchesMenu:
      if (switch_group_->selectSwitch(menuIndexForSource)) {
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
      auto selectedLight = light_group_->lights[menuIndexForSource];
      if (selectedLight == NULL) {
        break;
      }
      if (light_group_->selectLightDetailAtIndex(menuIndexForSource)) {
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
  MenuStates currentMenu =
      rootMenuTitles(speaker_group_ != NULL, service_group_ != NULL, true, true,
                     true)[menuIndex];
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
          speaker_group_ != NULL, service_group_ != NULL, true, true, true));
    case sourcesMenu: {
      auto sources = speaker_group_->activePlayerSources();
      auto sourceTitles = activePlayerSourceTitles(sources);
      return {sourceTitles.begin(), sourceTitles.end()};
    }
    case mediaPlayersMenu: {
      auto mediaPlayersTitles =
          mediaPlayersTitleString(speaker_group_->media_players_);
      return {mediaPlayersTitles.begin(), mediaPlayersTitles.end()};
    }
    case scenesMenu:
      return sceneTitleStrings(service_group_->services);
    case sensorsMenu:
      return sensorTitles(sensor_group_->sensors);
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
      if (light_group_->lightDetailSelected) {
        // deselect light if selected and stay in lightsDetailMenu
        light_group_->lightDetailSelected = false;
        update();
        return;
      }
      break;
    case nowPlayingMenu:
      if (optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
        speaker_group_->sendActivePlayerRemoteCommand("power");
        update();
        return;
      }

      switch (speaker_group_->activePlayer->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          speaker_group_->sendActivePlayerRemoteCommand("select");
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
  if (speaker_group_ != NULL && !speaker_group_->playerSearchFinished) {
    speaker_group_->findActivePlayer();
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
  } else if (menu_rollover_on_ && menuIndex == activeMenuTitleCount - 1) {
    menuIndex = 0;
  }
  debounceUpdateDisplay();
}

void HomeThingMenuBase::rotaryScrollCounterClockwise(int rotary) {
  rotary_ = rotary;
  animation_->resetAnimation();
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
        update();
        return;
      }

      switch (speaker_group_->activePlayer->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          speaker_group_->sendActivePlayerRemoteCommand("up");
          return;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
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
      update();
      return;
    case lightsDetailMenu:
      if (light_group_->lightDetailSelected) {
        // deselect light if selected and stay in lightsDetailMenu
        light_group_->lightDetailSelected = false;
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
    speaker_group_->toggleShuffle();
    optionMenu = noOptionMenu;
    update();
    return;
  }
  optionMenu = noOptionMenu;
  light_group_->clearActiveLight();
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
        speaker_group_->sendActivePlayerRemoteCommand("play");
        update();
        return;
      }

      switch (speaker_group_->activePlayer->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          speaker_group_->sendActivePlayerRemoteCommand("down");
          break;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
          if (optionMenu == speakerOptionMenu) {
            activeMenuState = groupMenu;
            update();
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

void HomeThingMenuBase::buttonPressLeft() {
  animation_->resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
    case nowPlayingMenu:
      if (optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
        speaker_group_->sendActivePlayerRemoteCommand("back");
        update();
        return;
      }

      switch (speaker_group_->activePlayer->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          speaker_group_->sendActivePlayerRemoteCommand("left");
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
        speaker_group_->sendActivePlayerRemoteCommand("menu");
        update();
        return;
      }
      switch (speaker_group_->activePlayer->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          speaker_group_->sendActivePlayerRemoteCommand("right");
          break;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
          if (optionMenu == speakerOptionMenu) {
            speaker_group_->toggleMute();
            update();
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

void HomeThingMenuBase::buttonReleaseScreenLeft() {
  animation_->resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
    case nowPlayingMenu:
      switch (speaker_group_->activePlayer->get_player_type()) {
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
      switch (speaker_group_->activePlayer->get_player_type()) {
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
      speaker_group_->selectNextMediaPlayer();
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
  if (display_update_tick_->state != rotary_) {
    display_update_tick_->publish_state(rotary_);
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
    if (speaker_group_ != NULL && speaker_group_->playerSearchFinished) {
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
  if (speaker_group_ != NULL) {
    bool updatedMediaPositions = speaker_group_->updateMediaPosition();
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
  for (auto& media_player : speaker_group_->media_players_) {
    if (media_player->entity_id_ == activeMenuTitle->entity_id_) {
      speaker_group_->activePlayer = media_player;
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
    light_group_->clearActiveLight();
    menuIndex = 0;
    animation_->resetAnimation();
    optionMenu = noOptionMenu;
    activeMenuState = nowPlayingMenu;
    if (speaker_group_ != NULL) {
      speaker_group_->newSpeakerGroupParent = NULL;
    }
    if (force) {
      update();
    }
  }
}
}  // namespace homething_menu_base
}  // namespace esphome
