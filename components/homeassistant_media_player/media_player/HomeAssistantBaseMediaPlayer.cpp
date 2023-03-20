#include "HomeAssistantBaseMediaPlayer.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_media_player {

static const char* const TAG = "homeassistant.media_player_base";

void HomeAssistantBaseMediaPlayer::setupBase() {
  ESP_LOGI(TAG, "'%s': Subscribe states", get_name().c_str());

  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("supported_features"),
      std::bind(
          &HomeAssistantBaseMediaPlayer::player_supported_features_changed,
          this, std::placeholders::_1));
}

void HomeAssistantBaseMediaPlayer::playSource(MediaPlayerSource source) {
  ESP_LOGI(TAG, "Player play source %s %s %s %d", this->entity_id_.c_str(),
           source.media_content_id_.c_str(), source.sourceTypeString().c_str(),
           source.media_content_type_);
  switch (source.media_content_type_) {
    case MusicRemotePlayerSourceType:
    case FavoriteItemIDRemotePlayerSourceType:
    case PlaylistRemotePlayerSourceType:
      playMedia(source);
      break;
    case SourceRemotePlayerSourceType:
      selectSource(source);
      break;
  }
}

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
  playlist_title = "";
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

void HomeAssistantBaseMediaPlayer::player_supported_features_changed(
    std::string state) {
  ESP_LOGI(TAG, "%s state changed to %s", this->entity_id_.c_str(),
           state.c_str());
  std::vector<std::shared_ptr<MediaPlayerSupportedFeature>> out;
  for (auto const& x : supported_feature_string_map) {
    if (state.find(x.second) != std::string::npos) {
      auto new_feature = std::make_shared<MediaPlayerSupportedFeature>(x.first);
      out.push_back(new_feature);
      ESP_LOGI(TAG, "%s, supported feature: %s", this->entity_id_.c_str(),
               x.second.c_str());
      switch (x.first) {
        case PAUSE:
          subscribe_player_state();
          break;
        case VOLUME_MUTE:
          subscribe_muted();
          break;
        case SHUFFLE_SET:
          subscribe_shuffle();
          break;
        case VOLUME_SET:
          subscribe_volume();
          break;
        case SEEK:
          subscribe_media_position();
          break;
        case SELECT_SOURCE:
          subscribe_sources();
          break;
        case CLEAR_PLAYLIST:
          subscribe_playlist();
          break;
        case GROUPING:
          subscribe_group_members();
          break;
        case SELECT_SOUND_MODE:
        case BROWSE_MEDIA:
        case REPEAT_SET:
        case TURN_ON:
        case TURN_OFF:
        case PLAY_MEDIA:
        case VOLUME_STEP:
        case STOP:
          break;

        // unsupported
        case PLAY:
        case PREVIOUS_TRACK:
        case NEXT_TRACK:
        default:
          break;
      }
    }
  }
  supported_features_ = out;
}

void HomeAssistantBaseMediaPlayer::subscribe_player_state() {
  ESP_LOGI(TAG, "subscribe_player_state: %s", this->entity_id_.c_str());
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, {},
      std::bind(&HomeAssistantBaseMediaPlayer::playerState_changed, this,
                std::placeholders::_1));
}

void HomeAssistantBaseMediaPlayer::subscribe_shuffle() {
  ESP_LOGI(TAG, "subscribe_shuffle: %s", this->entity_id_.c_str());
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("shuffle"),
      std::bind(&HomeAssistantBaseMediaPlayer::shuffle_changed, this,
                std::placeholders::_1));
}

void HomeAssistantBaseMediaPlayer::subscribe_muted() {
  ESP_LOGI(TAG, "subscribe_muted: %s", this->entity_id_.c_str());
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("is_volume_muted"),
      std::bind(&HomeAssistantBaseMediaPlayer::muted_changed, this,
                std::placeholders::_1));
}

void HomeAssistantBaseMediaPlayer::subscribe_media_position() {
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("media_duration"),
      std::bind(&HomeAssistantBaseMediaPlayer::media_duration_changed, this,
                std::placeholders::_1));

  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("media_position"),
      std::bind(&HomeAssistantBaseMediaPlayer::media_position_changed, this,
                std::placeholders::_1));
}

void HomeAssistantBaseMediaPlayer::subscribe_volume() {
  ESP_LOGI(TAG, "subscribe_volume: %s", this->entity_id_.c_str());
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("volume_level"),
      std::bind(&HomeAssistantBaseMediaPlayer::volume_changed, this,
                std::placeholders::_1));
}

void HomeAssistantBaseMediaPlayer::subscribe_playlist() {
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("media_playlist"),
      std::bind(&HomeAssistantBaseMediaPlayer::playlist_changed, this,
                std::placeholders::_1));
}

void HomeAssistantBaseMediaPlayer::subscribe_group_members() {
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("group_members"),
      std::bind(&HomeAssistantBaseMediaPlayer::group_members_changed, this,
                std::placeholders::_1));
}

void HomeAssistantBaseMediaPlayer::muted_changed(std::string state) {
  ESP_LOGI(TAG, "%s speaker_muted_changed to %s", this->entity_id_.c_str(),
           state.c_str());
  muted_ = strcmp(state.c_str(), "on") == 0;
  this->publish_state();
}

void HomeAssistantBaseMediaPlayer::shuffle_changed(std::string state) {
  ESP_LOGI(TAG, "%s speaker_muted_changed to %s", this->entity_id_.c_str(),
           state.c_str());
  shuffle_ = strcmp(state.c_str(), "on") == 0;
  this->publish_state();
}

void HomeAssistantBaseMediaPlayer::volume_changed(std::string state) {
  ESP_LOGI(TAG, "%s volume_changed to %s", this->entity_id_.c_str(),
           state.c_str());
  volume = parse_number<float>(state).value_or(-1.0f);
}

void HomeAssistantBaseMediaPlayer::media_duration_changed(std::string state) {
  ESP_LOGI(TAG, "%s media_duration_changed to %s", this->entity_id_.c_str(),
           state.c_str());
  mediaDuration = atof(state.c_str());
  this->publish_state();
}

void HomeAssistantBaseMediaPlayer::media_position_changed(std::string state) {
  ESP_LOGI(TAG, "%s media_position_changed to %s", this->entity_id_.c_str(),
           state.c_str());
  mediaPosition = atof(state.c_str());
  this->publish_state();
}

void HomeAssistantBaseMediaPlayer::playlist_changed(std::string state) {
  ESP_LOGI(TAG, "%s playlist_changed to %s", this->entity_id_.c_str(),
           state.c_str());
  playlist_title = state.c_str();
  this->publish_state();
}

void HomeAssistantBaseMediaPlayer::toggle_shuffle() {
  ESP_LOGI(TAG, "%s toggle shuffle", this->entity_id_.c_str());
  call_homeassistant_service("media_player.shuffle_set",
                             {
                                 {"entity_id", this->entity_id_},
                                 {"shuffle", is_shuffling() ? "false" : "true"},
                             });
}

void HomeAssistantBaseMediaPlayer::toggle_mute() {
  ESP_LOGI(TAG, "%s toggle mute", this->entity_id_.c_str());
  call_homeassistant_service(
      "media_player.volume_mute",
      {
          {"entity_id", this->entity_id_},
          {"is_volume_muted", is_muted() ? "false" : "true"},
      });
}
}  // namespace homeassistant_media_player
}  // namespace esphome
