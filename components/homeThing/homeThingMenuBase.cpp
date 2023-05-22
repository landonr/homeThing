#include "homeThingMenuBase.h"
#include "esphome/core/log.h"
#include "version.h"

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

#ifdef USE_LIGHT_GROUP
  if (this->light_group_) {
    this->light_group_->add_on_state_callback([this](float state) {
      switch (activeMenuState) {
        case lightsMenu:
        case lightsDetailMenu:
          reload_menu_items_ = true;
          this->update_display();
        default:
          break;
      }
    });
  }
#endif

#ifdef USE_MEDIA_PLAYER_GROUP
  if (this->media_player_group_) {
    this->media_player_group_->add_on_state_callback([this](float state) {
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
  }
#endif

#ifdef USE_SWITCH_GROUP
  if (this->switch_group_) {
    this->switch_group_->add_on_state_callback([this](float state) {
      switch (activeMenuState) {
        case switchesMenu:
          reload_menu_items_ = true;
          this->update_display();
        default:
          break;
      }
    });
  }
#endif

#ifdef USE_SENSOR_GROUP
  if (this->sensor_group_) {
    this->sensor_group_->add_on_state_callback([this](float state) {
      switch (activeMenuState) {
        case sensorsMenu:
          reload_menu_items_ = true;
          this->update_display();
        default:
          break;
      }
    });
  }
#endif
}

void HomeThingMenuBase::draw_menu_screen() {
  if (display_can_sleep()) {
    ESP_LOGI(TAG, "draw_menu_screen: not drawing");
    sleep_display();
    return;
  }
  if (!menu_drawing_) {
    menu_drawing_ = true;
    auto title_name = menuTitleForType(activeMenuState)->get_name();
    if (reload_menu_items_ ||
        (menu_titles.size() == 0 && activeMenuState != bootMenu)) {
      ESP_LOGD(TAG, "draw_menu_screen: reload %d %s #%d", menuIndex,
               title_name.c_str(), menu_titles.size());
      menu_titles.clear();
      menu_titles = activeMenu();
      reload_menu_items_ = false;
    }
    ESP_LOGD(TAG, "draw_menu_screen: draw %d %s #%d", menuIndex,
             title_name.c_str(), menu_titles.size());
    if (menu_display_->draw_menu_screen(&activeMenuState, &menu_titles,
                                        menuIndex,
                                        circle_menu_->get_active_menu())) {
      this->animation_->tickAnimation();
      this->animation_->animating = true;
    } else {
      this->animation_->animating = false;
    }
    if (device_locked_ && idleTime < 3) {
      menu_display_->draw_lock_screen(unlock_presses_);
    } else {
      ESP_LOGD(TAG,
               "draw_menu_screen: unlocked, not drawing - time: %d locked: %d",
               idleTime, device_locked_);
    }
    menu_drawing_ = false;
  }
}

void HomeThingMenuBase::topMenu() {
  if (activeMenuState == bootMenu) {
    return;
  }
  ESP_LOGI(TAG, "topMenu: activeMenuState %d", activeMenuState);
  activeMenuState = rootMenu;
  reset_menu();
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
      auto baseTitleState =
          std::static_pointer_cast<MenuTitleBase>(activeMenuTitle);
      if (baseTitleState->titleType == SourceMenuTitleType) {
        auto sourceTitleState =
            std::static_pointer_cast<MenuTitleSource>(activeMenuTitle);
        idleMenu(true);
        media_player_group_->playSource(sourceTitleState->media_source_);
        circle_menu_->set_active_menu(playingNewSourceMenu,
                                      media_player_group_->active_player_);
        update_display();
      } else {
        media_player_group_->set_active_player_source_index(menuIndex);
        menuIndex = 0;
        update_display();
      }
      break;
    }
    case groupMenu: {
      auto playerTitleState =
          std::static_pointer_cast<MenuTitlePlayer>(activeMenuTitle);
      media_player_group_->selectGroup(playerTitleState->media_player_,
                                       menuIndex);
      break;
    }
    case lightsMenu: {
#ifdef USE_LIGHT_GROUP
      light_group_->toggleLight(menuIndex);
#endif
      return true;
    }
    case lightsDetailMenu:
#ifdef USE_LIGHT_GROUP
      light_group_->lightDetailSelected = true;
#endif
      break;
    case mediaPlayersMenu: {
      auto media_player_title =
          std::static_pointer_cast<MenuTitlePlayer>(activeMenuTitle);
      if (media_player_group_->selectMediaPlayers(
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
#ifdef USE_SWITCH_GROUP
      if (switch_group_->selectSwitch(menuIndex)) {
        topMenu();
      }
#endif
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
#ifdef USE_LIGHT_GROUP
      auto selectedLight = light_group_->lights[menuIndexForSource];
      if (selectedLight == NULL) {
        break;
      }
      if (light_group_->selectLightDetailAtIndex(menuIndexForSource)) {
        menuIndex = 0;
        activeMenuState = lightsDetailMenu;
      }
#endif
      return true;
    }
    default:
      break;
  }
  return false;
}

std::vector<MenuStates> HomeThingMenuBase::rootMenuTitles() {
  std::vector<MenuStates> out;
  if (media_player_group_) {
    out.insert(out.end(), {nowPlayingMenu, sourcesMenu, mediaPlayersMenu});
  }
  if (service_group_) {
    out.push_back(scenesMenu);
  }
#ifdef USE_SENSOR_GROUP
  if (sensor_group_) {
    out.push_back(sensorsMenu);
  }
#endif
#ifdef USE_LIGHT_GROUP
  if (light_group_) {
    out.push_back(lightsMenu);
  }
#endif

#ifdef USE_SWITCH_GROUP
  if (switch_group_) {
    out.push_back(switchesMenu);
  }
#endif

  out.push_back(sleepMenu);
  out.push_back(aboutMenu);
  return out;
}

bool HomeThingMenuBase::selectRootMenu() {
  MenuStates currentMenu = rootMenuTitles()[menuIndex];
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
      sleep_display();
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

void HomeThingMenuBase::finish_boot() {
  ESP_LOGI(TAG, "finish_boot: finished boot");
  activeMenuState = rootMenu;
  idleTime = 0;
  topMenu();
}

std::vector<std::shared_ptr<MenuTitleBase>> HomeThingMenuBase::activeMenu() {
  if (media_player_group_ && media_player_group_->playerSearchFinished &&
      activeMenuState == bootMenu) {
    finish_boot();
  }
  ESP_LOGI(TAG, "activeMenu: finished boot");
  switch (activeMenuState) {
    case rootMenu:
      return menuTypesToTitles(rootMenuTitles());
    case sourcesMenu: {
      auto sources = media_player_group_->activePlayerSources();
      auto index = media_player_group_->get_active_player_source_index();
      if (index == -1 && sources.size() > 1) {
        auto sourceTitles = activePlayerSourceTitles(sources);
        return {sourceTitles.begin(), sourceTitles.end()};
      } else if (index == -1 && sources.size() == 1) {
        auto playerSources = sources[0]->get_sources();
        auto sourceTitles = activePlayerSourceItemTitles(playerSources);
        return {sourceTitles.begin(), sourceTitles.end()};
      } else if (sources.size() > 1) {
        auto playerSources = sources[index]->get_sources();
        auto sourceTitles = activePlayerSourceItemTitles(playerSources);
        return {sourceTitles.begin(), sourceTitles.end()};
      } else {
        return {};
      }
    }
    case mediaPlayersMenu: {
      auto mediaPlayersTitles =
          mediaPlayersTitleString(media_player_group_->media_players_);
      return {mediaPlayersTitles.begin(), mediaPlayersTitles.end()};
    }
    case scenesMenu:
      return sceneTitleStrings(service_group_->services);
    case sensorsMenu:
#ifdef USE_SENSOR_GROUP
      return sensorTitles(sensor_group_->sensors);
#endif
      break;
    case lightsMenu:
#ifdef USE_LIGHT_GROUP
      return lightTitleSwitches(light_group_->lights);
#endif
      break;
    case lightsDetailMenu:
#ifdef USE_LIGHT_GROUP
      if (light_group_->getActiveLight() != NULL) {
        return lightTitleItems(light_group_->getActiveLight());
      } else {
        return {};
      }
#endif
      break;
    case switchesMenu:
#ifdef USE_SWITCH_GROUP
      return switchTitleSwitches(switch_group_->switches);
#endif
      break;
    case nowPlayingMenu:
      return speakerNowPlayingMenuStates(media_player_group_->active_player_);
    case groupMenu: {
      if (media_player_group_->newSpeakerGroupParent != NULL) {
        return groupTitleSwitches(media_player_group_->media_players_,
                                  media_player_group_->newSpeakerGroupParent);
      }
      return groupTitleString(media_player_group_->media_players_);
    }
    case bootMenu:
      break;
    default:
      ESP_LOGW(TAG, "activeMenu: menu is bad %d, %s", menuIndex,
               menuTitleForType(activeMenuState)->get_name().c_str());
      break;
  }
  return {};
}

void HomeThingMenuBase::selectNowPlayingMenu() {
  if (menu_titles.size() <= 0 && menuIndex < menu_titles.size()) {
    ESP_LOGI(TAG, "selectNowPlayingMenu: select menud %d", menuIndex);
    return;
  }
  auto menu_name = speakerNowPlayingMenuStates(
                       media_player_group_->active_player_)[menuIndex]
                       ->entity_id_;
  homeassistant_media_player::MediaPlayerSupportedFeature menu_item =
      homeassistant_media_player::supported_feature_item_map[menu_name];

  ESP_LOGI(TAG, "selectNowPlayingMenu: select menu # %d, named: %s, item %d",
           menuIndex, menu_name.c_str(), menu_item);
  switch (menu_item) {
    case homeassistant_media_player::MediaPlayerSupportedFeature::VOLUME_STEP:
      circle_menu_->set_active_menu(volumeOptionMenu,
                                    media_player_group_->active_player_);
      break;
    case homeassistant_media_player::MediaPlayerSupportedFeature::GROUPING:
      menuIndex = 0;
      activeMenuState = groupMenu;
      break;
    default:
      break;
  }
  select_media_player_feature(&menu_item);
  update_display();
}

bool HomeThingMenuBase::buttonPressWakeUpDisplay() {
  if (idleTime != -1) {
    idleTime = 0;
  }

  if (backlight_) {
    if (!backlight_->remote_values.is_on()) {
      ESP_LOGI(TAG, "buttonPressWakeUpDisplay: turning on display");
      turn_on_backlight();
      update_display();
      return true;
    } else {
      backlight_->turn_on()
          .set_transition_length(250)
          .set_brightness(1)
          .perform();
    }
  }
  return false;
}

bool HomeThingMenuBase::select_media_player_feature(
    homeassistant_media_player::MediaPlayerSupportedFeature* feature) {
  switch (*feature) {
    case homeassistant_media_player::MediaPlayerSupportedFeature::MENU_HOME:
      topMenu();
      return true;
    case homeassistant_media_player::MediaPlayerSupportedFeature::GROUPING:
      menuIndex = 0;
      activeMenuState = groupMenu;
      return true;
    default:
      media_player_group_->call_feature(*feature);
      break;
  }
  return false;
}

bool HomeThingMenuBase::button_press_now_playing_option_continue(
    CircleOptionMenuPosition position) {
  if (circle_menu_->get_active_menu()) {
    auto feature = circle_menu_->tap_option_menu(
        position, media_player_group_->get_active_player());
    if (feature) {
      ESP_LOGI(
          TAG,
          "button_press_now_playing_option_continue: option menu selected %d",
          *feature);
      circle_menu_->clear_active_menu();
      if (!select_media_player_feature(feature)) {
        return false;
      }
      reload_menu_items_ = true;
      update_display();
      return false;
    }
    ESP_LOGW(
        TAG,
        "button_press_now_playing_option_continue: option menu NOT selected");
    return false;
  }
  return true;
}

void HomeThingMenuBase::buttonPressSelect() {
  if (!button_press_and_continue())
    return;
  reload_menu_items_ = true;
  if (menu_settings_->get_mode() == MENU_MODE_ROTARY) {
    switch (activeMenuState) {
      case lightsDetailMenu:
#ifdef USE_LIGHT_GROUP
        if (light_group_->lightDetailSelected) {
          // deselect light if selected and stay in lightsDetailMenu
          light_group_->lightDetailSelected = false;
          reload_menu_items_ = true;
          update_display();
          return;
        }
#endif
        break;
      case nowPlayingMenu:
        if (!button_press_now_playing_option_continue(
                CircleOptionMenuPosition::CENTER))
          return;

        switch (media_player_group_->active_player_->get_player_type()) {
          case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
            media_player_group_->sendActivePlayerRemoteCommand(
                homeassistant_media_player::MediaPlayerTVRemoteCommand::SELECT);
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
#ifdef USE_LIGHT_GROUP
        if (light_group_->lightDetailSelected) {
          // deselect light if selected and stay in lightsDetailMenu
          light_group_->lightDetailSelected = false;
          reload_menu_items_ = true;
          update_display();
          return;
        }
#endif
        break;
      case nowPlayingMenu:
        // if (option_menu_ == tvOptionMenu) {
        //   option_menu_ = noOptionMenu;
        //   update_display();
        //   return;
        // }

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
  if (activeMenuState == bootMenu) {
    return;
  }
  if (selectMenuHold()) {
    reload_menu_items_ = true;
    update_display();
  }
}

void HomeThingMenuBase::rotaryScrollCounterClockwise(int rotary) {
  if (!button_press_and_continue())
    return;
  rotary_ = rotary;
  if (menu_settings_->get_mode() == MENU_MODE_ROTARY) {
    switch (activeMenuState) {
      case nowPlayingMenu:
        media_player_group_->decreaseSpeakerVolume();
        circle_menu_->set_active_menu(volumeOptionMenu,
                                      media_player_group_->active_player_);
        debounceUpdateDisplay();
        return;
      case lightsDetailMenu:
#ifdef USE_LIGHT_GROUP
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
#endif
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
#ifdef USE_LIGHT_GROUP
    if (activeMenuState == lightsDetailMenu && menuIndex > 0 &&
        light_group_->lightDetailSelected) {
      if (menuIndex == 1) {
        light_group_->getActiveLight()->incBrightness();
      } else if (menuIndex == 2) {
        light_group_->getActiveLight()->incTemperature();
      } else {
        selectMenu();
      }
      debounceUpdateDisplay();
      return;
    }
#endif

    if (menuIndex > 0) {
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
  if (!button_press_and_continue())
    return;
  rotary_ = rotary;
  if (menu_settings_->get_mode() == MENU_MODE_ROTARY) {
    switch (activeMenuState) {
      case nowPlayingMenu:
        media_player_group_->increaseSpeakerVolume();
        circle_menu_->set_active_menu(volumeOptionMenu,
                                      media_player_group_->active_player_);
        debounceUpdateDisplay();
        return;
      case lightsDetailMenu:
#ifdef USE_LIGHT_GROUP
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
#endif
        return;
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
#ifdef USE_LIGHT_GROUP
    if (activeMenuState == lightsDetailMenu && menuIndex > 0 &&
        light_group_->lightDetailSelected) {
      if (menuIndex == 1) {
        light_group_->getActiveLight()->decBrightness();
      } else if (menuIndex == 2) {
        light_group_->getActiveLight()->decTemperature();
      } else {
        selectMenu();
      }
      debounceUpdateDisplay();
      return;
    }
#endif

    if (menuIndex < menu_titles.size() - 1) {
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
  if (!button_press_and_continue())
    return;
  switch (activeMenuState) {
    case nowPlayingMenu:
      if (!button_press_now_playing_option_continue(
              CircleOptionMenuPosition::TOP))
        return;

      switch (media_player_group_->active_player_->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          media_player_group_->sendActivePlayerRemoteCommand(
              homeassistant_media_player::MediaPlayerTVRemoteCommand::UP);
          return;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
          break;
      }
      break;
    case groupMenu:
      menuIndex = 0;
      if (media_player_group_->newSpeakerGroupParent != NULL) {
        media_player_group_->newSpeakerGroupParent = NULL;
      } else {
        activeMenuState = nowPlayingMenu;
        circle_menu_->clear_active_menu();
      }
      update_display();
      return;
    case lightsDetailMenu:
#ifdef USE_LIGHT_GROUP
      if (light_group_->lightDetailSelected) {
        // deselect light if selected and stay in lightsDetailMenu
        light_group_->lightDetailSelected = false;
        reload_menu_items_ = true;
        update_display();
        return;
      } else {
        // if no light is selected go back to lightsMenu
        activeMenuState = lightsMenu;
        reload_menu_items_ = true;
        update_display();
        return;
      }
#endif
      break;
    default:
      break;
  }
// if (option_menu_ == speakerOptionMenu) {
//   media_player_group_->toggle_shuffle();
//   option_menu_ = noOptionMenu;
//   update_display();
//   return;
// }
// option_menu_ = noOptionMenu;
#ifdef USE_LIGHT_GROUP
  if (light_group_)
    light_group_->clearActiveLight();
#endif
  topMenu();
  update_display();
}

void HomeThingMenuBase::buttonPressDown() {
  if (!button_press_and_continue())
    return;
  switch (activeMenuState) {
    case nowPlayingMenu:
      // if (option_menu_ == tvOptionMenu) {
      if (!button_press_now_playing_option_continue(
              CircleOptionMenuPosition::BOTTOM))
        return;
      //   option_menu_ = noOptionMenu;
      //   media_player_group_->sendActivePlayerRemoteCommand("play");
      //   update_display();
      //   return;
      // }

      switch (media_player_group_->active_player_->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          media_player_group_->sendActivePlayerRemoteCommand(
              homeassistant_media_player::MediaPlayerTVRemoteCommand::DOWN);
          return;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
          // if (option_menu_ == speakerOptionMenu) {
          // activeMenuState = groupMenu;
          // update_display();
          // } else {
          media_player_group_->active_player_->playPause();
      }
      break;
    default:
      break;
  }
}

void HomeThingMenuBase::buttonPressLeft() {
  if (!button_press_and_continue())
    return;
  switch (activeMenuState) {
    case nowPlayingMenu:
      // if (option_menu_ == tvOptionMenu) {
      if (!button_press_now_playing_option_continue(
              CircleOptionMenuPosition::LEFT))
        return;
      // option_menu_ = noOptionMenu;
      // media_player_group_->sendActivePlayerRemoteCommand("back");
      // update_display();
      // return;
      // }

      switch (media_player_group_->active_player_->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          media_player_group_->sendActivePlayerRemoteCommand(
              homeassistant_media_player::MediaPlayerTVRemoteCommand::LEFT);
          return;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
          circle_menu_->clear_active_menu();
          break;
      }
      break;
    default:
      break;
  }
}

bool HomeThingMenuBase::skipBootPressed() {
  switch (activeMenuState) {
    case bootMenu: {
      switch (menu_display_->boot_->bootSequenceCanSkip(activeMenuState)) {
        case BOOT_MENU_SKIP_STATE_SLEEP:
          ESP_LOGI(TAG, "skipBootPressed: sleep");
          sleep_switch_->turn_on();
          return true;
        case BOOT_MENU_SKIP_STATE_MENU:
          ESP_LOGI(TAG, "skipBootPressed: menu");
          media_player_group_->selectFirstActivePlayer();
          finish_boot();
          return true;
        case BOOT_MENU_SKIP_STATE_NONE:
          ESP_LOGI(TAG, "skipBootPressed: none");
          break;
      }
      break;
    }
    default:
      break;
  }
  return false;
}

void HomeThingMenuBase::buttonPressRight() {
  if (skipBootPressed())
    return;
  if (!button_press_and_continue())
    return;
  switch (activeMenuState) {
    case nowPlayingMenu:
      // if (option_menu_ == tvOptionMenu) {
      if (!button_press_now_playing_option_continue(
              CircleOptionMenuPosition::RIGHT))
        return;
      // option_menu_ = noOptionMenu;
      // media_player_group_->sendActivePlayerRemoteCommand("menu");
      // update_display();
      // return;
      // }
      switch (media_player_group_->active_player_->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          media_player_group_->sendActivePlayerRemoteCommand(
              homeassistant_media_player::MediaPlayerTVRemoteCommand::RIGHT);
          return;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
          // if (option_menu_ == speakerOptionMenu) {
          //   media_player_group_->toggle_mute();
          //   update_display();
          // } else {
          media_player_group_->active_player_->nextTrack();
          // }
          break;
      }
      break;
    default:
      break;
  }
}

void HomeThingMenuBase::buttonReleaseScreenLeft() {
  if (!button_press_and_continue())
    return;
  switch (activeMenuState) {
    case nowPlayingMenu:
      switch (media_player_group_->active_player_->get_player_type()) {
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

bool HomeThingMenuBase::buttonPressUnlock() {
  if (device_locked_) {
    unlock_presses_++;
    if (unlock_presses_ >= 2) {
      device_locked_ = false;
      unlock_presses_ = 0;
    }
    idleTime = 0;
    update_display();
    return true;
  }
  return false;
}

void HomeThingMenuBase::buttonPressScreenLeft() {
  if (buttonPressUnlock()) {
    return;
  } else if (!button_press_and_continue()) {
    return;
  }
  switch (activeMenuState) {
    case nowPlayingMenu:
      if (circle_menu_->get_active_menu()) {
        circle_menu_->clear_active_menu();
      } else {
        circle_menu_->set_active_menu(speakerOptionMenu,
                                      media_player_group_->active_player_);
      }
      update_display();
      break;
    default:
      break;
  }
}

void HomeThingMenuBase::buttonPressScreenRight() {
  if (!button_press_and_continue())
    return;
  circle_menu_->clear_active_menu();
  switch (activeMenuState) {
    case rootMenu:
    case backlightMenu:
    case sleepMenu:
    case nowPlayingMenu:
      media_player_group_->selectNextMediaPlayer();
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

bool HomeThingMenuBase::display_can_sleep() {
  int timeout = menu_settings_->get_display_timeout();
  bool idle_timeout = timeout != 0 && idleTime >= timeout;

  bool display_timeout_while_charging = false;
  bool charging_timeout =
      display_timeout_while_charging ? get_charging() : false;

  ESP_LOGD(TAG, "screen timeout %d, charging %d, charging timeout %d idle %d",
           idle_timeout, get_charging(), charging_timeout, idle_timeout);

  if (get_charging()) {
    if (display_timeout_while_charging)
      return idle_timeout;
    else
      return false;
  } else {
    return idle_timeout;
  }
}

void HomeThingMenuBase::turn_on_backlight() {
  if (backlight_ && !backlight_->remote_values.is_on()) {
    ESP_LOGI(TAG, "turn_on_backlight: turning on display");
    auto call = backlight_->turn_on();
    call.set_transition_length(250);
    call.set_brightness(1);
    call.perform();
  }
}

void HomeThingMenuBase::sleep_display() {
  if (backlight_ && backlight_->remote_values.is_on()) {
    ESP_LOGI(TAG, "sleep_display: turning off display");
    auto call = backlight_->turn_off();
    call.set_transition_length(500);
    call.perform();
  } else {
    ESP_LOGD(TAG, "sleep_display: NOT turning off display %d, %d", backlight_,
             backlight_->remote_values.is_on());
  }
}

void HomeThingMenuBase::fade_out_display() {
  if (backlight_ && backlight_->remote_values.is_on()) {
    ESP_LOGI(TAG, "fade_out_display: fading out display");
    auto call = backlight_->turn_on();
    call.set_brightness(0.3);
    call.set_transition_length(500);
    call.perform();
  }
}

void HomeThingMenuBase::lockDevice() {
  if (!device_locked_) {
    ESP_LOGI(TAG, "lockDevice: locking device");
    device_locked_ = true;
    unlock_presses_ = 0;
  }
}

void HomeThingMenuBase::idleTick() {
  ESP_LOGD(TAG, "idleTick: idle %d", idleTime);
  if (activeMenuState == bootMenu) {
    if (display_can_sleep()) {
      sleep_display();
    }
    idleTime++;
    return;
  }
  if (menu_settings_->get_lock_after() != 0 &&
      idleTime >= menu_settings_->get_lock_after()) {
    lockDevice();
  }
  if (idleTime == 2) {
    unlock_presses_ = 0;
  } else if (idleTime == 3) {
    circle_menu_->clear_active_menu();
    update_display();
  } else if (idleTime == menu_settings_->get_display_timeout() - 4) {
    fade_out_display();
  } else if (idleTime == menu_settings_->get_display_timeout()) {
    if (media_player_group_ != NULL &&
        media_player_group_->playerSearchFinished) {
      if (get_charging() && activeMenuState != bootMenu) {
        idleTime++;
        return;
      }
      ESP_LOGI(TAG, "idleTick: idle root menu %d", display_can_sleep());
      activeMenuState = rootMenu;
      animation_->resetAnimation();
      idleMenu(false);
      menu_display_->updateDisplay(false);
    }

    ESP_LOGD(TAG, "idleTick: turning off display? %d", display_can_sleep());
    if (display_can_sleep()) {
      ESP_LOGD(TAG, "idleTick: turning off display");
      sleep_display();
    }
    idleTime++;
    return;
  } else if (idleTime == 180 && get_charging()) {
    idleMenu(true);
    idleTime++;
    return;
  } else if (idleTime > menu_settings_->get_sleep_after()) {
    ESP_LOGD(TAG, "idleTick: night night? %d", get_charging());
    if (!get_charging() && sleep_switch_) {
      ESP_LOGD(TAG, "idleTick: night night");
      sleep_switch_->turn_on();
      return;
    }
  }
  if (media_player_group_ != NULL) {
    bool updatedMediaPositions = media_player_group_->updateMediaPosition();
    if (updatedMediaPositions) {
      switch (activeMenuState) {
        case nowPlayingMenu: {
          ESP_LOGD(TAG, "idleTick: update media positions %d",
                   display_can_sleep());
          if (!display_can_sleep()) {
            update_display();
          } else {
            if (!get_charging())
              sleep_display();
          }
          break;
        }
        default:
          break;
      }
    }
  }
  idleTime++;
}

void HomeThingMenuBase::goToScreenFromString(std::string screenName) {
  ESP_LOGI(TAG, "goToScreenFromString: %s", screenName.c_str());
  if (screenName == "nowPlaying") {
    activeMenuState = nowPlayingMenu;
  } else if (screenName == "sources") {
    activeMenuState = sourcesMenu;
  } else if (screenName == "backlight") {
    activeMenuState = backlightMenu;
  } else if (screenName == "sleep") {
    activeMenuState = sleepMenu;
  } else if (screenName == "mediaPlayers") {
    activeMenuState = mediaPlayersMenu;
  } else if (screenName == "lights") {
    activeMenuState = lightsMenu;
  } else if (screenName == "lightDetail") {
    activeMenuState = lightsDetailMenu;
  } else if (screenName == "switches") {
    activeMenuState = switchesMenu;
  } else if (screenName == "scenes") {
    activeMenuState = scenesMenu;
  } else if (screenName == "home") {
    activeMenuState = rootMenu;
  } else if (screenName == "speakerGroup") {
    activeMenuState = groupMenu;
  } else if (screenName == "sensors") {
    activeMenuState = sensorsMenu;
  } else if (screenName == "boot") {
    activeMenuState = bootMenu;
  }
  reset_menu();
  update_display();
}

void HomeThingMenuBase::idleMenu(bool force) {
  ESP_LOGI(TAG, "idleMenu %d", force);
  if (activeMenuState == bootMenu) {
    return;
  }
  if (!get_charging() || force) {
#ifdef USE_LIGHT_GROUP
    if (light_group_)
      light_group_->clearActiveLight();
#endif
    reset_menu();
    activeMenuState = nowPlayingMenu;
    if (media_player_group_)
      media_player_group_->newSpeakerGroupParent = NULL;
    if (force) {
      update_display();
    }
  }
}
}  // namespace homething_menu_base
}  // namespace esphome
