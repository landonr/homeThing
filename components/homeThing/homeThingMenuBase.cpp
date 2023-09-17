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

  for (auto screen : menu_screens_) {
    screen->add_on_state_callback([this]() {
      switch (menuTree.back()) {
        case settingsMenu:
          reload_menu_items_ = true;
          this->update_display();
        default:
          break;
      }
    });
  }
  if (home_screen_) {
    home_screen_->add_on_state_callback([this]() {
      switch (menuTree.back()) {
        case rootMenu:
          reload_menu_items_ = true;
          this->update_display();
        default:
          break;
      }
    });
  }
  // #ifdef USE_MEDIA_PLAYER_GROUP
  // circle_menu_->set_bottom_menu(menu_display_->get_draw_now_playing_menu());
  // if (this->media_player_group_) {
  //   this->media_player_group_->add_on_state_callback([this](float state) {
  //     switch (menuTree.back()) {
  //       case bootMenu:
  //         ESP_LOGI(TAG, "draw_menu_screen: update display boot");
  //         this->update_display();
  //         break;
  //       case nowPlayingMenu:
  //       case groupMenu:
  //         ESP_LOGI(TAG, "draw_menu_screen: update display");
  //         reload_menu_items_ = true;
  //         this->displayUpdateDebounced();
  //         break;
  //       default:
  //         break;
  //     }
  //   });
  // }
  // #endif
}

