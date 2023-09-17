#include "homeThingMenuNowPlayingControl.h"

namespace esphome {
namespace homething_menu_now_playing {

homeassistant_media_player::HomeAssistantMediaPlayerGroup*
HomeThingMenuNowPlayingControl::get_media_player_group() {
  ESP_LOGI(TAG, "get_media_player_group null %d",
           media_player_group_ == nullptr);
  return media_player_group_;
}

void HomeThingMenuNowPlayingControl::set_media_player_group(
    homeassistant_media_player::HomeAssistantMediaPlayerGroup*
        media_player_group) {
  ESP_LOGI(TAG, "set_media_player_group null %d",
           media_player_group_ == nullptr);
  media_player_group_ = media_player_group;
}

void HomeThingMenuNowPlayingControl::rootMenuTitles(
    std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {
  ESP_LOGI(TAG, "rootMenuTitles null %d", media_player_group_ == nullptr);
  if (media_player_group_ != nullptr) {
    menu_titles->push_back(new homething_menu_base::MenuTitleBase(
        "Now Playing", "", homething_menu_base::NoMenuTitleRightIcon));
    menu_titles->push_back(new homething_menu_base::MenuTitleBase(
        "Sources", "", homething_menu_base::NoMenuTitleRightIcon));
    if (media_player_group_->totalPlayers() > 1) {
      menu_titles->push_back(new homething_menu_base::MenuTitleBase(
          "Media Players", "", homething_menu_base::NoMenuTitleRightIcon));
    }
  }
}

void HomeThingMenuNowPlayingControl::app_menu_titles(
    std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {
  ESP_LOGI(TAG, "menu_titles null %d app menu index %d",
           media_player_group_ == nullptr, app_menu_index_);
  if (media_player_group_ != nullptr) {
    if (app_menu_index_ == 0) {
      return;
    } else if (app_menu_index_ == 1) {
      sourceMenuTitles(menu_titles);
    } else if (app_menu_index_ == 2) {
      // media_player_group_->menuTitles(menu_titles);
    }
  }
}

void HomeThingMenuNowPlayingControl::select_source_menu(int index) {
  // const auto sourceTitleState = static_cast<MenuTitleSource*>(activeMenuTitle);
  // auto source = sourceTitleState->media_source_;
  auto player_source_index =
      media_player_group_->get_active_player_source_index();
  if (player_source_index == -1) {
    ESP_LOGI(TAG, "select_source_menu: set player source index %d", index);
    media_player_group_->set_active_player_source_index(index);
    return;
  }

  const auto sources = media_player_group_->activePlayerSources();
  if (sources->size() == 0) {
    return;
  }
  int active_player_source_index =
      sources->size() == 1 ? 0 : player_source_index;
  auto playerSources = (*sources)[active_player_source_index]->get_sources();
  media_player_source::MediaPlayerSourceItem* source = (*playerSources)[index];
  media_player_group_->playSource(source);
  ESP_LOGI(TAG, "select_source_menu: set player source index %d",
           active_player_source_index);
  // const auto new_source = new media_player_source::MediaPlayerSourceItem(
  //     source->get_name(), source->get_media_content_id(),
  //     source->get_media_type());
  // media_player_group_->playSource(new_source);
  // idleMenu(true);
  // circle_menu_->set_active_menu(playingNewSourceMenu,
  //                               media_player_group_->active_player_);
}

bool HomeThingMenuNowPlayingControl::app_menu_select(int index) {
  ESP_LOGI(TAG, "app_menu_select null %d app menu index %d",
           media_player_group_ == nullptr, app_menu_index_);
  switch (app_menu_index_) {
    case 0:
      break;
    case 1:
      select_source_menu(index);
      return true;
    case 2:
      // media_player_group_->selectMediaPlayer(index);
      break;
  }
  return false;
}

void HomeThingMenuNowPlayingControl::sourceMenuTitles(
    std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {
  if (media_player_group_ == nullptr) {
    return;
  }
  if (media_player_group_->active_player_ == nullptr) {
    media_player_group_->selectNextMediaPlayer();
    if (media_player_group_->active_player_ == nullptr) {
      return;
    }
  }
  const auto sources = media_player_group_->activePlayerSources();
  const auto index = media_player_group_->get_active_player_source_index();
  if (index == -1 && sources->size() > 1) {
    activePlayerSourceTitles(sources, menu_titles);
    return;
  } else if (index == -1 && sources->size() == 1) {
    auto playerSources = (*sources)[0]->get_sources();
    activePlayerSourceItemTitles(playerSources, menu_titles);
    return;
  } else if (sources->size() > 1) {
    auto playerSources = (*sources)[index]->get_sources();
    activePlayerSourceItemTitles(playerSources, menu_titles);
    return;
  }
}

int HomeThingMenuNowPlayingControl::root_menu_size() {
  if (media_player_group_ != nullptr) {
    if (media_player_group_->totalPlayers() > 1) {
      return 3;
    }
    return 2;
  }
  return 0;
}

void HomeThingMenuNowPlayingControl::set_app_menu_index(int app_menu_index) {
  if (app_menu_index_ >= root_menu_size()) {
    return;
  }
  app_menu_index_ = app_menu_index;
}

void HomeThingMenuNowPlayingControl::idleTick(int idleTime,
                                              int display_timeout) {
  ESP_LOGD(TAG, "idleTick: idle %d", idleTime);
  if (idleTime == 3) {
    circle_menu_->clear_active_menu();
  } else if (idleTime == display_timeout) {
    if (media_player_group_ != NULL &&
        media_player_group_->playerSearchFinished) {
      // if (get_charging() && menuTree.back() != bootMenu) {
      //   idleTime++;
      //   return;
      // }
      // ESP_LOGI(TAG, "idleTick: idle root menu %d", display_can_sleep());
      // menuTree.assign(1, rootMenu);
      // animation_->resetAnimation();
      // idleMenu(false);
      // menu_display_->updateDisplay(false);
    }
    return;
  }
  if (media_player_group_ != NULL) {
    bool updatedMediaPositions = media_player_group_->updateMediaPosition();
    if (updatedMediaPositions) {
      // switch (menuTree.back()) {
      //   case nowPlayingMenu: {
      //     ESP_LOGD(TAG, "idleTick: update media positions %d",
      //              display_can_sleep());
      //     if (!display_can_sleep()) {
      //       update_display();
      //     } else {
      //       if (!get_charging())
      //         sleep_display();
      //     }
      //     break;
      //   }
      //   default:
      //     break;
      // }
    }
  }
}

void HomeThingMenuNowPlayingControl::reset_menu() {
  if (media_player_group_) {
    media_player_group_->newSpeakerGroupParent = NULL;
    media_player_group_->set_active_player_source_index(-1);
  }
  app_menu_index_ = -1;
}

// void HomeThingMenuNowPlayingControl::activeMenu(std::vector<MenuTitleBase*>* menu_titles) {
//   switch (menuTree.back()) {
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
//   }
// }

void HomeThingMenuNowPlayingControl::rotaryScrollClockwise(int rotary) {
  //   if (menu_display_->get_draw_now_playing_menu()) {
  //     break;
  //   }
  media_player_group_->increaseSpeakerVolume();
  circle_menu_->set_active_menu(volumeOptionMenu,
                                media_player_group_->active_player_);
  // debounceUpdateDisplay();
}
void HomeThingMenuNowPlayingControl::rotaryScrollCounterClockwise(int rotary) {
  //   if (menu_display_->get_draw_now_playing_menu()) {
  //     break;
  //   }
  media_player_group_->decreaseSpeakerVolume();
  circle_menu_->set_active_menu(volumeOptionMenu,
                                media_player_group_->active_player_);
  // debounceUpdateDisplay();
}

void HomeThingMenuNowPlayingControl::buttonPressUp() {
  if (!button_press_now_playing_option_continue(CircleOptionMenuPosition::TOP))
    return;

  switch (media_player_group_->active_player_->get_player_type()) {
    case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
      media_player_group_->sendActivePlayerRemoteCommand(
          homeassistant_media_player::MediaPlayerTVRemoteCommand::UP);
      return;
    case homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType:
      break;
  }
}

void HomeThingMenuNowPlayingControl::buttonPressDown() {
  // if (option_menu_ == tvOptionMenu) {
  if (!button_press_now_playing_option_continue(
          CircleOptionMenuPosition::BOTTOM))
    return;

  switch (media_player_group_->active_player_->get_player_type()) {
    case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
      media_player_group_->sendActivePlayerRemoteCommand(
          homeassistant_media_player::MediaPlayerTVRemoteCommand::DOWN);
      return;
    case homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType:
      media_player_group_->active_player_->playPause();
  }
}

void HomeThingMenuNowPlayingControl::buttonPressLeft() {
  // if (option_menu_ == tvOptionMenu) {
  if (!button_press_now_playing_option_continue(CircleOptionMenuPosition::LEFT))
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
    case homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType:
      circle_menu_->clear_active_menu();
      break;
  }
}

void HomeThingMenuNowPlayingControl::buttonPressRight() {
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
    case homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType:
      // if (option_menu_ == speakerOptionMenu) {
      //   media_player_group_->toggle_mute();
      //   update_display();
      // } else {
      media_player_group_->active_player_->nextTrack();
      // }
      break;
  }
}

void HomeThingMenuNowPlayingControl::buttonPressSelect(int menuIndex) {
  // if (menu_titles.size() <= 0 && menuIndex < menu_titles.size()) {
  //   ESP_LOGI(TAG, "selectNowPlayingMenu: select menu %d", menuIndex);
  //   return;
  // }
  //   auto features = media_player_group_->active_player_->get_option_menu_features(
  //       menu_display_->get_draw_now_playing_menu());
  auto features =
      media_player_group_->active_player_->get_option_menu_features(true);
  auto active_feature = (*features)[menuIndex];
  ESP_LOGI(TAG, "selectNowPlayingMenu: %d, %s", menuIndex,
           active_feature->get_title().c_str());
  switch (active_feature->get_feature()) {
    case homeassistant_media_player::MediaPlayerSupportedFeature::VOLUME_SET:
    case homeassistant_media_player::MediaPlayerSupportedFeature::VOLUME_UP:
    case homeassistant_media_player::MediaPlayerSupportedFeature::VOLUME_DOWN:
      circle_menu_->set_active_menu(volumeOptionMenu,
                                    media_player_group_->active_player_);
      break;
    case homeassistant_media_player::MediaPlayerSupportedFeature::GROUPING:
      menuIndex = 0;
      // menuTree.push_back(groupMenu);
      break;
    default:
      break;
  }
  select_media_player_feature(active_feature);
  // update_display();
}

void HomeThingMenuNowPlayingControl::buttonPressSelectHold() {}

void HomeThingMenuNowPlayingControl::buttonPressScreenLeft() {
  if (circle_menu_->get_active_menu()) {
    circle_menu_->clear_active_menu();
  } else {
    circle_menu_->set_active_menu(speakerOptionMenu,
                                  media_player_group_->active_player_);
  }
}

void HomeThingMenuNowPlayingControl::buttonReleaseScreenLeft() {
  switch (media_player_group_->active_player_->get_player_type()) {
    case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
      // update_display();
      break;
    case homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType:
      break;
  }
}

void HomeThingMenuNowPlayingControl::buttonPressScreenRight() {
  media_player_group_->selectNextMediaPlayer();
}

}  // namespace homething_menu_now_playing
}  // namespace esphome