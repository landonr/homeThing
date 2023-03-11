#include "homeThingMenuBase.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_menu_base {

void HomeThingMenuBase::setup() {
  menu_display_->set_animation(animation_);
  this->animation_->animationTick->add_on_state_callback(
      [this](float state) { this->displayUpdateDebounced(); });

  display_update_tick_ = new sensor::Sensor();
  auto filter = new sensor::DebounceFilter(17);
  display_update_tick_->add_filter(filter);
  this->display_update_tick_->add_on_state_callback(
      [this](float state) { this->displayUpdateDebounced(); });

  this->light_group_->add_on_state_callback([this](float state) {
    switch (activeMenuState) {
      case lightsMenu:
      case lightsDetailMenu:
        this->update_display();
      default:
        break;
    }
  });

  this->speaker_group_->add_on_state_callback([this](float state) {
    switch (activeMenuState) {
      case bootMenu:
      case rootMenu:
      case nowPlayingMenu:
      case sourcesMenu:
      case groupMenu:
        this->update_display();
      default:
        break;
    }
  });

  this->switch_group_->add_on_state_callback([this](float state) {
    switch (activeMenuState) {
      case switchesMenu:
        this->update_display();
      default:
        break;
    }
  });

  this->sensor_group_->add_on_state_callback([this](float state) {
    switch (activeMenuState) {
      case sensorsMenu:
        this->update_display();
      default:
        break;
    }
  });
}

void HomeThingMenuBase::draw_menu_screen() {
  if (idleTime > 16 && !get_charging()) {
    ESP_LOGW(TAG, "not drawing");
    return;
  }
  if (!menu_drawing_) {
    menu_drawing_ = true;
    menu_titles = activeMenu();
    ESP_LOGD(TAG, "drawMenu:%d %s #%d", menuIndex,
             menuTitleForType(activeMenuState)->get_name().c_str(),
             menu_titles.size());
    if (menu_display_->draw_menu_screen(&activeMenuState, menu_titles,
                                        menuIndex, option_menu_)) {
      this->animation_->tickAnimation();
      this->animation_->animating = true;
    } else {
      this->animation_->animating = false;
    }
    menu_drawing_ = false;
  }
}
void HomeThingMenuBase::topMenu() {
  if (activeMenuState == bootMenu) {
    return;
  }
  animation_->resetAnimation(true);
  if (speaker_group_ != NULL)
    speaker_group_->newSpeakerGroupParent = NULL;
  option_menu_ = noOptionMenu;
  activeMenuState = rootMenu;
  menuIndex = 0;
}

void HomeThingMenuBase::update() {
  idleTick();
}

bool HomeThingMenuBase::selectMenu() {
  auto activeMenuTitle = menu_titles[menuIndex];
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
      option_menu_ = playingNewSourceMenu;
      update_display();
      break;
    }
    case groupMenu: {
      auto playerTitleState =
          std::static_pointer_cast<MenuTitlePlayer>(activeMenuTitle);
      speaker_group_->selectGroup(playerTitleState->media_player_, menuIndex);
      break;
    }
    case lightsMenu: {
      light_group_->toggleLight(menuIndex);
      return true;
    }
    case lightsDetailMenu:
      light_group_->lightDetailSelected = true;
    case mediaPlayersMenu: {
      auto media_player_title =
          std::static_pointer_cast<MenuTitlePlayer>(activeMenuTitle);
      if (speaker_group_->selectMediaPlayers(
              media_player_title->media_player_)) {
        topMenu();
      }
      break;
    }
    case scenesMenu:
      if (service_group_->select_service(menuIndex)) {
        topMenu();
      }
      break;
    case switchesMenu:
      if (switch_group_->selectSwitch(menuIndex)) {
        topMenu();
      }
      break;
    default:
      ESP_LOGW(TAG, "menu state is bad but its an enum");
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
  menuIndex = 0;
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
      sleep_switch_->turn_on();
      return false;
    case sensorsMenu:
      activeMenuState = sensorsMenu;
      break;
    case lightsDetailMenu:
    case groupMenu:
    case rootMenu:
    case bootMenu:
      ESP_LOGD(TAG, "select_root_menu: selecting menu is bad %d %s", menuIndex,
               menuTitleForType(activeMenuState)->get_name().c_str());
      return false;
  }
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
  if (speaker_group_ != NULL && speaker_group_->playerSearchFinished &&
      activeMenuState == bootMenu) {
    ESP_LOGI(TAG, "finished boot");
    activeMenuState = rootMenu;
    topMenu();
  }
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
    case lightsMenu:
      return lightTitleSwitches(light_group_->lights);
    case switchesMenu:
      return switchTitleSwitches(switch_group_->switches);
    case nowPlayingMenu:
      return getNowPlayingMenuStates(
          speaker_group_->activePlayer->get_player_type());
    case bootMenu:
      return {};
    default:
      ESP_LOGW(TAG, "activeMenu: menu is bad %d, %s", menuIndex,
               menuTitleForType(activeMenuState)->get_name().c_str());
      return {};
  }
}