void HomeThingMenuBase::draw_menu_screen() {
  auto activeMenuState = menuTree.back();
  if (display_can_sleep()) {
    ESP_LOGI(TAG, "draw_menu_screen: not drawing");
    sleep_display();
    return;
  }
  if (menu_drawing_) {
    return;
  }
  if (menuTree.front() == bootMenu && menu_display_->boot_->boot_complete()) {
    finish_boot();
    return;
  }
  menu_drawing_ = true;
  auto title_name = menu_state_title(activeMenuState);
  if (reload_menu_items_ ||
      (menu_titles.size() == 0 && activeMenuState != bootMenu)) {
    ESP_LOGI(TAG, "draw_menu_screen: reload %d index %d %s #%d",
             reload_menu_items_, menuIndex, title_name.c_str(),
             menu_titles.size());
    for (auto title : menu_titles) {
      delete title;
    }
    menu_titles.clear();
    activeMenu(&menu_titles);
    reload_menu_items_ = false;
  }
  ESP_LOGD(TAG, "draw_menu_screen: draw %d %s #%d", menuIndex,
           title_name.c_str(), menu_titles.size());
  // #ifdef USE_MEDIA_PLAYER_GROUP
  //   if (menu_display_->draw_menu_screen(&activeMenuState, &menu_titles, menuIndex,
  //                                       circle_menu_->get_active_menu(),
  //                                       editing_menu_item)) {
  // #else
  if (menu_display_->draw_menu_screen(&activeMenuState, &menu_titles, menuIndex,
                                      nullptr, editing_menu_item)) {
    // #endif
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

void HomeThingMenuBase::topMenu() {
  ESP_LOGI(TAG, "topMenu: active menu state %d", menuTree.back());
  if (menuTree.back() == bootMenu) {
    return;
  }
  menuTree.assign(1, rootMenu);
  reset_menu();
  update_display();
}

void HomeThingMenuBase::update() {
  idleTick();
}

bool HomeThingMenuBase::selectMenu() {
  auto activeMenuTitle = menu_titles[menuIndex];
  switch (menuTree.back()) {
    case rootMenu:
      return selectRootMenu();
      //     case nowPlayingMenu:
      // #ifdef USE_MEDIA_PLAYER_GROUP
      //       menuTree.push_back(nowPlayingMenu);
      // #endif
      //       break;
      //     case sourcesMenu: {
      // #ifdef USE_MEDIA_PLAYER_GROUP
      //       if (activeMenuTitle->titleType == SourceMenuTitleType) {
      //         const auto sourceTitleState =
      //             static_cast<MenuTitleSource*>(activeMenuTitle);
      //         auto source = sourceTitleState->media_source_;
      //         const auto new_source = new media_player_source::MediaPlayerSourceItem(
      //             source->get_name(), source->get_media_content_id(),
      //             source->get_media_type());
      //         media_player_group_->playSource(new_source);
      //         idleMenu(true);
      //         circle_menu_->set_active_menu(playingNewSourceMenu,
      //                                       media_player_group_->active_player_);
      //         update_display();
      //       } else {
      //         media_player_group_->set_active_player_source_index(menuIndex);
      //         menuIndex = 0;
      //         update_display();
      //       }
      // #endif
      //       break;
      //     }
      //     case groupMenu: {
      // #ifdef USE_MEDIA_PLAYER_GROUP
      //       auto playerTitleState = static_cast<MenuTitlePlayer*>(activeMenuTitle);
      //       media_player_group_->selectGroup(playerTitleState->media_player_,
      //                                        menuIndex);
      // #endif
      //       break;
      //     }
      //     case mediaPlayersMenu: {
      // #ifdef USE_MEDIA_PLAYER_GROUP
      //       auto media_player_title = static_cast<MenuTitlePlayer*>(activeMenuTitle);
      //       if (media_player_group_->selectMediaPlayers(
      //               media_player_title->media_player_)) {
      //         topMenu();
      //       }
      // #endif
    case lightsDetailMenu:
      ESP_LOGI(TAG, "selectMenu: began editing light detail");
      editing_menu_item = true;
      break;
    case settingsMenu:
      if (active_menu_screen && active_menu_screen->select_menu(menuIndex)) {
        auto selected_entity = active_menu_screen->get_selected_entity();
        if (selected_entity) {
          MenuItemType menu_item_type = std::get<0>(*selected_entity);
          ESP_LOGI(TAG, "selectMenu: began editing type %d", menu_item_type);
          editing_menu_item = true;
        }
        update_display();
      }
      break;
    case appMenu:
      ESP_LOGI(TAG, "selectMenu: select app menu %d", menuIndex);
      if (active_app_) {
        return active_app_->app_menu_select(menuIndex);
      }
      return false;
    default:
      ESP_LOGW(TAG, "selectMenu: menu state is bad but its an enum");
      return false;
  }
  return true;
}

bool HomeThingMenuBase::selectLightEntity(
    const std::tuple<MenuItemType, EntityBase*>* menu_item) {
  MenuItemType menu_item_type = std::get<0>(*menu_item);
  ESP_LOGW(TAG, "selectLightEntity: %d type: %d", menuIndex, menu_item_type);
  if (menu_item_type == MenuItemTypeLight) {
    ESP_LOGW(TAG, "selectLightEntit2y: %d type: %d", menuIndex, menu_item_type);
#ifdef USE_LIGHT
    auto light = static_cast<light::LightState*>(std::get<1>(*menu_item));
    ESP_LOGW(TAG, "selectMenuHold: name %s", light->get_name().c_str());
    if (supportsBrightness(light)) {
      active_menu_screen->set_selected_entity(menu_item);
      menuIndex = 0;
      menuTree.push_back(lightsDetailMenu);
      return true;
    }
#endif
  }
  ESP_LOGW(TAG, "selectLightEntity: %d returned", menuIndex);
  return false;
}

bool HomeThingMenuBase::selectMenuHold() {
  ESP_LOGW(TAG, "selectMenuHold: %d", menuIndex);
  switch (menuTree.back()) {
    case rootMenu: {
      if (home_screen_) {
        int index = menuIndex;
        auto menu_item = home_screen_->get_menu_item(index);
        ESP_LOGW(TAG, "selectMenuHold: %d type: %d, name %s type %s", index,
                 std::get<0>(*menu_item),
                 home_screen_->entity_name_at_index(index).c_str(),
                 nameForMenuItemType(std::get<0>(*menu_item)).c_str());
        return selectLightEntity(menu_item);
      }
      break;
    }
    case settingsMenu: {
      if (active_menu_screen) {
        auto menu_item = active_menu_screen->get_menu_item(menuIndex);
        return selectLightEntity(menu_item);
      }
      break;
    }
    default:
      break;
  }
  return false;
}

bool HomeThingMenuBase::selectRootMenu() {
  // if (menuIndex < menu_titles.size()) {
  //   MenuStates currentMenu = menu_titles[menuIndex];
  //   ESP_LOGW(TAG, "select_root_menu: selecting menu %d %s of %d", menuIndex,
  //            menu_state_title(menuTree.back()).c_str(),
  //            menu_titles.size());
  //   switch (currentMenu) {
  //     // case sourcesMenu:
  //     //   menuTree.push_back(sourcesMenu);
  //     //   break;
  //     // case nowPlayingMenu:
  //     //   menuTree.push_back(nowPlayingMenu);
  //     //   break;
  //     // case mediaPlayersMenu:
  //     //   menuTree.push_back(mediaPlayersMenu);
  //     //   break;
  //     // case groupMenu:
  //     case entityMenu:
  //     case settingsMenu:
  //     case lightsDetailMenu:
  //     case rootMenu:
  //     case bootMenu:
  //       ESP_LOGW(TAG, "select_root_menu: selecting menu is bad %d %s",
  //                menuIndex, menu_state_title(menuTree.back()).c_str());
  //       menuIndex = 0;
  //       return false;
  //   }
  // }
  int offset =
      now_playing_control_ ? now_playing_control_->root_menu_size() : 0;
  int index = menuIndex - offset;
  if (menuIndex < offset) {
    ESP_LOGI(TAG, "selectRootMenu: now playing %d offset %d", menuIndex,
             offset);
    if (now_playing_control_) {
      active_app_ = now_playing_control_;
      active_app_->set_app_menu_index(menuIndex);
      menuTree.push_back(appMenu);
    }
  } else if (home_screen_ && index < home_screen_->get_entity_count()) {
    ESP_LOGI(TAG, "selectRootMenu: home screen %d offset %d", menuIndex,
             offset);
    if (!home_screen_->select_menu(index)) {
      // update_display();
      return false;
    }
  } else {
    ESP_LOGI(TAG, "selectRootMenu: screen %d offset %d", menuIndex, offset);
    menuTree.push_back(settingsMenu);
    int home_screen_count = home_screen_ ? home_screen_->get_entity_count() : 0;
    offset = home_screen_count + offset;
    index = menuIndex - offset;
    ESP_LOGI(TAG,
             "selectRootMenu: 3 %d new offset %d menu screens %d index %d home "
             "screen count %d",
             menuIndex, offset, menu_screens_.size(), index, home_screen_count);
    active_menu_screen = menu_screens_[index];
  }
  menuIndex = 0;
  return true;
}

MenuTitleBase* HomeThingMenuBase::menuTitleForType(MenuStates stringType,
                                                   int index) {
  if (stringType == settingsMenu && menu_screens_.size() > 0) {
    int offset = home_screen_ ? home_screen_->get_entity_count() : 0;
    HomeThingMenuScreen* menu_screen = menu_screens_[index - offset];
    std::string menu_name = menu_screen->get_name();
    return new MenuTitleBase(menu_name, "", ArrowMenuTitleRightIcon);
  } else if (stringType == entityMenu && home_screen_) {
    std::string menu_name = home_screen_->entity_name_at_index(index);
    return new MenuTitleBase(menu_name, "", NoMenuTitleRightIcon);
  }
  return new MenuTitleBase(menu_state_title(stringType), "",
                           menu_state_right_icon(stringType));
}

void HomeThingMenuBase::finish_boot() {
  ESP_LOGI(TAG, "finish_boot: finished boot %d", menuTree.size());
  menuTree.assign(1, rootMenu);
  idleTime = 0;
  topMenu();
}

void HomeThingMenuBase::activeMenu(std::vector<MenuTitleBase*>* menu_titles) {
  // #ifdef USE_MEDIA_PLAYER_GROUP
  //   if (media_player_group_ && media_player_group_->playerSearchFinished &&
  //       menuTree.back() == bootMenu) {
  //     finish_boot();
  //   }
  // #endif
  switch (menuTree.back()) {
    case rootMenu: {
      if (now_playing_control_ != nullptr) {
        now_playing_control_->rootMenuTitles(menu_titles);
      }
      // if (home_screen_) {
      //   // for (int i = 0; i < home_screen_->get_entity_count(); i++) {
      //   //   out.push_back(entityMenu);
      //   // }
      //   home_screen_->menu_titles(menu_titles, false);
      //   active_menu_screen = home_screen_;
      // }
      for (auto& menu_screen : menu_screens_) {
        std::string menu_name = menu_screen->get_name();
        menu_titles->push_back(
            new MenuTitleBase(menu_name, "", ArrowMenuTitleRightIcon));
      }
      return;
    }
    case lightsDetailMenu: {
#ifdef USE_LIGHT
      auto selectedEntity = active_menu_screen->get_selected_entity();
      if (selectedEntity != NULL &&
          std::get<0>(*selectedEntity) == MenuItemTypeLight) {
        auto light =
            static_cast<light::LightState*>(std::get<1>(*selectedEntity));
        lightTitleItems(light, menu_titles);
        return;
      } else {
        return;
      }
#endif
      break;
    }
      //     case sourcesMenu: {
      // #ifdef USE_MEDIA_PLAYER_GROUP
      //       const auto sources = media_player_group_->activePlayerSources();
      //       const auto index = media_player_group_->get_active_player_source_index();
      //       if (index == -1 && sources->size() > 1) {
      //         activePlayerSourceTitles(sources, menu_titles);
      //         return;
      //       } else if (index == -1 && sources->size() == 1) {
      //         auto playerSources = (*sources)[0]->get_sources();
      //         activePlayerSourceItemTitles(playerSources, menu_titles);
      //         return;
      //       } else if (sources->size() > 1) {
      //         auto playerSources = (*sources)[index]->get_sources();
      //         activePlayerSourceItemTitles(playerSources, menu_titles);
      //         return;
      //       }
      // #endif
      //       break;
      //     }
      //     case mediaPlayersMenu: {
      // #ifdef USE_MEDIA_PLAYER_GROUP
      //       mediaPlayersTitleString(media_player_group_->get_media_players(),
      //                               menu_titles);
      //       return;
      // #endif
      //       break;
      //     }
      //     case nowPlayingMenu:
      // #ifdef USE_MEDIA_PLAYER_GROUP
      //       speakerNowPlayingMenuStates(media_player_group_->active_player_,
      //                                   menu_display_->get_draw_now_playing_menu(),
      //                                   menu_titles);
      //       return;
      // #endif
      //       break;
      //     case groupMenu: {
      // #ifdef USE_MEDIA_PLAYER_GROUP
      //       if (media_player_group_->newSpeakerGroupParent != NULL) {
      //         return groupTitleSwitches(media_player_group_->get_media_players(),
      //                                   media_player_group_->newSpeakerGroupParent,
      //                                   menu_titles);
      //         return;
      //       }
      //       groupTitleString(media_player_group_->get_media_players(), menu_titles);
      //       return;
      // #endif
      //       break;
      //     }
    case bootMenu:
      break;
    case settingsMenu:
      active_menu_screen->menu_titles(menu_titles, true);
    case appMenu:
      if (active_app_) {
        active_app_->app_menu_titles(menu_titles);
      }
    default:
      ESP_LOGW(TAG, "activeMenu: menu is bad %d, %s", menuIndex,
               menu_state_title(menuTree.back()).c_str());
      break;
  }
}

bool HomeThingMenuBase::buttonPressWakeUpDisplay() {
  if (idleTime != -1) {
    idleTime = 0;
  }

#ifdef USE_LIGHT
  if (backlight_) {
    if (!backlight_->remote_values.is_on()) {
      ESP_LOGI(TAG, "buttonPressWakeUpDisplay: turning on display");
      turn_on_backlight();
      update_display();
      return true;
    } else if (backlight_->remote_values.get_brightness() < 1) {
      ESP_LOGI(TAG, "buttonPressWakeUpDisplay: turning on display 2");
      backlight_->turn_on()
          .set_transition_length(250)
          .set_brightness(1)
          .perform();
    }
  }
#endif
  return false;
}

// #ifdef USE_MEDIA_PLAYER_GROUP
// void HomeThingMenuBase::selectNowPlayingMenu() {
//   if (menu_titles.size() <= 0 && menuIndex < menu_titles.size()) {
//     ESP_LOGI(TAG, "selectNowPlayingMenu: select menu %d", menuIndex);
//     return;
//   }
//   auto features = media_player_group_->active_player_->get_option_menu_features(
//       menu_display_->get_draw_now_playing_menu());
//   auto active_feature = (*features)[menuIndex];
//   ESP_LOGI(TAG, "selectNowPlayingMenu: %d, %s", menuIndex,
//            active_feature->get_title().c_str());
//   switch (active_feature->get_feature()) {
//     case homeassistant_media_player::MediaPlayerSupportedFeature::VOLUME_SET:
//     case homeassistant_media_player::MediaPlayerSupportedFeature::VOLUME_UP:
//     case homeassistant_media_player::MediaPlayerSupportedFeature::VOLUME_DOWN:
//       circle_menu_->set_active_menu(volumeOptionMenu,
//                                     media_player_group_->active_player_);
//       break;
//     case homeassistant_media_player::MediaPlayerSupportedFeature::GROUPING:
//       menuIndex = 0;
//       menuTree.push_back(groupMenu);
//       break;
//     default:
//       break;
//   }
//   select_media_player_feature(active_feature);
//   update_display();
// }

// bool HomeThingMenuBase::select_media_player_feature(
//     homeassistant_media_player::MediaPlayerFeatureCommand* command) {
//   auto feature = command->get_feature();
//   switch (feature) {
//     case homeassistant_media_player::MediaPlayerSupportedFeature::MENU_HOME:
//       topMenu();
//       return true;
//     case homeassistant_media_player::MediaPlayerSupportedFeature::GROUPING:
//       menuIndex = 0;
//       menuTree.push_back(groupMenu);
//       return true;
//     case homeassistant_media_player::MediaPlayerSupportedFeature::
//         CUSTOM_COMMAND: {
//       auto feature_command = command->get_command();
//       if (feature_command != nullptr) {
//         feature_command->on_command();
//         return true;
//       }
//     }

//     default:
//       media_player_group_->call_feature(feature);
//       break;
//   }
//   return false;
// }

// bool HomeThingMenuBase::button_press_now_playing_option_continue(
//     CircleOptionMenuPosition position) {
//   if (circle_menu_->get_active_menu()) {
//     auto feature = circle_menu_->tap_option_menu(
//         position, media_player_group_->get_active_player());
//     if (feature) {
//       ESP_LOGI(
//           TAG,
//           "button_press_now_playing_option_continue: option menu selected %d",
//           feature->get_feature());
//       circle_menu_->clear_active_menu();
//       if (!select_media_player_feature(feature)) {
//         return false;
//       }
//       reload_menu_items_ = true;
//       update_display();
//       return false;
//     }
//     ESP_LOGW(
//         TAG,
//         "button_press_now_playing_option_continue: option menu NOT selected");
//     return false;
//   }
//   return true;
// }
// #endif

void HomeThingMenuBase::buttonPressSelect() {
  if (!button_press_and_continue())
    return;
  reload_menu_items_ = true;
  if (menu_settings_->get_mode() == MENU_MODE_ROTARY) {
    switch (menuTree.back()) {
      case lightsDetailMenu:
      case settingsMenu:
        if (editing_menu_item) {
          // deselect light if selected and stay in lightsDetailMenu
          editing_menu_item = false;
          reload_menu_items_ = true;
          update_display();
          return;
        }
        break;
        //       case nowPlayingMenu:
        // #ifdef USE_MEDIA_PLAYER_GROUP
        //         if (now_playing_control_ != nullptr)
        //           return now_playing_control_->buttonPressSelect();
        //           // if (!button_press_now_playing_option_continue(
        //           //         CircleOptionMenuPosition::CENTER))
        //           //   return;

        //           // switch (media_player_group_->active_player_->get_player_type()) {
        //           //   case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
        //           //     media_player_group_->sendActivePlayerRemoteCommand(
        //           //         homeassistant_media_player::MediaPlayerTVRemoteCommand::SELECT);
        //           //     break;
        //           //   case homeassistant_media_player::RemotePlayerType::
        //           //       SpeakerRemotePlayerType:
        //           //     break;
        //           // }
        //           // if (menu_display_->get_draw_now_playing_menu()) {
        //           //   selectNowPlayingMenu();
        //           // }
        // #endif
        //         return;
      default:
        break;
    }
  } else {
    // 3 button
    switch (menuTree.back()) {
      case lightsDetailMenu:
#ifdef USE_LIGHT
        if (editing_menu_item) {
          // deselect light if selected and stay in lightsDetailMenu
          editing_menu_item = false;
          reload_menu_items_ = true;
          update_display();
          return;
        }
#endif
        break;
        //       case nowPlayingMenu:
        // #ifdef USE_MEDIA_PLAYER_GROUP
        //         if (now_playing_control_ != nullptr)
        //           return now_playing_control_->buttonPressSelect();
        //           // selectNowPlayingMenu();
        // #endif
        //         return;
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
  if (menuTree.back() == bootMenu) {
    return;
  }
  if (selectMenuHold()) {
    reload_menu_items_ = true;
    update_display();
  }
}

bool HomeThingMenuBase::upMenu() {
  if (menuTree.back() == lightsDetailMenu) {
    if (active_menu_screen)
      active_menu_screen->set_selected_entity(nullptr);
  }
  if (menuTree.size() > 1) {
    menuTree.pop_back();
    menuIndex = 0;
    reload_menu_items_ = true;
    menu_titles.clear();
    update_display();
    return true;
  }
  return false;
}

void HomeThingMenuBase::rotaryScrollCounterClockwise(int rotary) {
  if (!button_press_and_continue())
    return;
  rotary_ = rotary;
  if (menuIndex == 0 && menu_settings_->get_menu_rollback()) {
    // && menuTree.back() != nowPlayingMenu) {
    upMenu();
    return;
  }
  if (menu_settings_->get_mode() == MENU_MODE_ROTARY) {
    switch (menuTree.back()) {
        //       case nowPlayingMenu:
        // #ifdef USE_MEDIA_PLAYER_GROUP
        //         if (now_playing_control_ != nullptr)
        //           return now_playing_control_->rotaryScrollCounterClockwise(rotary);
        //           // if (menu_display_->get_draw_now_playing_menu()) {
        //           //   break;
        //           // }
        //           // media_player_group_->decreaseSpeakerVolume();
        //           // circle_menu_->set_active_menu(volumeOptionMenu,
        //           //                               media_player_group_->active_player_);
        //           // debounceUpdateDisplay();
        // #endif
        //         return;
      case lightsDetailMenu:
#ifdef USE_LIGHT
        if (HomeThingMenuControls::editingScrollBack(
                active_menu_screen->get_selected_entity(), menuIndex,
                editing_menu_item)) {
          reload_menu_items_ = true;
          debounceUpdateDisplay();
          return;
        }
#endif
      case settingsMenu:
        if (editing_menu_item && active_menu_screen &&
            active_menu_screen->get_selected_entity() &&
            HomeThingMenuControls::editingScrollBack(
                active_menu_screen->get_selected_entity(), menuIndex,
                editing_menu_item)) {
          reload_menu_items_ = true;
          debounceUpdateDisplay();
          return;
        }
        break;
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
    switch (menuTree.back()) {
#ifdef USE_LIGHT
      case lightsDetailMenu:
        if (HomeThingMenuControls::editingScrollBack(
                active_menu_screen->get_selected_entity(), menuIndex,
                editing_menu_item)) {
          debounceUpdateDisplay();
          return;
        }
#endif
      default:
        break;
    }

    if (menuIndex > 0) {
      menuIndex--;
      // } else if (menuTree.back() == nowPlayingMenu) {
      //   menuIndex = menu_titles.size() - 1;
    }
  }
  debounceUpdateDisplay();
}

void HomeThingMenuBase::rotaryScrollClockwise(int rotary) {
  if (menu_settings_->get_mode() == MENU_MODE_3_BUTTON && skipBootPressed())
    return;
  if (!button_press_and_continue())
    return;
  rotary_ = rotary;
  if (menu_settings_->get_mode() == MENU_MODE_ROTARY) {
    switch (menuTree.back()) {
        //       case nowPlayingMenu:
        // #ifdef USE_MEDIA_PLAYER_GROUP
        //         if (now_playing_control_ != nullptr)
        //           return now_playing_control_->rotaryScrollClockwise(rotary);
        //           // if (menu_display_->get_draw_now_playing_menu()) {
        //           //   break;
        //           // }
        //           // media_player_group_->increaseSpeakerVolume();
        //           // circle_menu_->set_active_menu(volumeOptionMenu,
        //           //                               media_player_group_->active_player_);
        //           // debounceUpdateDisplay();
        // #endif
        //         return;
      case lightsDetailMenu:
#ifdef USE_LIGHT
        if (HomeThingMenuControls::editingScrollForward(
                active_menu_screen->get_selected_entity(), menuIndex,
                editing_menu_item)) {
          reload_menu_items_ = true;
          debounceUpdateDisplay();
          return;
        }
#endif
      case settingsMenu:
        if (editing_menu_item && active_menu_screen &&
            active_menu_screen->get_selected_entity() &&
            HomeThingMenuControls::editingScrollForward(
                active_menu_screen->get_selected_entity(), menuIndex,
                editing_menu_item)) {
          reload_menu_items_ = true;
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
    switch (menuTree.back()) {
#ifdef USE_LIGHT
      case lightsDetailMenu:
        if (HomeThingMenuControls::editingScrollForward(
                active_menu_screen->get_selected_entity(), menuIndex,
                editing_menu_item)) {
          debounceUpdateDisplay();
          return;
        }
#endif
      default:
        break;
    }

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
  switch (menuTree.back()) {
      //     case nowPlayingMenu:
      // #ifdef USE_MEDIA_PLAYER_GROUP
      //       if (now_playing_control_ != nullptr)
      //         return now_playing_control_->buttonPressUp();
      //         // if (!button_press_now_playing_option_continue(
      //         //         CircleOptionMenuPosition::TOP))
      //         //   return;

      //         // switch (media_player_group_->active_player_->get_player_type()) {
      //         //   case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
      //         //     media_player_group_->sendActivePlayerRemoteCommand(
      //         //         homeassistant_media_player::MediaPlayerTVRemoteCommand::UP);
      //         //     return;
      //         //   case homeassistant_media_player::RemotePlayerType::
      //         //       SpeakerRemotePlayerType:
      //         //     break;
      //         // }
      // #endif
      //       break;
      //     case groupMenu:
      // #ifdef USE_MEDIA_PLAYER_GROUP
      //       menuIndex = 0;
      //       if (media_player_group_->newSpeakerGroupParent != NULL) {
      //         media_player_group_->newSpeakerGroupParent = NULL;
      //       } else {
      //         circle_menu_->clear_active_menu();
      //       }
      //       topMenu();
      //       update_display();
      // #endif
      //       return;
    case lightsDetailMenu:
#ifdef USE_LIGHT
      if (editing_menu_item) {
        // deselect light if selected and stay in lightsDetailMenu
        editing_menu_item = false;
        reload_menu_items_ = true;
        update_display();
        return;
      }
      // if no light is selected go up menu
      if (upMenu())
        return;
#endif
      break;
    default:
    case settingsMenu:
      if (editing_menu_item) {
        editing_menu_item = false;
        reload_menu_items_ = true;
        update_display();
        return;
      }
      break;
  }
  // if (option_menu_ == speakerOptionMenu) {
  //   media_player_group_->toggle_shuffle();
  //   option_menu_ = noOptionMenu;
  //   update_display();
  //   return;
  // }
  // option_menu_ = noOptionMenu;
  if (active_menu_screen)
    active_menu_screen->set_selected_entity(nullptr);
  topMenu();
  update_display();
}

void HomeThingMenuBase::buttonPressDown() {
  if (!button_press_and_continue())
    return;
  switch (menuTree.back()) {
    //     case nowPlayingMenu:
    // #ifdef USE_MEDIA_PLAYER_GROUP
    //       if (now_playing_control_ != nullptr)
    //         return now_playing_control_->buttonPressDown();
    //         // // if (option_menu_ == tvOptionMenu) {
    //         // if (!button_press_now_playing_option_continue(
    //         //         CircleOptionMenuPosition::BOTTOM))
    //         //   return;

    //         // switch (media_player_group_->active_player_->get_player_type()) {
    //         //   case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
    //         //     media_player_group_->sendActivePlayerRemoteCommand(
    //         //         homeassistant_media_player::MediaPlayerTVRemoteCommand::DOWN);
    //         //     return;
    //         //   case homeassistant_media_player::RemotePlayerType::
    //         //       SpeakerRemotePlayerType:
    //         //     media_player_group_->active_player_->playPause();
    //         // }
    // #endif
    break;
    default:
      break;
  }
}

void HomeThingMenuBase::buttonPressLeft() {
  if (!button_press_and_continue())
    return;
  switch (menuTree.back()) {
    //     case nowPlayingMenu:
    // #ifdef USE_MEDIA_PLAYER_GROUP

    //       if (now_playing_control_ != nullptr)
    //         return now_playing_control_->buttonPressLeft();
    //         // // if (option_menu_ == tvOptionMenu) {
    //         // if (!button_press_now_playing_option_continue(
    //         //         CircleOptionMenuPosition::LEFT))
    //         //   return;
    //         // // option_menu_ = noOptionMenu;
    //         // // media_player_group_->sendActivePlayerRemoteCommand("back");
    //         // // update_display();
    //         // // return;
    //         // // }

    //         // switch (media_player_group_->active_player_->get_player_type()) {
    //         //   case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
    //         //     media_player_group_->sendActivePlayerRemoteCommand(
    //         //         homeassistant_media_player::MediaPlayerTVRemoteCommand::LEFT);
    //         //     return;
    //         //   case homeassistant_media_player::RemotePlayerType::
    //         //       SpeakerRemotePlayerType:
    //         //     circle_menu_->clear_active_menu();
    //         //     break;
    //         // }
    // #endif
    break;
    default:
      break;
  }
}

bool HomeThingMenuBase::skipBootPressed() {
  switch (menuTree.back()) {
    case bootMenu: {
      switch (menu_display_->boot_->bootSequenceCanSkip(menuTree.back())) {
        case BOOT_MENU_SKIP_STATE_SLEEP:
          ESP_LOGI(TAG, "skipBootPressed: sleep");
#ifdef USE_SWITCH
          sleep_switch_->turn_on();
#endif
          return true;
        case BOOT_MENU_SKIP_STATE_MENU:
          ESP_LOGI(TAG, "skipBootPressed: menu");
          // #ifdef USE_MEDIA_PLAYER_GROUP
          //           media_player_group_->selectFirstActivePlayer();
          // #endif
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
  switch (menuTree.back()) {
    //     case nowPlayingMenu:
    // #ifdef USE_MEDIA_PLAYER_GROUP
    //       if (now_playing_control_ != nullptr)
    //         return now_playing_control_->buttonPressRight();
    //         // // if (option_menu_ == tvOptionMenu) {
    //         // if (!button_press_now_playing_option_continue(
    //         //         CircleOptionMenuPosition::RIGHT))
    //         //   return;
    //         // // option_menu_ = noOptionMenu;
    //         // // media_player_group_->sendActivePlayerRemoteCommand("menu");
    //         // // update_display();
    //         // // return;
    //         // // }
    //         // switch (media_player_group_->active_player_->get_player_type()) {
    //         //   case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
    //         //     media_player_group_->sendActivePlayerRemoteCommand(
    //         //         homeassistant_media_player::MediaPlayerTVRemoteCommand::RIGHT);
    //         //     return;
    //         //   case homeassistant_media_player::RemotePlayerType::
    //         //       SpeakerRemotePlayerType:
    //         //     // if (option_menu_ == speakerOptionMenu) {
    //         //     //   media_player_group_->toggle_mute();
    //         //     //   update_display();
    //         //     // } else {
    //         //     media_player_group_->active_player_->nextTrack();
    //         //     // }
    //         //     break;
    //         // }
    // #endif
    break;
    default:
      break;
  }
}

void HomeThingMenuBase::buttonReleaseScreenLeft() {
  if (!button_press_and_continue())
    return;
  switch (menuTree.back()) {
      //     case nowPlayingMenu:
      // #ifdef USE_MEDIA_PLAYER_GROUP
      //       if (now_playing_control_ != nullptr)
      //         return now_playing_control_->buttonReleaseScreenLeft();
      //         // switch (media_player_group_->active_player_->get_player_type()) {
      //         //   case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
      //         //     update_display();
      //         //     break;
      //         //   case homeassistant_media_player::RemotePlayerType::
      //         //       SpeakerRemotePlayerType:
      //         //     break;
      //         // }
      // #endif
      //       break;
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
  switch (menuTree.back()) {
      //     case nowPlayingMenu:
      // #ifdef USE_MEDIA_PLAYER_GROUP
      //       if (now_playing_control_ != nullptr)
      //         return now_playing_control_->buttonPressScreenLeft();
      //         // if (circle_menu_->get_active_menu()) {
      //         //   circle_menu_->clear_active_menu();
      //         // } else {
      //         //   circle_menu_->set_active_menu(speakerOptionMenu,
      //         //                                 media_player_group_->active_player_);
      //         // }
      // #endif
      //       update_display();
      //       break;
    default:
      break;
  }
}

void HomeThingMenuBase::buttonPressScreenRight() {
  if (!button_press_and_continue())
    return;
  // #ifdef USE_MEDIA_PLAYER_GROUP
  //   circle_menu_->clear_active_menu();
  // #endif
  switch (menuTree.back()) {
    case rootMenu:
    case settingsMenu:
      //     case nowPlayingMenu:
      // #ifdef USE_MEDIA_PLAYER_GROUP
      //       if (now_playing_control_ != nullptr)
      //         return now_playing_control_->buttonPressScreenRight();
      //         // media_player_group_->selectNextMediaPlayer();
      // #endif
      //       update_display();
      //       break;
      //     case sourcesMenu:
      //     case groupMenu:
      //     case mediaPlayersMenu:
    case lightsDetailMenu:
    case bootMenu:
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

bool HomeThingMenuBase::display_can_fade_out() {
  int display_timeout_while_charging =
      menu_settings_->get_display_timeout_while_charging();
  if (get_charging()) {
    if (display_timeout_while_charging > 0)
      return idleTime == display_timeout_while_charging - 4;
    else
      return false;
  }
  return idleTime == menu_settings_->get_display_timeout() - 4;
}

bool HomeThingMenuBase::display_can_sleep() {
  int timeout = menu_settings_->get_display_timeout();
  bool idle_timeout = timeout != 0 && idleTime >= timeout;

  int display_timeout_while_charging =
      menu_settings_->get_display_timeout_while_charging();

  ESP_LOGI(TAG,
           "screen timeout %d, charging %d, display_timeout_while_charging %d "
           "idle %d",
           idle_timeout, get_charging(), display_timeout_while_charging,
           idleTime);

  if (get_charging()) {
    if (display_timeout_while_charging > 0)
      return idleTime >= display_timeout_while_charging;
    else
      return false;
  } else {
    return idle_timeout;
  }
}

void HomeThingMenuBase::turn_on_backlight() {
#ifdef USE_LIGHT
  if (backlight_ && !backlight_->remote_values.is_on()) {
    ESP_LOGI(TAG, "turn_on_backlight: turning on display");
    backlight_->turn_on()
        .set_transition_length(250)
        .set_brightness(1)
        .perform();
  }
#endif
}

void HomeThingMenuBase::sleep_display() {
#ifdef USE_LIGHT
  if (backlight_ != nullptr &&
      (backlight_->remote_values.is_on() ||
       backlight_->remote_values.get_brightness() > 0)) {
    ESP_LOGI(TAG, "sleep_display: turning off display %d, %d, %f",
             backlight_ == nullptr, backlight_->remote_values.is_on(),
             backlight_->remote_values.get_brightness());
    backlight_->turn_off()
        .set_brightness(0)
        .set_transition_length(500)
        .perform();
  } else {
    ESP_LOGI(TAG, "sleep_display: NOT turning off display %d, %d, %f",
             backlight_ == nullptr, backlight_->remote_values.is_on(),
             backlight_->remote_values.get_brightness());
  }
#endif
}

void HomeThingMenuBase::fade_out_display() {
#ifdef USE_LIGHT
  auto brightness = backlight_->remote_values.get_brightness();
  if (backlight_ != nullptr && backlight_->remote_values.is_on() &&
      brightness > 0.3f) {
    ESP_LOGI(TAG, "fade_out_display: fading out display %d, %d, %f",
             backlight_ == nullptr, backlight_->remote_values.is_on(),
             brightness);
    backlight_->turn_on()
        .set_brightness(0.3)
        .set_transition_length(500)
        .perform();
  }
#endif
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
  if (menuTree.back() == bootMenu) {
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
    // #ifdef USE_MEDIA_PLAYER_GROUP
    //     circle_menu_->clear_active_menu();
    // #endif
    update_display();
  } else if (display_can_fade_out()) {
    fade_out_display();
  } else if (idleTime == menu_settings_->get_display_timeout()) {
    // #ifdef USE_MEDIA_PLAYER_GROUP
    // if (media_player_group_ != NULL &&
    //     media_player_group_->playerSearchFinished) {
    //   if (get_charging() && menuTree.back() != bootMenu) {
    //     idleTime++;
    //     return;
    // }
    //   ESP_LOGI(TAG, "idleTick: idle root menu %d", display_can_sleep());
    //   menuTree.assign(1, rootMenu);
    //   animation_->resetAnimation();
    //   idleMenu(false);
    //   menu_display_->updateDisplay(false);
    // }
    // #endif

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
#ifdef USE_SWITCH
    if (!get_charging() && sleep_switch_) {
      ESP_LOGD(TAG, "idleTick: night night");
      sleep_switch_->turn_on();
      return;
    }
#endif
  }
  // #ifdef USE_MEDIA_PLAYER_GROUP
  //   if (media_player_group_ != NULL) {
  //     bool updatedMediaPositions = media_player_group_->updateMediaPosition();
  //     if (updatedMediaPositions) {
  //       switch (menuTree.back()) {
  //         // case nowPlayingMenu: {
  //         //   ESP_LOGD(TAG, "idleTick: update media positions %d",
  //         //            display_can_sleep());
  //         //   if (!display_can_sleep()) {
  //         //     update_display();
  //         //   } else {
  //         //     if (!get_charging())
  //         //       sleep_display();
  //         //   }
  //         //   break;
  //         // }
  //         default:
  //           break;
  //       }
  //     }
  //   }
  // #endif
  idleTime++;
}

void HomeThingMenuBase::goToScreenFromString(std::string screenName) {
  ESP_LOGI(TAG, "goToScreenFromString: %s", screenName.c_str());
  if (screenName == "lightDetail") {
    menuTree.push_back(lightsDetailMenu);
  } else if (screenName == "home") {
    menuTree.assign(1, rootMenu);
  } else if (screenName == "boot") {
    menuTree.push_back(bootMenu);
    // } else if (screenName == "nowPlaying") {
    //   menuTree.push_back(nowPlayingMenu);
    // } else if (screenName == "sources") {
    //   menuTree.push_back(sourcesMenu);
    // } else if (screenName == "mediaPlayers") {
    //   menuTree.push_back(mediaPlayersMenu);
    // } else if (screenName == "speakerGroup") {
    //   menuTree.push_back(groupMenu);
  }
  reset_menu();
  update_display();
}

void HomeThingMenuBase::idleMenu(bool force) {
  ESP_LOGI(TAG, "idleMenu %d", force);
  if (menuTree.back() == bootMenu) {
    return;
  }
  if (!get_charging() || force) {
    if (active_menu_screen)
      active_menu_screen->set_selected_entity(nullptr);
    reset_menu();
    // #ifdef USE_MEDIA_PLAYER_GROUP
    //     menuTree.push_back(nowPlayingMenu);
    //     if (media_player_group_)
    //       media_player_group_->newSpeakerGroupParent = NULL;
    // #endif
    if (force) {
      update_display();
    }
  }
}
}  // namespace homething_menu_base
}  // namespace esphome
