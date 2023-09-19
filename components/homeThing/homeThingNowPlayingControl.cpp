#include "homeThingNowPlayingControl.h"

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
  header_source_ =
      new HomeThingMenuNowPlayingHeader(media_player_group, &app_menu_index_);
}

void HomeThingMenuNowPlayingControl::rootMenuTitles(
    std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {
  ESP_LOGI(TAG, "rootMenuTitles null %d", media_player_group_ == nullptr);
  if (media_player_group_ != nullptr) {
    std::string title = "Now Playing";
    if (media_player_group_->active_player_ != nullptr) {
      title = media_player_group_->active_player_->get_name();
    }
    menu_titles->push_back(new homething_menu_base::MenuTitleBase(
        title, "", homething_menu_base::NoMenuTitleRightIcon));
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
      media_player_menu_titles(menu_titles);
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
  circle_menu_->set_active_menu(playingNewSourceMenu,
                                media_player_group_->active_player_);
  app_menu_index_ = 0;
}

homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::app_menu_select(int index) {
  ESP_LOGI(TAG, "app_menu_select null %d app menu index %d",
           media_player_group_ == nullptr, app_menu_index_);
  switch (app_menu_index_) {
    case 0:
      break;
    case 1:
      select_source_menu(index);
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationUpdate;
    case 2: {
      auto media_players = media_player_group_->get_media_players();
      if (media_players->size() == 0 || media_players->size() <= index) {
        return homething_menu_app::NavigationCoordination::
            NavigationCoordinationNone;
      }
      media_player_group_->selectMediaPlayers((*media_players)[index]);
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationPop;
    }
  }
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
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

void HomeThingMenuNowPlayingControl::media_player_menu_titles(
    std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {

  std::map<
      homeassistant_media_player::HomeAssistantBaseMediaPlayer*,
      std::vector<homeassistant_media_player::HomeAssistantBaseMediaPlayer*>>
      tree;

  for (const auto media_player : (*media_player_group_->get_media_players())) {
    auto parent = media_player->get_parent_media_player();
    auto groupMembers = media_player->get_group_members();
    ESP_LOGD(
        TAG,
        "mediaPlayersTitleString: player %s parent set %d group members %d",
        media_player->get_entity_id().c_str(), parent != NULL,
        groupMembers->size());
    if (parent != NULL) {
      // ignore parent if its a soundbar because the tv is the root parent
      if (parent->mediaSource ==
              homeassistant_media_player::RemotePlayerMediaSource::
                  TVRemotePlayerMediaSource &&
          parent->get_parent_media_player() != NULL) {
        auto grand_parent = parent->get_parent_media_player();
        // grand parent is tv, parent is soundbar, mediaplayer is grouped speaker
        if (tree.find(grand_parent) == tree.end()) {
          ESP_LOGD(TAG,
                   "mediaPlayersTitleString: adding grandparent1 %s player %s",
                   grand_parent->get_entity_id().c_str(),
                   media_player->get_entity_id().c_str());
          tree[grand_parent] = {media_player};
        } else {
          ESP_LOGD(TAG,
                   "mediaPlayersTitleString: adding grandparent2 %s player %s",
                   grand_parent->get_entity_id().c_str(),
                   media_player->get_entity_id().c_str());
          tree[grand_parent].push_back(media_player);
        }
      } else if (tree.find(parent) == tree.end()) {
        // dont add soundbar to tv if it's not playing
        if (parent->get_player_type() ==
                homeassistant_media_player::RemotePlayerType::
                    TVRemotePlayerType &&
            media_player->mediaSource !=
                homeassistant_media_player::RemotePlayerMediaSource::
                    TVRemotePlayerMediaSource) {
          ESP_LOGD(TAG, "mediaPlayersTitleString: adding player %s no parent",
                   parent->get_entity_id().c_str(),
                   media_player->get_entity_id().c_str());
          tree[media_player] = {};
        } else {
          ESP_LOGD(TAG, "mediaPlayersTitleString: adding parent2 %s player %s",
                   parent->get_entity_id().c_str(),
                   media_player->get_entity_id().c_str());
          tree[parent] = {media_player};
        }
      } else {
        ESP_LOGD(TAG, "mediaPlayersTitleString: adding parent3 %s player %s",
                 parent->get_entity_id().c_str(),
                 media_player->get_entity_id().c_str());
        tree[parent].push_back(media_player);
      }
    } else {
      if (tree.find(media_player) == tree.end()) {
        ESP_LOGD(TAG, "mediaPlayersTitleString: player set %s",
                 media_player->get_entity_id().c_str());
        tree[media_player] = {};
      }
    }
  }
  ESP_LOGD(TAG, "mediaPlayersTitleString: ------");
  for (auto tree_item : tree) {
    auto parent = tree_item.first;
    menu_titles->push_back(new homething_menu_base::MenuTitlePlayer(
        parent->get_name(), parent->get_entity_id(),
        homething_menu_base::NoMenuTitleLeftIcon,
        homething_menu_base::NoMenuTitleRightIcon, parent));
    for (const auto media_player : tree_item.second) {
      menu_titles->push_back(new homething_menu_base::MenuTitlePlayer(
          media_player->get_name(), media_player->get_entity_id(),
          homething_menu_base::GroupedMenuTitleLeftIcon,
          homething_menu_base::NoMenuTitleRightIcon, media_player));
    }
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

bool HomeThingMenuNowPlayingControl::should_draw_app() {
  if (app_menu_index_ == 0) {
    return true;
  }
  return false;
}

void HomeThingMenuNowPlayingControl::draw_app(
    int menuIndex,
    const std::vector<homething_menu_base::MenuTitleBase*>* active_menu) {
  switch (app_menu_index_) {
    case 0:
      now_playing_display_->drawNowPlaying(
          menuIndex, circle_menu_->get_active_menu(), active_menu);
      break;
    default:
      break;
  }
}

void HomeThingMenuNowPlayingControl::idleTick(int idleTime,
                                              int display_timeout) {
  ESP_LOGI(TAG, "idleTick: idle %d", idleTime);
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

bool HomeThingMenuNowPlayingControl::select_media_player_feature(
    homeassistant_media_player::MediaPlayerFeatureCommand* command) {
  auto feature = command->get_feature();
  switch (feature) {
    case homeassistant_media_player::MediaPlayerSupportedFeature::MENU_HOME:
      // topMenu();
      return true;
    case homeassistant_media_player::MediaPlayerSupportedFeature::GROUPING:
      // menuIndex = 0;
      // menuTree.push_back(groupMenu);
      return true;
    case homeassistant_media_player::MediaPlayerSupportedFeature::
        CUSTOM_COMMAND: {
      auto feature_command = command->get_command();
      if (feature_command != nullptr) {
        feature_command->on_command();
        return true;
      }
    }

    default:
      media_player_group_->call_feature(feature);
      break;
  }
  return false;
}

homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::button_press_now_playing_option(
    CircleOptionMenuPosition position) {
  if (circle_menu_->get_active_menu()) {
    auto feature = circle_menu_->tap_option_menu(
        position, media_player_group_->get_active_player());
    if (feature) {
      ESP_LOGI(TAG, "button_press_now_playing_option: option menu selected %d",
               feature->get_feature());
      circle_menu_->clear_active_menu();
      if (select_media_player_feature(feature)) {
        return homething_menu_app::NavigationCoordination::
            NavigationCoordinationPop;
      }
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationUpdate;
    }
    ESP_LOGW(TAG, "button_press_now_playing_option: option menu NOT selected");
    return homething_menu_app::NavigationCoordination::
        NavigationCoordinationNone;
  }
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

// MARK: Buttons

void HomeThingMenuNowPlayingControl::rotaryScrollClockwise(int rotary) {
  //   if (menu_display_->get_draw_now_playing_menu()) {
  //     break;
  //   }
  media_player_group_->increaseSpeakerVolume();
  circle_menu_->set_active_menu(volumeOptionMenu,
                                media_player_group_->active_player_);
}
void HomeThingMenuNowPlayingControl::rotaryScrollCounterClockwise(int rotary) {
  //   if (menu_display_->get_draw_now_playing_menu()) {
  //     break;
  //   }
  media_player_group_->decreaseSpeakerVolume();
  circle_menu_->set_active_menu(volumeOptionMenu,
                                media_player_group_->active_player_);
}

homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::buttonPressUp() {
  switch (button_press_now_playing_option(CircleOptionMenuPosition::TOP)) {
    case homething_menu_app::NavigationCoordination::
        NavigationCoordinationUpdate:
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationUpdate;
    case homething_menu_app::NavigationCoordination::NavigationCoordinationPop:
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationPop;
    default:
      break;
  }

  switch (media_player_group_->active_player_->get_player_type()) {
    case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
      media_player_group_->sendActivePlayerRemoteCommand(
          homeassistant_media_player::MediaPlayerTVRemoteCommand::UP);
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationReturn;
    case homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType:
      break;
  }
  return homething_menu_app::NavigationCoordination::NavigationCoordinationPop;
}

homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::buttonPressDown() {
  switch (button_press_now_playing_option(CircleOptionMenuPosition::BOTTOM)) {
    case homething_menu_app::NavigationCoordination::
        NavigationCoordinationUpdate:
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationUpdate;
    case homething_menu_app::NavigationCoordination::NavigationCoordinationPop:
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationPop;
    default:
      break;
  }

  switch (media_player_group_->active_player_->get_player_type()) {
    case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
      media_player_group_->sendActivePlayerRemoteCommand(
          homeassistant_media_player::MediaPlayerTVRemoteCommand::DOWN);
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationReturn;
    case homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType:
      media_player_group_->active_player_->playPause();
      break;
  }
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::buttonPressLeft() {
  switch (button_press_now_playing_option(CircleOptionMenuPosition::LEFT)) {
    case homething_menu_app::NavigationCoordination::
        NavigationCoordinationUpdate:
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationUpdate;
    case homething_menu_app::NavigationCoordination::NavigationCoordinationPop:
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationPop;
    default:
      break;
  }

  switch (media_player_group_->active_player_->get_player_type()) {
    case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
      media_player_group_->sendActivePlayerRemoteCommand(
          homeassistant_media_player::MediaPlayerTVRemoteCommand::LEFT);
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationReturn;
    case homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType:
      circle_menu_->clear_active_menu();
      break;
  }
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::buttonPressRight() {
  switch (button_press_now_playing_option(CircleOptionMenuPosition::RIGHT)) {
    case homething_menu_app::NavigationCoordination::
        NavigationCoordinationUpdate:
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationUpdate;
    case homething_menu_app::NavigationCoordination::NavigationCoordinationPop:
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationPop;
    default:
      break;
  }

  switch (media_player_group_->active_player_->get_player_type()) {
    case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
      media_player_group_->sendActivePlayerRemoteCommand(
          homeassistant_media_player::MediaPlayerTVRemoteCommand::RIGHT);
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationReturn;
    case homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType:
      media_player_group_->active_player_->nextTrack();
      break;
  }
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::buttonPressSelect(int menuIndex) {
  switch (button_press_now_playing_option(CircleOptionMenuPosition::CENTER)) {
    case homething_menu_app::NavigationCoordination::
        NavigationCoordinationUpdate:
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationUpdate;
    case homething_menu_app::NavigationCoordination::NavigationCoordinationPop:
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationPop;
    default:
      break;
  }
  switch (media_player_group_->active_player_->get_player_type()) {
    case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
      media_player_group_->sendActivePlayerRemoteCommand(
          homeassistant_media_player::MediaPlayerTVRemoteCommand::SELECT);
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationReturn;
    case homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType:
      break;
  }
  // auto features =
  //     media_player_group_->active_player_->get_option_menu_features(true);
  // auto active_feature = (*features)[menuIndex];
  // ESP_LOGI(TAG, "selectNowPlayingMenu: %d, %s", menuIndex,
  //          active_feature->get_title().c_str());
  // switch (active_feature->get_feature()) {
  //   case homeassistant_media_player::MediaPlayerSupportedFeature::VOLUME_SET:
  //   case homeassistant_media_player::MediaPlayerSupportedFeature::VOLUME_UP:
  //   case homeassistant_media_player::MediaPlayerSupportedFeature::VOLUME_DOWN:
  //     circle_menu_->set_active_menu(volumeOptionMenu,
  //                                   media_player_group_->active_player_);
  //     break;
  //   case homeassistant_media_player::MediaPlayerSupportedFeature::GROUPING:
  //     menuIndex = 0;
  //     // menuTree.push_back(groupMenu);
  //     break;
  //   default:
  //     break;
  // }
  // select_media_player_feature(active_feature);
  return homething_menu_app::NavigationCoordination::
      NavigationCoordinationUpdate;
}

homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::buttonPressSelectHold() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::buttonPressScreenLeft() {
  if (circle_menu_->get_active_menu()) {
    circle_menu_->clear_active_menu();
  } else {
    circle_menu_->set_active_menu(speakerOptionMenu,
                                  media_player_group_->active_player_);
  }
  return homething_menu_app::NavigationCoordination::
      NavigationCoordinationUpdate;
}

homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::buttonReleaseScreenLeft() {
  switch (media_player_group_->active_player_->get_player_type()) {
    case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
      // update_display();
      break;
    case homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType:
      break;
  }
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::buttonPressScreenRight() {
  media_player_group_->selectNextMediaPlayer();
  return homething_menu_app::NavigationCoordination::
      NavigationCoordinationUpdate;
}

}  // namespace homething_menu_now_playing
}  // namespace esphome