void HomeThingMenuBase::selectNowPlayingMenu() {
  if (menu_titles.size() <= 0 && menuIndex < menu_titles.size()) {
    return;
  }
  auto menu_name =
      getNowPlayingMenuStates(
          speaker_group_->activePlayer->get_player_type())[menuIndex]
          ->get_name();
  if (menu_name == "Pause") {
    speaker_group_->activePlayer->playPause();
  } else if (menu_name == "Vl Up") {
    speaker_group_->increaseSpeakerVolume();
    option_menu_ = volumeOptionMenu;
  } else if (menu_name == "Vl Down") {
    speaker_group_->decreaseSpeakerVolume();
    option_menu_ = volumeOptionMenu;
  } else if (menu_name == "Next") {
    speaker_group_->activePlayer->nextTrack();
  } else if (menu_name == "Shuffle") {
    speaker_group_->toggleShuffle();
  } else if (menu_name == "Menu") {
    topMenu();
  } else if (menu_name == "Group") {
    menuIndex = 0;
    activeMenuState = groupMenu;
  } else if (menu_name == "Power") {
    speaker_group_->sendActivePlayerRemoteCommand("power");
  } else if (menu_name == "Back") {
    speaker_group_->sendActivePlayerRemoteCommand("back");
  } else if (menu_name == "Home") {
    speaker_group_->sendActivePlayerRemoteCommand("menu");
  }
  update_display();
}

bool HomeThingMenuBase::buttonPressWakeUpDisplay() {
  if (idleTime != -1) {
    idleTime = 0;
  }
  // if (speaker_group_ != NULL && !speaker_group_->playerSearchFinished) {
  //   speaker_group_->findActivePlayer();
  // }
  if (backlight_ != NULL && !backlight_->state) {
    backlight_->turn_on();
    update_display();
    return true;
  }
  return false;
}

