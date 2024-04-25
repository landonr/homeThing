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
  media_player_group_->add_on_state_callback(
      [this]() { this->callback_.call(); });
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
    if (draw_source()) {
      menu_titles->push_back(new homething_menu_base::MenuTitleBase(
          get_menu_title(NOW_PLAYING_MENU_STATE_SOURCE), "",
          homething_menu_base::ArrowMenuTitleRightIcon));
    }
    if (draw_media_players()) {
      menu_titles->push_back(new homething_menu_base::MenuTitleBase(
          get_menu_title(NOW_PLAYING_MENU_STATE_MEDIA_PLAYERS), "",
          homething_menu_base::ArrowMenuTitleRightIcon));
    }
    if (draw_grouping()) {
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
        if (media_player_group_->get_active_player() != nullptr) {
          homeThingNowPlayingMenuSources::bottomMenuTitles(
              media_player_group_->get_active_player(),
              circle_menu_->get_bottom_menu(), menu_titles);
        }
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
      case NOW_PLAYING_MENU_STATE_NONE:
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
      return homeThingNowPlayingMenuSources::select_source_menu(
          index, media_player_group_, circle_menu_, &menu_state_);
    case NOW_PLAYING_MENU_STATE_MEDIA_PLAYERS: {
      return homeThingNowPlayingMenuMediaPlayers::select_menu(
          index, media_player_group_);
      case NOW_PLAYING_MENU_STATE_NONE:
        break;
    }
  }
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

int HomeThingMenuNowPlayingControl::root_menu_size() {
  if (media_player_group_ != nullptr) {
    bool show_source = draw_source();
    bool show_media_players = draw_media_players();
    bool show_grouping = draw_grouping();
    return 1 + show_source + show_media_players + show_grouping;
  }
  return 0;
}

void HomeThingMenuNowPlayingControl::set_app_menu_index(int app_menu_index) {
  if (app_menu_index >= root_menu_size()) {
    return;
  }
  menu_state_ = NowPlayingMenuState(app_menu_index + 1);
}

