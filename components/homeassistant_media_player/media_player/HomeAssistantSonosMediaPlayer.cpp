#include "HomeAssistantSonosMediaPlayer.h"

namespace esphome {
namespace homeassistant_media_player {

static const char* const TAG = "homeassistant.media_player_roku";

void HomeAssistantSonosMediaPlayer::setup() {
  ESP_LOGI(TAG, "Sonos Speaker subbed %s", this->entity_id_.c_str());
  subscribe_homeassistant_state(
      &HomeAssistantSonosMediaPlayer::speaker_volume_changed, this->entity_id_,
      "volume_level");
  subscribe_homeassistant_state(
      &HomeAssistantSonosMediaPlayer::speaker_muted_changed, this->entity_id_,
      "is_volume_muted");
  subscribe_homeassistant_state(&HomeAssistantSonosMediaPlayer::shuffle_changed,
                                this->entity_id_, "shuffle");
  subscribe_homeassistant_state(
      &HomeAssistantSonosMediaPlayer::group_members_changed, this->entity_id_,
      "group_members");
  subscribe_homeassistant_state(
      &HomeAssistantSonosMediaPlayer::player_media_title_changed,
      this->entity_id_, "media_title");
  subscribe_homeassistant_state(
      &HomeAssistantSonosMediaPlayer::player_media_artist_changed,
      this->entity_id_, "media_artist");
  subscribe_homeassistant_state(
      &HomeAssistantSonosMediaPlayer::playlist_changed, this->entity_id_,
      "media_playlist");
  subscribe_homeassistant_state(
      &HomeAssistantSonosMediaPlayer::media_album_changed, this->entity_id_,
      "media_album_name");
  subscribe_homeassistant_state(
      &HomeAssistantSonosMediaPlayer::media_duration_changed, this->entity_id_,
      "media_duration");
  subscribe_homeassistant_state(
      &HomeAssistantSonosMediaPlayer::media_position_changed, this->entity_id_,
      "media_position");
  subscribe_homeassistant_state(
      &HomeAssistantSonosMediaPlayer::media_source_changed, this->entity_id_,
      "media_content_id");
}

void HomeAssistantSonosMediaPlayer::ungroup() {
  ESP_LOGI(TAG, "%s ungroup speaker", this->entity_id_.c_str());
  call_homeassistant_service("media_player.unjoin",
                             {
                                 {"entity_id", this->entity_id_},
                             });
}

void HomeAssistantSonosMediaPlayer::joinGroup(std::string newSpeakerName) {
  ESP_LOGI(TAG, "%s group speaker to %s", this->entity_id_.c_str(),
           newSpeakerName.c_str());
  call_homeassistant_service("media_player.join",
                             {
                                 {"entity_id", newSpeakerName.c_str()},
                                 {"group_members", this->entity_id_.c_str()},
                             });
}

void HomeAssistantSonosMediaPlayer::toggleShuffle() {
  ESP_LOGI(TAG, "%s toggle shuffle", this->entity_id_.c_str());
  call_homeassistant_service("media_player.shuffle_set",
                             {
                                 {"entity_id", this->entity_id_},
                                 {"shuffle", shuffle ? "false" : "true"},
                             });
}

void HomeAssistantSonosMediaPlayer::toggleMute() {
  ESP_LOGI(TAG, "%s toggle mute", this->entity_id_.c_str());
  call_homeassistant_service("media_player.volume_mute",
                             {
                                 {"entity_id", this->entity_id_},
                                 {"is_volume_muted", muted ? "false" : "true"},
                             });
}

void HomeAssistantSonosMediaPlayer::increaseVolume() {
  if (speaker_volume == -1) {
    localVolume = 0;
    return;
  }
  if (localVolume + volumeStep > 1) {
    localVolume = 1;
  } else {
    localVolume = localVolume + volumeStep;
  }
  updateVolumeLevel();
}

void HomeAssistantSonosMediaPlayer::decreaseVolume() {
  if (speaker_volume == -1 || localVolume - volumeStep < 0) {
    localVolume = 0;
    return;
  }
  if (localVolume - volumeStep > 1) {
    localVolume = 0;
  } else {
    localVolume = localVolume - volumeStep;
  }
  updateVolumeLevel();
}

void HomeAssistantSonosMediaPlayer::updateVolumeLevel() {
  std::string entityIds = this->entity_id_;
  for (auto& speaker : groupMembers) {
    if (speaker != this->entity_id_) {
      entityIds = entityIds + ", " + speaker;
    }
  }
  if (muted) {
    // unmute all speakers
    call_homeassistant_service("media_player.volume_mute",
                               {
                                   {"entity_id", entityIds},
                                   {"is_volume_muted", "false"},
                               });
  }
  ESP_LOGI(TAG, "%s volume update %f", entityIds.c_str(), localVolume);
  call_homeassistant_service("media_player.volume_set",
                             {
                                 {"entity_id", entityIds},
                                 {"volume_level", to_string(localVolume)},
                             });
}

void HomeAssistantSonosMediaPlayer::clearSource() {
  HomeAssistantBaseMediaPlayer::clearSource();
  mediaPlaylist = "";
  mediaAlbumName = "";
}

void HomeAssistantSonosMediaPlayer::player_media_title_changed(
    std::string state) {
  ESP_LOGI("Player", "%s Player media title changed to %s",
           this->entity_id_.c_str(), state.c_str());
  if (strcmp(state.c_str(), mediaTitle.c_str()) != 0) {
    mediaPosition = 0;
  }
  if (strcmp("TV", state.c_str()) != 0) {
    mediaTitle = state.c_str();
  } else {
    mediaTitle = "";
    mediaArtist = "";
    mediaPlaylist = "";
    mediaPosition = -1;
  }
  mediaDuration = -1;
}

void HomeAssistantSonosMediaPlayer::player_media_artist_changed(
    std::string state) {
  ESP_LOGI("Player", "%s Player artist changed to %s", this->entity_id_.c_str(),
           state.c_str());
  mediaArtist = state.c_str();
}

void HomeAssistantSonosMediaPlayer::speaker_volume_changed(std::string state) {
  ESP_LOGI(TAG, "%s Sonos Speaker volume changed to %s",
           this->entity_id_.c_str(), state.c_str());
  speaker_volume = atof(state.c_str());
  if (localVolume == -1) {
    localVolume = atof(state.c_str());
  }
}

void HomeAssistantSonosMediaPlayer::speaker_muted_changed(std::string state) {
  ESP_LOGI(TAG, "%s Sonos Speaker muted changed to %s",
           this->entity_id_.c_str(), state.c_str());
  muted = strcmp(state.c_str(), "on") == 0;
}

void HomeAssistantSonosMediaPlayer::shuffle_changed(std::string state) {
  ESP_LOGI(TAG, "%s Sonos Speaker shuffle changed to %s",
           this->entity_id_.c_str(), state.c_str());
  shuffle = strcmp(state.c_str(), "on") == 0;
}

void HomeAssistantSonosMediaPlayer::playlist_changed(std::string state) {
  ESP_LOGI(TAG, "%s Sonos Speaker playlist changed to %s",
           this->entity_id_.c_str(), state.c_str());
  mediaPlaylist = state.c_str();
}

void HomeAssistantSonosMediaPlayer::media_album_changed(std::string state) {
  ESP_LOGI(TAG, "%s Sonos Speaker album changed to %s",
           this->entity_id_.c_str(), state.c_str());
  mediaAlbumName = state.c_str();
}

void HomeAssistantSonosMediaPlayer::group_members_changed(std::string state) {
  ESP_LOGI(TAG, "%s Sonos Speaker group members changed to %s",
           this->entity_id_.c_str(), state.c_str());
  groupMembers.clear();
  std::vector<std::string>* out = new std::vector<std::string>();
  TextHelpers::tokenize(state, ",", out);
  for (auto state = out->begin(); state != out->end(); ++state) {
    std::string newGroupedSpeaker = TextHelpers::filter(*state);
    groupMembers.push_back(newGroupedSpeaker);
  }
}

void HomeAssistantSonosMediaPlayer::media_duration_changed(std::string state) {
  ESP_LOGI(TAG, "%s Sonos Speaker media duration changed to %s",
           this->entity_id_.c_str(), state.c_str());
  mediaDuration = atof(state.c_str());
}

void HomeAssistantSonosMediaPlayer::media_position_changed(std::string state) {
  ESP_LOGI(TAG, "%s Sonos Speaker media position changed to %s",
           this->entity_id_.c_str(), state.c_str());
  mediaPosition = atof(state.c_str());
}

void HomeAssistantSonosMediaPlayer::media_source_changed(std::string state) {
  ESP_LOGI(TAG, "%s Speaker source changed to %s", this->entity_id_.c_str(),
           state.c_str());
  if (state.find("spdif") != std::string::npos) {
    mediaSource = TVRemotePlayerMediaSource;
  } else if (state.find("spotify") != std::string::npos) {
    mediaSource = SpotifyRemotePlayerMediaSource;
  } else {
    mediaSource = NoRemotePlayerMediaSource;
  }
}

media_player::MediaPlayerTraits HomeAssistantSonosMediaPlayer::get_traits() {
  auto traits = media_player::MediaPlayerTraits();
  traits.set_supports_pause(true);
  return traits;
}

void HomeAssistantSonosMediaPlayer::control(
    const media_player::MediaPlayerCall& call) {
  if (call.get_media_url().has_value()) {
    // if (this->audio_->isRunning())
    //   this->audio_->stopSong();
    // this->high_freq_.start();
    // this->audio_->connecttohost(call.get_media_url().value().c_str());
    // this->state = media_player::MEDIA_PLAYER_STATE_PLAYING;
  }
  if (call.get_volume().has_value()) {
    // this->volume = call.get_volume().value();
    // this->set_volume_(volume);
    // this->unmute_();
  }
  if (call.get_command().has_value()) {
    // switch (call.get_command().value()) {
    //   case media_player::MEDIA_PLAYER_COMMAND_PLAY:
    //     if (!this->audio_->isRunning())
    //       this->audio_->pauseResume();
    //     this->state = media_player::MEDIA_PLAYER_STATE_PLAYING;
    //     break;
    //   case media_player::MEDIA_PLAYER_COMMAND_PAUSE:
    //     if (this->audio_->isRunning())
    //       this->audio_->pauseResume();
    //     this->state = media_player::MEDIA_PLAYER_STATE_PAUSED;
    //     break;
    //   case media_player::MEDIA_PLAYER_COMMAND_STOP:
    //     this->stop_();
    //     break;
    //   case media_player::MEDIA_PLAYER_COMMAND_MUTE:
    //     this->mute_();
    //     break;
    //   case media_player::MEDIA_PLAYER_COMMAND_UNMUTE:
    //     this->unmute_();
    //     break;
    //   case media_player::MEDIA_PLAYER_COMMAND_TOGGLE:
    //     this->audio_->pauseResume();
    //     if (this->audio_->isRunning()) {
    //       this->state = media_player::MEDIA_PLAYER_STATE_PLAYING;
    //     } else {
    //       this->state = media_player::MEDIA_PLAYER_STATE_PAUSED;
    //     }
    //     break;
    //   case media_player::MEDIA_PLAYER_COMMAND_VOLUME_UP: {
    //     float new_volume = this->volume + 0.1f;
    //     if (new_volume > 1.0f)
    //       new_volume = 1.0f;
    //     this->set_volume_(new_volume);
    //     this->unmute_();
    //     break;
    //   }
    //   case media_player::MEDIA_PLAYER_COMMAND_VOLUME_DOWN: {
    //     float new_volume = this->volume - 0.1f;
    //     if (new_volume < 0.0f)
    //       new_volume = 0.0f;
    //     this->set_volume_(new_volume);
    //     this->unmute_();
    //     break;
    //   }
    // }
  }
  this->publish_state();
}

}  // namespace homeassistant_media_player
}  // namespace esphome
