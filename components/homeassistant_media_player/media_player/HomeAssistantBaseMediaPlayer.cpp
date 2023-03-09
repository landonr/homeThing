#include "HomeAssistantBaseMediaPlayer.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_media_player {

static const char* const TAG = "homeassistant.media_player_base";

void HomeAssistantBaseMediaPlayer::setupBase() {
  ESP_LOGI(TAG, "'%s': Subscribe states", get_name().c_str());
  subscribe_homeassistant_state(
      &HomeAssistantBaseMediaPlayer::playerState_changed,
      this->entity_id_.c_str());
}

void HomeAssistantBaseMediaPlayer::playSource(MediaPlayerSource source) {
  ESP_LOGI(TAG, "Player play source %s %s %s %d", this->entity_id_.c_str(),
           source.media_content_id_.c_str(), source.sourceTypeString().c_str(),
           source.media_content_type_);
  switch (source.media_content_type_) {
    case MusicRemotePlayerSourceType:
    case FavoriteItemIDRemotePlayerSourceType:
      playMedia(source);
      break;
    case SourceRemotePlayerSourceType:
      selectSource(source);
      break;
  }
}

void HomeAssistantBaseMediaPlayer::clearSource() {}

void HomeAssistantBaseMediaPlayer::playPause() {
  ESP_LOGI(TAG, "%s play pause", this->entity_id_.c_str());
  call_homeassistant_service("media_player.media_play_pause",
                             {
                                 {"entity_id", this->entity_id_},
                             });
}

void HomeAssistantBaseMediaPlayer::nextTrack() {
  ESP_LOGI(TAG, "%s next track", this->entity_id_.c_str());
  call_homeassistant_service("media_player.media_next_track",
                             {
                                 {"entity_id", this->entity_id_},
                             });
}

std::string HomeAssistantBaseMediaPlayer::mediaTitleString() {
  switch (get_player_type()) {
    case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
      return playerSourceStateString(mediaSource);
    case homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType:
      return mediaArtist;
  }
  return "";
}

std::string HomeAssistantBaseMediaPlayer::mediaSubtitleString() {
  switch (get_player_type()) {
    case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
      return "";
    case homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType:
      return mediaTitle;
  }
  return "";
}

void HomeAssistantBaseMediaPlayer::clearMedia() {
  clearSource();
  mediaTitle = "";
  mediaArtist = "";
}

void HomeAssistantBaseMediaPlayer::selectSource(MediaPlayerSource source) {
  ESP_LOGI(TAG, "%s select source %s", this->entity_id_.c_str(),
           source.media_content_id_.c_str());
  call_homeassistant_service("media_player.select_source",
                             {
                                 {"entity_id", entity_id_},
                                 {"source", source.media_content_id_.c_str()},
                             });
}

void HomeAssistantBaseMediaPlayer::playMedia(MediaPlayerSource source) {
  ESP_LOGI(TAG, "%s play media %s %s", this->entity_id_.c_str(),
           source.title_.c_str(), source.media_content_id_.c_str());
  call_homeassistant_service(
      "media_player.play_media",
      {
          {"entity_id", this->entity_id_},
          {"media_content_id", source.media_content_id_.c_str()},
          {"media_content_type", source.sourceTypeString().c_str()},
      });
}

void HomeAssistantBaseMediaPlayer::playerState_changed(std::string state) {
  ESP_LOGI(TAG, "%s state changed to %s", this->entity_id_.c_str(),
           state.c_str());
  if (state.length() == 0) {
    playerState = StoppedRemotePlayerState;
    this->state = media_player::MEDIA_PLAYER_STATE_IDLE;
  } else if (strcmp(state.c_str(), "playing") == 0) {
    if (playerState == PlayingRemotePlayerState) {
      clearSource();
    }
    playerState = PlayingRemotePlayerState;
    this->state = media_player::MEDIA_PLAYER_STATE_PLAYING;
  } else if (strcmp(state.c_str(), "paused") == 0) {
    playerState = PausedRemotePlayerState;
    this->state = media_player::MEDIA_PLAYER_STATE_PAUSED;
  } else if (strcmp(state.c_str(), "standby") == 0) {
    playerState = PowerOffRemotePlayerState;
    this->state = media_player::MEDIA_PLAYER_STATE_IDLE;
    clearMedia();
  } else if (strcmp(state.c_str(), "off") == 0) {
    this->state = media_player::MEDIA_PLAYER_STATE_IDLE;
    playerState = PowerOffRemotePlayerState;
    clearMedia();
  } else if (strcmp(state.c_str(), "home") == 0 ||
             strcmp(state.c_str(), "Roku") == 0) {
    this->state = media_player::MEDIA_PLAYER_STATE_IDLE;
    playerState = StoppedRemotePlayerState;
    clearMedia();
  } else if (strcmp(state.c_str(), "on") == 0) {
    this->state = media_player::MEDIA_PLAYER_STATE_IDLE;
    playerState = StoppedRemotePlayerState;
  } else if (strcmp(state.c_str(), "idle") == 0) {
    this->state = media_player::MEDIA_PLAYER_STATE_IDLE;
    clearMedia();
    playerState = StoppedRemotePlayerState;
  } else if (strcmp(state.c_str(), "unavailable") == 0) {
    this->state = media_player::MEDIA_PLAYER_STATE_NONE;
    playerState = UnavailableRemotePlayerState;
  } else {
    this->state = media_player::MEDIA_PLAYER_STATE_NONE;
    playerState = NoRemotePlayerState;
  }
  this->publish_state();
}
}  // namespace homeassistant_media_player
}  // namespace esphome