void HomeThingMenuBase::buttonPressSelect() {
  animation_->resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  if (menu_settings_->get_mode() == MENU_MODE_ROTARY) {
    switch (activeMenuState) {
      case lightsDetailMenu:
        if (light_group_->lightDetailSelected) {
          // deselect light if selected and stay in lightsDetailMenu
          light_group_->lightDetailSelected = false;
          update_display();
          return;
        }
        break;
      case nowPlayingMenu:
        if (option_menu_ == tvOptionMenu) {
          option_menu_ = noOptionMenu;
          speaker_group_->sendActivePlayerRemoteCommand("power");
          update_display();
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
  } else {
    switch (activeMenuState) {
      case lightsDetailMenu:
        if (light_group_->lightDetailSelected) {
          // deselect ligh if selected and stay in lightsDetailMenu
          light_group_->lightDetailSelected = false;
          update_display();
          return;
        }
        break;
      case nowPlayingMenu:
        if (option_menu_ == tvOptionMenu) {
          option_menu_ = noOptionMenu;
          update_display();
          return;
        }

        selectNowPlayingMenu();
        return;
      default:
        break;
    }
  }
  if (selectMenu()) {
    update_display();
  }
}

void HomeThingMenuBase::buttonPressSelectHold() {
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  if (selectMenuHold()) {
    update_display();
  }
}

void HomeThingMenuBase::rotaryScrollCounterClockwise(int rotary) {
  rotary_ = rotary;
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  animation_->resetAnimation();
  if (menu_settings_->get_mode() == MENU_MODE_ROTARY) {
    switch (activeMenuState) {
      case nowPlayingMenu:
        speaker_group_->decreaseSpeakerVolume();
        option_menu_ = volumeOptionMenu;
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
    } else if (menu_settings_->get_menu_rollover() && menuIndex == 0) {
      menuIndex = menu_titles.size() - 1;
    }
  } else {
    // 3 button
    if (activeMenuState == lightsDetailMenu && menuIndex > 0 &&
        light_group_->lightDetailSelected) {
      if (menuIndex == 1) {
        light_group_->getActiveLight()->incBrightness();
      } else if (menuIndex == 2) {
        light_group_->getActiveLight()->incTemperature();
      } else {
        selectMenu();
      }
    } else if (menuIndex > 0) {
      menuIndex--;
    } else if (activeMenuState == nowPlayingMenu) {
      menuIndex = menu_titles.size() - 1;
    } else if (activeMenuState == lightsDetailMenu && menuIndex == 0) {
      activeMenuState = lightsMenu;
    } else {
      topMenu();
    }
  }
  debounceUpdateDisplay();
}

void HomeThingMenuBase::rotaryScrollClockwise(int rotary) {
  rotary_ = rotary;
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  animation_->resetAnimation();
  if (menu_settings_->get_mode() == MENU_MODE_ROTARY) {
    switch (activeMenuState) {
      case nowPlayingMenu:
        speaker_group_->increaseSpeakerVolume();
        option_menu_ = volumeOptionMenu;
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
    if (menuIndex < menu_titles.size() - 1) {
      menuIndex++;
    } else if (menu_settings_->get_menu_rollover() &&
               menuIndex == menu_titles.size() - 1) {
      menuIndex = 0;
    }
  } else {
    // 3 button
    if (activeMenuState == lightsDetailMenu && menuIndex > 0 &&
        light_group_->lightDetailSelected) {
      if (menuIndex == 1) {
        light_group_->getActiveLight()->decBrightness();
      } else if (menuIndex == 2) {
        light_group_->getActiveLight()->decTemperature();
      } else {
        selectMenu();
      }
    } else if (menuIndex < menu_titles.size() - 1) {
      menuIndex++;
    } else if (menuIndex == menu_titles.size() - 1) {
      menuIndex = 0;
    } else {
      menuIndex = 0;
    }
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
      if (option_menu_ == tvOptionMenu) {
        option_menu_ = noOptionMenu;
        topMenu();
        update_display();
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
        option_menu_ = noOptionMenu;
      }
      update_display();
      return;
    case lightsDetailMenu:
      if (light_group_->lightDetailSelected) {
        // deselect light if selected and stay in lightsDetailMenu
        light_group_->lightDetailSelected = false;
        update_display();
        return;
      } else {
        // if no light is selected go back to lightsMenu
        activeMenuState = lightsMenu;
        update_display();
        return;
      }
      break;
    default:
      break;
  }
  if (option_menu_ == speakerOptionMenu) {
    speaker_group_->toggleShuffle();
    option_menu_ = noOptionMenu;
    update_display();
    return;
  }
  option_menu_ = noOptionMenu;
  light_group_->clearActiveLight();
  topMenu();
  update_display();
}

void HomeThingMenuBase::buttonPressDown() {
  animation_->resetAnimation();
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  switch (activeMenuState) {
    case nowPlayingMenu:
      if (option_menu_ == tvOptionMenu) {
        option_menu_ = noOptionMenu;
        speaker_group_->sendActivePlayerRemoteCommand("play");
        update_display();
        return;
      }

      switch (speaker_group_->activePlayer->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          speaker_group_->sendActivePlayerRemoteCommand("down");
          break;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
          if (option_menu_ == speakerOptionMenu) {
            activeMenuState = groupMenu;
            update_display();
          } else {
            speaker_group_->activePlayer->playPause();
            option_menu_ = noOptionMenu;
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
      if (option_menu_ == tvOptionMenu) {
        option_menu_ = noOptionMenu;
        speaker_group_->sendActivePlayerRemoteCommand("back");
        update_display();
        return;
      }

      switch (speaker_group_->activePlayer->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          speaker_group_->sendActivePlayerRemoteCommand("left");
          break;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
          option_menu_ = noOptionMenu;
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
      if (option_menu_ == tvOptionMenu) {
        option_menu_ = noOptionMenu;
        speaker_group_->sendActivePlayerRemoteCommand("menu");
        update_display();
        return;
      }
      switch (speaker_group_->activePlayer->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          speaker_group_->sendActivePlayerRemoteCommand("right");
          break;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
          if (option_menu_ == speakerOptionMenu) {
            speaker_group_->toggleMute();
            update_display();
          } else {
            speaker_group_->activePlayer->nextTrack();
          }
          option_menu_ = noOptionMenu;
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
          update_display();
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
          if (option_menu_ == tvOptionMenu) {
            option_menu_ = noOptionMenu;
          } else {
            option_menu_ = tvOptionMenu;
          }
          update_display();
          break;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
          if (option_menu_ == speakerOptionMenu) {
            option_menu_ = noOptionMenu;
          } else {
            option_menu_ = speakerOptionMenu;
          }
          update_display();
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
  option_menu_ = noOptionMenu;
  switch (activeMenuState) {
    case rootMenu:
    case backlightMenu:
    case sleepMenu:
    case nowPlayingMenu:
      speaker_group_->selectNextMediaPlayer();
      update_display();
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
  if (idleTime < 2 || animation_->animating || get_charging()) {
    update_display();
  }
}

void HomeThingMenuBase::debounceUpdateDisplay() {
  if (menu_settings_->get_mode() == MENU_MODE_ROTARY) {
    if (display_update_tick_->state != rotary_) {
      display_update_tick_->publish_state(rotary_);
    }
  } else {
    update_display();
  }
}

void HomeThingMenuBase::idleTick() {
  ESP_LOGD(TAG, "idle %d", idleTime);
  if (activeMenuState == bootMenu) {
    if (idleTime == menu_settings_->get_display_timeout() && !get_charging()) {
      ESP_LOGD(TAG, "turning off display");
      // backlight_->turn_off();
    }
    idleTime++;
    return;
  }
  if (idleTime == 3) {
    option_menu_ = noOptionMenu;
    update_display();
  } else if (idleTime == menu_settings_->get_display_timeout()) {
    if (speaker_group_ != NULL && speaker_group_->playerSearchFinished) {
      if (get_charging() && activeMenuState != bootMenu) {
        idleTime++;
        return;
      }
      activeMenuState = rootMenu;
      animation_->resetAnimation();
      idleMenu(false);
      menu_display_->updateDisplay(false);
    }
    if (!get_charging()) {
      ESP_LOGD(TAG, "turning off display");
      // backlight_->turn_off();
    }
    idleTime++;
    return;
  } else if (idleTime == 180 && get_charging()) {
    idleMenu(true);
    idleTime++;
    return;
  } else if (idleTime > menu_settings_->get_sleep_after()) {
    if (!get_charging()) {
      ESP_LOGI(TAG, "night night");
      // sleep_switch_->turn_on();
      return;
    }
  }
  if (speaker_group_ != NULL) {
    bool updatedMediaPositions = speaker_group_->updateMediaPosition();
    if (updatedMediaPositions) {
      switch (activeMenuState) {
        case nowPlayingMenu:
          if (idleTime < 16 || get_charging()) {
            update_display();
          }
          break;
        default:
          break;
      }
    }
  }
  idleTime++;
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
  ESP_LOGI(TAG, "idleMenu %d", force);
  if (activeMenuState == bootMenu) {
    return;
  }
  if (!get_charging() || force) {
    light_group_->clearActiveLight();
    menuIndex = 0;
    animation_->resetAnimation();
    option_menu_ = noOptionMenu;
    activeMenuState = nowPlayingMenu;
    if (speaker_group_ != NULL) {
      speaker_group_->newSpeakerGroupParent = NULL;
    }
    if (force) {
      update_display();
    }
  }
}
}  // namespace homething_menu_base
}  // namespace esphome