bool HomeThingMenuNowPlayingControl::should_draw_app() {
  switch (menu_state_) {
    case NOW_PLAYING_MENU_STATE_NOW_PLAYING:
      return true;
    case NOW_PLAYING_MENU_STATE_NONE:
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
  active_tick();
  switch (menu_state_) {
    case NOW_PLAYING_MENU_STATE_NOW_PLAYING:
      now_playing_display_->drawNowPlaying(
          menuIndex, circle_menu_->get_active_menu(), active_menu);
      break;
    case NOW_PLAYING_MENU_STATE_NONE:
    case NOW_PLAYING_MENU_STATE_GROUPING:
    case NOW_PLAYING_MENU_STATE_SOURCE:
    case NOW_PLAYING_MENU_STATE_MEDIA_PLAYERS:
      break;
  }
}

bool HomeThingMenuNowPlayingControl::idleTick(int idleTime,
                                              int display_timeout) {
  ESP_LOGD(TAG, "idleTick: idle %d", idleTime);
  if (media_player_group_ != nullptr) {
    media_player_group_->findActivePlayer();
  }
  animating_ = idleTime < display_timeout;
  if (idleTime == 3) {
    circle_menu_->clear_active_menu();
  } else if (idleTime == display_timeout) {
    now_playing_display_->resetTick();
    return false;
  } else if (media_player_group_ != NULL) {
    bool updatedMediaPositions = media_player_group_->updateMediaPosition();
    if (updatedMediaPositions) {
      switch (menu_state_) {
        case NOW_PLAYING_MENU_STATE_NOW_PLAYING:
          ESP_LOGD(TAG, "idleTick: update media positions");
          return true;
          break;
        default:
          break;
      }
    }
  }
  return false;
}

void HomeThingMenuNowPlayingControl::active_tick() {
  now_playing_display_->tickAnimation();
}

void HomeThingMenuNowPlayingControl::reset_menu() {
  if (media_player_group_) {
    media_player_group_->newSpeakerGroupParent = NULL;
    media_player_group_->set_active_player_source_index(-1);
  }
  menu_state_ = NOW_PLAYING_MENU_STATE_NONE;
  now_playing_display_->resetTick();
}

void HomeThingMenuNowPlayingControl::select_media_player_feature(
    homeassistant_media_player::MediaPlayerFeatureCommand* command) {
  auto feature = command->get_feature();
  switch (feature) {
    case homeassistant_media_player::MediaPlayerSupportedFeature::GROUPING:
      menu_state_ = NOW_PLAYING_MENU_STATE_GROUPING;
      ESP_LOGD(TAG, "select_media_player_feature: GROUPING");
      return;
    case homeassistant_media_player::MediaPlayerSupportedFeature::
        CUSTOM_COMMAND: {
      auto feature_command = command->get_command();
      ESP_LOGD(TAG, "select_media_player_feature: CUSTOM_COMMAND %s",
               feature_command->get_name().c_str());
      if (feature_command != nullptr) {
        feature_command->on_command();
        return;
      }
      break;
    }
    default:
      media_player_group_->call_feature(feature);
      break;
  }
  return;
}

homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::button_press_now_playing_option(
    CircleOptionMenuPosition position) {
  auto menu = circle_menu_->get_active_menu();
  if (menu == nullptr) {
    return homething_menu_app::NavigationCoordination::
        NavigationCoordinationNone;
  }
  switch (menu->type) {
    case circleOptionMenu: {
      auto command = circle_menu_->tap_option_menu(
          position, media_player_group_->get_active_player());
      if (command) {
        auto feature = command->get_feature();
        ESP_LOGI(TAG,
                 "button_press_now_playing_option: option menu selected %d",
                 feature);
        circle_menu_->clear_active_menu();
        if (feature == homeassistant_media_player::MediaPlayerSupportedFeature::
                           MENU_HOME) {
          return homething_menu_app::NavigationCoordination::
              NavigationCoordinationPop;
        }
        select_media_player_feature(command);
        return homething_menu_app::NavigationCoordination::
            NavigationCoordinationUpdate;
      }
      ESP_LOGW(TAG,
               "button_press_now_playing_option: option menu NOT selected");
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationNone;
    }
    default:
      break;
  }
  circle_menu_->clear_active_menu();
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::selectNowPlayingBottomMenu(int index) {
  auto features = media_player_group_->active_player_->get_option_menu_features(
      circle_menu_->get_bottom_menu());
  auto active_feature = (*features)[index];
  ESP_LOGI(TAG, "selectNowPlayingMenu: %d, %s", index,
           active_feature->get_title().c_str());
  switch (active_feature->get_feature()) {
    case homeassistant_media_player::MediaPlayerSupportedFeature::VOLUME_SET:
    case homeassistant_media_player::MediaPlayerSupportedFeature::VOLUME_UP:
    case homeassistant_media_player::MediaPlayerSupportedFeature::VOLUME_DOWN:
      circle_menu_->set_active_menu(volumeOptionMenu,
                                    media_player_group_->active_player_);
      select_media_player_feature(active_feature);
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationUpdate;
    case homeassistant_media_player::MediaPlayerSupportedFeature::GROUPING:
      menu_state_ = NOW_PLAYING_MENU_STATE_GROUPING;
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationNone;
    case homeassistant_media_player::MediaPlayerSupportedFeature::MENU_HOME:
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationRoot;
    default:
      break;
  }
  select_media_player_feature(active_feature);
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

// MARK: Buttons

homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::rotaryScrollClockwise(int rotary) {
  switch (menu_state_) {
    case NOW_PLAYING_MENU_STATE_NOW_PLAYING:
      if (media_player_group_->active_player_ == nullptr) {
        return homething_menu_app::NavigationCoordination::
            NavigationCoordinationNone;
      }
      if (circle_menu_->get_bottom_menu())
        return homething_menu_app::NavigationCoordination::
            NavigationCoordinationNone;
      break;
    default:
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationNone;
  }
  media_player_group_->increaseSpeakerVolume();
  circle_menu_->set_active_menu(volumeOptionMenu,
                                media_player_group_->active_player_);
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}
homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::rotaryScrollCounterClockwise(int rotary) {
  switch (menu_state_) {
    case NOW_PLAYING_MENU_STATE_NOW_PLAYING:
      if (media_player_group_->active_player_ == nullptr) {
        return homething_menu_app::NavigationCoordination::
            NavigationCoordinationNone;
      }
      if (circle_menu_->get_bottom_menu())
        return homething_menu_app::NavigationCoordination::
            NavigationCoordinationNone;
      break;
    default:
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationNone;
  }
  media_player_group_->decreaseSpeakerVolume();
  circle_menu_->set_active_menu(volumeOptionMenu,
                                media_player_group_->active_player_);
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::buttonPressUp() {
  switch (menu_state_) {
    case NOW_PLAYING_MENU_STATE_NOW_PLAYING: {
      if (media_player_group_->active_player_ == nullptr) {
        return homething_menu_app::NavigationCoordination::
            NavigationCoordinationPop;
      }
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
              homeassistant_media_player::MediaPlayerTVRemoteCommand::
                  MEDIA_PLAYER_TV_COMMAND_UP);
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
    case NOW_PLAYING_MENU_STATE_NONE:
      break;
  }
  return homething_menu_app::NavigationCoordination::NavigationCoordinationPop;
}

homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::buttonPressDown() {
  switch (menu_state_) {
    case NOW_PLAYING_MENU_STATE_NOW_PLAYING:
      if (media_player_group_->active_player_ == nullptr) {
        return homething_menu_app::NavigationCoordination::
            NavigationCoordinationNone;
      }
      break;
    default:
      break;
  }
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
          homeassistant_media_player::MediaPlayerTVRemoteCommand::
              MEDIA_PLAYER_TV_COMMAND_DOWN);
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationReturn;
    case homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType:
      media_player_group_->active_player_->toggle();
      break;
  }
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingMenuNowPlayingControl::buttonPressLeft() {
  switch (menu_state_) {
    case NOW_PLAYING_MENU_STATE_NOW_PLAYING:
      if (media_player_group_->active_player_ == nullptr) {
        return homething_menu_app::NavigationCoordination::
            NavigationCoordinationNone;
      }
      break;
    default:
      break;
  }
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
          homeassistant_media_player::MediaPlayerTVRemoteCommand::
              MEDIA_PLAYER_TV_COMMAND_LEFT);
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
  switch (menu_state_) {
    case NOW_PLAYING_MENU_STATE_NOW_PLAYING:
      if (media_player_group_->active_player_ == nullptr) {
        return homething_menu_app::NavigationCoordination::
            NavigationCoordinationNone;
      }
      break;
    default:
      break;
  }
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
          homeassistant_media_player::MediaPlayerTVRemoteCommand::
              MEDIA_PLAYER_TV_COMMAND_RIGHT);
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
  switch (menu_state_) {
    case NOW_PLAYING_MENU_STATE_NOW_PLAYING:
      if (media_player_group_->active_player_ == nullptr) {
        return homething_menu_app::NavigationCoordination::
            NavigationCoordinationNone;
      }
      if (circle_menu_->get_bottom_menu()) {
        return selectNowPlayingBottomMenu(menuIndex);
      }
    default:
      break;
  }
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
          homeassistant_media_player::MediaPlayerTVRemoteCommand::
              MEDIA_PLAYER_TV_COMMAND_SELECT);
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
HomeThingMenuNowPlayingControl::buttonPressOption() {
  if (media_player_group_ == nullptr ||
      media_player_group_->active_player_ == nullptr) {
    return homething_menu_app::NavigationCoordination::
        NavigationCoordinationNone;
  }
  ESP_LOGI(TAG, "buttonPressOption: %d", menu_state_);
  if (menu_state_ == NOW_PLAYING_MENU_STATE_NONE) {
    ESP_LOGI(TAG, "buttonPressOption: toggle power");
    media_player_group_->togglePower();
    return homething_menu_app::NavigationCoordination::
        NavigationCoordinationUpdate;
  }
  if (circle_menu_->get_active_menu()) {
    circle_menu_->clear_active_menu();
  } else {
    circle_menu_->set_active_menu(circleOptionMenu,
                                  media_player_group_->active_player_);
  }
  return homething_menu_app::NavigationCoordination::
      NavigationCoordinationUpdate;
}

}  // namespace homething_menu_now_playing
}  // namespace esphome
