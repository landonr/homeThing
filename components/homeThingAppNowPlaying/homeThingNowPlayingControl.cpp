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
      new HomeThingMenuNowPlayingHeader(media_player_group, &menu_state_);
}

void HomeThingMenuNowPlayingControl::rootMenuTitles(
    std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {
  ESP_LOGI(TAG, "rootMenuTitles null %d", media_player_group_ == nullptr);
  if (media_player_group_ != nullptr) {
    std::string title = get_menu_title(NOW_PLAYING_MENU_STATE_NOW_PLAYING);
    if (media_player_group_->active_player_ != nullptr) {
      title = media_player_group_->active_player_->get_name();
    }
    menu_titles->push_back(new homething_menu_base::MenuTitleBase(
        title, "", homething_menu_base::ArrowMenuTitleRightIcon));
    menu_titles->push_back(new homething_menu_base::MenuTitleBase(
        get_menu_title(NOW_PLAYING_MENU_STATE_SOURCE), "",
        homething_menu_base::ArrowMenuTitleRightIcon));
    if (media_player_group_->totalPlayers() > 1) {
      menu_titles->push_back(new homething_menu_base::MenuTitleBase(
          get_menu_title(NOW_PLAYING_MENU_STATE_MEDIA_PLAYERS), "",
          homething_menu_base::ArrowMenuTitleRightIcon));
    }
    if (media_player_group_->active_player_ != nullptr &&
        media_player_group_->active_player_->get_group_members() > 0) {
      menu_titles->push_back(new homething_menu_base::MenuTitleBase(
          get_menu_title(NOW_PLAYING_MENU_STATE_GROUPING), "",
          homething_menu_base::ArrowMenuTitleRightIcon));
    }
  }
}

void HomeThingMenuNowPlayingControl::app_menu_titles(
    std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {
  ESP_LOGI(TAG, "menu_titles null %d app menu index %d",
           media_player_group_ == nullptr, menu_state_);
  if (media_player_group_ != nullptr) {
    switch (menu_state_) {
      case NOW_PLAYING_MENU_STATE_NOW_PLAYING:
        return;
      case NOW_PLAYING_MENU_STATE_GROUPING:
        homeThingNowPlayingMenuGroup::active_menu(menu_titles,
                                                  media_player_group_);
        return;
      case NOW_PLAYING_MENU_STATE_SOURCE:
        homeThingNowPlayingMenuSources::sourceMenuTitles(menu_titles,
                                                         media_player_group_);
        return;
      case NOW_PLAYING_MENU_STATE_MEDIA_PLAYERS:
        homeThingNowPlayingMenuMediaPlayers::media_player_menu_titles(
            menu_titles, media_player_group_);
        return;
    }
  }
}

homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::app_menu_select(int index) {
  ESP_LOGI(TAG, "app_menu_select null %d app menu index %d",
           media_player_group_ == nullptr, menu_state_);
  switch (menu_state_) {
    case NOW_PLAYING_MENU_STATE_NOW_PLAYING:
      break;
    case NOW_PLAYING_MENU_STATE_GROUPING:
      return homeThingNowPlayingMenuGroup::select_menu(index,
                                                       media_player_group_);
    case NOW_PLAYING_MENU_STATE_SOURCE:
      homeThingNowPlayingMenuSources::select_source_menu(
          index, media_player_group_, circle_menu_, &menu_state_);
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationUpdate;
    case NOW_PLAYING_MENU_STATE_MEDIA_PLAYERS: {
      return homeThingNowPlayingMenuMediaPlayers::select_menu(
          index, media_player_group_);
    }
  }
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

int HomeThingMenuNowPlayingControl::root_menu_size() {
  if (media_player_group_ != nullptr) {
    bool show_source =
        media_player_group_->active_player_ != nullptr &&
        media_player_group_->active_player_->get_sources() != nullptr &&
        media_player_group_->active_player_->get_sources()->size() > 0;
    bool show_media_players = media_player_group_->totalPlayers() > 1;
    bool show_grouping =
        media_player_group_->active_player_ != nullptr &&
        media_player_group_->active_player_->get_group_members() > 0;
    return 1 + show_source + show_media_players + show_grouping;
  }
  return 0;
}

void HomeThingMenuNowPlayingControl::set_app_menu_index(int app_menu_index) {
  if (app_menu_index >= root_menu_size()) {
    return;
  }
  menu_state_ = NowPlayingMenuState(app_menu_index);
}

bool HomeThingMenuNowPlayingControl::should_draw_app() {
  switch (menu_state_) {
    case NOW_PLAYING_MENU_STATE_NOW_PLAYING:
      return true;
    case NOW_PLAYING_MENU_STATE_GROUPING:
    case NOW_PLAYING_MENU_STATE_SOURCE:
    case NOW_PLAYING_MENU_STATE_MEDIA_PLAYERS:
      return false;
  }
  return false;
}

void HomeThingMenuNowPlayingControl::draw_app(
    int menuIndex,
    const std::vector<homething_menu_base::MenuTitleBase*>* active_menu) {
  switch (menu_state_) {
    case NOW_PLAYING_MENU_STATE_NOW_PLAYING:
      now_playing_display_->drawNowPlaying(
          menuIndex, circle_menu_->get_active_menu(), active_menu);
      break;
    case NOW_PLAYING_MENU_STATE_GROUPING:
    case NOW_PLAYING_MENU_STATE_SOURCE:
    case NOW_PLAYING_MENU_STATE_MEDIA_PLAYERS:
      break;
  }
}

void HomeThingMenuNowPlayingControl::idleTick(int idleTime,
                                              int display_timeout) {
  ESP_LOGI(TAG, "idleTick: idle %d", idleTime);
  if (media_player_group_ != nullptr) {
    media_player_group_->findActivePlayer();
  }
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
  menu_state_ = NOW_PLAYING_MENU_STATE_NOW_PLAYING;
}

bool HomeThingMenuNowPlayingControl::select_media_player_feature(
    homeassistant_media_player::MediaPlayerFeatureCommand* command) {
  auto feature = command->get_feature();
  switch (feature) {
    case homeassistant_media_player::MediaPlayerSupportedFeature::MENU_HOME:
      // topMenu();
      return true;
    case homeassistant_media_player::MediaPlayerSupportedFeature::GROUPING:
      menu_state_ = NOW_PLAYING_MENU_STATE_GROUPING;
      return false;
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
  switch (menu_state_) {
    case NOW_PLAYING_MENU_STATE_NOW_PLAYING: {
      switch (button_press_now_playing_option(CircleOptionMenuPosition::TOP)) {
        case homething_menu_app::NavigationCoordination::
            NavigationCoordinationUpdate:
          return homething_menu_app::NavigationCoordination::
              NavigationCoordinationUpdate;
        case homething_menu_app::NavigationCoordination::
            NavigationCoordinationPop:
          return homething_menu_app::NavigationCoordination::
              NavigationCoordinationPop;
        default:
          break;
      }

      if (media_player_group_ == nullptr) {
        return homething_menu_app::NavigationCoordination::
            NavigationCoordinationPop;
      }

      switch (media_player_group_->active_player_->get_player_type()) {
        case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
          media_player_group_->sendActivePlayerRemoteCommand(
              homeassistant_media_player::MediaPlayerTVRemoteCommand::UP);
          return homething_menu_app::NavigationCoordination::
              NavigationCoordinationReturn;
        case homeassistant_media_player::RemotePlayerType::
            SpeakerRemotePlayerType:
          break;
      }
      break;
    }
    case NOW_PLAYING_MENU_STATE_GROUPING:
      return homeThingNowPlayingMenuGroup::pop_menu(media_player_group_);
    case NOW_PLAYING_MENU_STATE_SOURCE:
      return homeThingNowPlayingMenuSources::pop_menu(media_player_group_);
    case NOW_PLAYING_MENU_STATE_MEDIA_PLAYERS:
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