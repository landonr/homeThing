#include "homeThingMenuNowPlayingControl.h"

namespace esphome {
namespace homething_menu_now_playing {

std::vector<MenuStates> HomeThingMenuNowPlayingControl::rootMenuTitles() {
  std::vector<MenuStates> out;
  if (media_player_group_) {
    if (media_player_group_->totalPlayers() > 1) {
      out.insert(out.end(), {nowPlayingMenu, sourcesMenu, mediaPlayersMenu});
    } else {
      out.insert(out.end(), {nowPlayingMenu, sourcesMenu});
    }
  }
  return out;
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
      menuTree.push_back(groupMenu);
      break;
    default:
      break;
  }
  select_media_player_feature(active_feature);
  update_display();
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
      update_display();
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