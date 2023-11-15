#pragma once
#ifdef USE_NOW_PLAYING  // now playing bottom menu

#include <string>
#include "esphome/components/homeThing/homeThingMenuTitleToggle.h"

namespace esphome {
namespace homething_menu_base {

class MenuTitlePlayer : public MenuTitleToggle {
 public:
  homeassistant_media_player::HomeAssistantBaseMediaPlayer* media_player_;

  MenuTitlePlayer(std::string new_name, std::string newEntityId,
                  MenuTitleLeftIcon newLeftIconState,
                  MenuTitleRightIcon newRightIconState,
                  homeassistant_media_player::HomeAssistantBaseMediaPlayer*
                      new_media_player)
      : MenuTitleToggle{new_name, newEntityId, newLeftIconState,
                        newRightIconState, PlayerMenuTitleType},
        media_player_(new_media_player) {}

  std::string mediaSourceIcon() {
    switch (media_player_->mediaSource) {
      case homeassistant_media_player::RemotePlayerMediaSource::
          NoRemotePlayerMediaSource:
        return "󰐊";
      case homeassistant_media_player::RemotePlayerMediaSource::
          HomeRemotePlayerMediaSource:
        return "󰋜";
      case homeassistant_media_player::RemotePlayerMediaSource::
          YouTubeRemotePlayerMediaSource:
        return "󰗃";
      case homeassistant_media_player::RemotePlayerMediaSource::
          SpotifyRemotePlayerMediaSource:
        return "󰓇";
      case homeassistant_media_player::RemotePlayerMediaSource::
          NetflixRemotePlayerMediaSource:
        return "󰝆";
      case homeassistant_media_player::RemotePlayerMediaSource::
          PlexRemotePlayerMediaSource:
        return "󰚺";
      case homeassistant_media_player::RemotePlayerMediaSource::
          TVRemotePlayerMediaSource:
        return "󰔂";
    }
    return "";
  }

  Color mediaSourceIconColor(Color defaultColor) {
    switch (media_player_->mediaSource) {
      case homeassistant_media_player::RemotePlayerMediaSource::
          NoRemotePlayerMediaSource:
      case homeassistant_media_player::RemotePlayerMediaSource::
          TVRemotePlayerMediaSource:
      case homeassistant_media_player::RemotePlayerMediaSource::
          HomeRemotePlayerMediaSource:
        return defaultColor;
      case homeassistant_media_player::RemotePlayerMediaSource::
          YouTubeRemotePlayerMediaSource:
        return Color(248, 0, 0);
      case homeassistant_media_player::RemotePlayerMediaSource::
          SpotifyRemotePlayerMediaSource:
        return Color(27, 212, 96);
      case homeassistant_media_player::RemotePlayerMediaSource::
          NetflixRemotePlayerMediaSource:
        return Color(179, 0, 10);
      case homeassistant_media_player::RemotePlayerMediaSource::
          PlexRemotePlayerMediaSource:
        return Color(223, 156, 13);
    }
    return defaultColor;
  }

  static MenuTitlePlayer headerMediaPlayerTitle(
      homeassistant_media_player::HomeAssistantBaseMediaPlayer* active_player) {
    std::string friendlyName = active_player->get_name();
    if (active_player->get_player_type() !=
        homeassistant_media_player::RemotePlayerType::TVRemotePlayerType) {
      auto activeSpeaker = static_cast<
          homeassistant_media_player::HomeAssistantSpeakerMediaPlayer*>(
          active_player);
      if (activeSpeaker != NULL) {
        auto groupMembers = *(activeSpeaker->get_group_members());
        if (groupMembers.size() > 1) {
          friendlyName =
              friendlyName + " + " + to_string(groupMembers.size() - 1);
        }
      }
    }
    return MenuTitlePlayer(friendlyName, active_player->get_entity_id(),
                           NoMenuTitleLeftIcon, NoMenuTitleRightIcon,
                           active_player);
  }
};
}  // namespace homething_menu_base
}  // namespace esphome
#endif
