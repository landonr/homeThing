#include "HomeAssistantRokuMediaPlayer.h"

namespace esphome {
namespace homeassistant_media_player {

static const char* const TAG = "homeassistant.media_player_roku";

void HomeAssistantRokuMediaPlayer::setup() {
  setupBase();
  ESP_LOGI(TAG, "'%s': Subscribe states", get_name().c_str());
  subscribe_homeassistant_state(
      &HomeAssistantRokuMediaPlayer::player_source_changed, entity_id_,
      "source");
  subscribe_homeassistant_state(
      &HomeAssistantRokuMediaPlayer::player_source_list_changed, entity_id_,
      "source_list");
}

void HomeAssistantRokuMediaPlayer::player_source_changed(std::string state) {
  ESP_LOGI(TAG, "%s Player source changed to %s", get_name().c_str(),
           state.c_str());
  if (state.find("YouTube") != std::string::npos) {
    mediaSource = YouTubeRemotePlayerMediaSource;
  } else if (state.find("Netflix") != std::string::npos) {
    mediaSource = NetflixRemotePlayerMediaSource;
  } else if (state.find("Plex") != std::string::npos) {
    mediaSource = PlexRemotePlayerMediaSource;
  } else if (state.find("Roku") != std::string::npos) {
    mediaSource = HomeRemotePlayerMediaSource;
  } else {
    mediaSource = NoRemotePlayerMediaSource;
  }
  display->updateDisplay(false);
}

void HomeAssistantRokuMediaPlayer::player_source_list_changed(
    std::string state) {
  ESP_LOGI(TAG, "%s Player source list changed to %s", get_name().c_str(),
           state.c_str());
  auto newSources = TextHelpers::parseJsonArray(
      TextHelpers::replaceAll(state, "\\xa0", " "), "source");
  sources.assign(newSources.begin(), newSources.end());
}

void HomeAssistantRokuMediaPlayer::tvRemoteCommand(std::string command) {
  std::string remoteName = entity_id_.substr(12).insert(0, "remote");
  ESP_LOGI(TAG, "remote %s, %s", command.c_str(), remoteName.c_str());
  call_homeassistant_service("remote.send_command",
                             {
                                 {"entity_id", remoteName},
                                 {"command", command.c_str()},
                             });
}

void HomeAssistantRokuMediaPlayer::increaseVolume() {
  tvRemoteCommand("volume_up");
}

void HomeAssistantRokuMediaPlayer::decreaseVolume() {
  tvRemoteCommand("volume_down");
}

}  // namespace homeassistant_media_player
}  // namespace esphome
