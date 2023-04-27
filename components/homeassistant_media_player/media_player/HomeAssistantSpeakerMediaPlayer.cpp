#include "HomeAssistantSpeakerMediaPlayer.h"
#include "JSONTextHelpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_media_player {

static const char* const TAG = "homeassistant.media_player_speaker";

void HomeAssistantSpeakerMediaPlayer::setup() {
  HomeAssistantBaseMediaPlayer::setup();
}

void HomeAssistantSpeakerMediaPlayer::subscribe_source() {
  ESP_LOGI(TAG, "subscribe_source: %s", this->entity_id_.c_str());
  subscribe_homeassistant_state(
      &HomeAssistantSpeakerMediaPlayer::media_album_changed, this->entity_id_,
      "media_album_name");
  subscribe_homeassistant_state(
      &HomeAssistantSpeakerMediaPlayer::media_content_id_changed,
      this->entity_id_, "media_content_id");
}

void HomeAssistantSpeakerMediaPlayer::ungroup() {
  ESP_LOGI(TAG, "ungroup: %s", this->entity_id_.c_str());
  call_homeassistant_service("media_player.unjoin",
                             {
                                 {"entity_id", this->entity_id_},
                             });
}

void HomeAssistantSpeakerMediaPlayer::joinGroup(std::string newSpeakerName) {
  ESP_LOGI(TAG, "joinGroup: %s group to %s", this->entity_id_.c_str(),
           newSpeakerName.c_str());
  call_homeassistant_service("media_player.join",
                             {
                                 {"entity_id", newSpeakerName.c_str()},
                                 {"group_members", this->entity_id_.c_str()},
                             });
}

void HomeAssistantSpeakerMediaPlayer::updateVolumeLevel() {
  std::string entityIds = this->entity_id_;
  for (auto& speaker : groupMembers) {
    if (speaker != this->entity_id_) {
      entityIds = entityIds + ", " + speaker;
    }
  }
  ignore_api_updates_with_seconds(2);
  if (is_muted()) {
    // unmute all speakers
    call_homeassistant_service("media_player.volume_mute",
                               {
                                   {"entity_id", entityIds},
                                   {"is_volume_muted", "false"},
                               });
  }
  ESP_LOGI(TAG, "%s volume update %f", entityIds.c_str(), volume);
  call_homeassistant_service("media_player.volume_set",
                             {
                                 {"entity_id", entityIds},
                                 {"volume_level", to_string(volume)},
                             });
}

void HomeAssistantSpeakerMediaPlayer::clearSource() {
  HomeAssistantBaseMediaPlayer::clearSource();
  playlist_title = "";
  media_album_name = "";
}

void HomeAssistantSpeakerMediaPlayer::media_album_changed(std::string state) {
  ESP_LOGI(TAG, "media_album_changed: %s changed to %s",
           this->entity_id_.c_str(), state.c_str());
  media_album_name = state.c_str();
  this->publish_state();
}

void HomeAssistantSpeakerMediaPlayer::media_content_id_changed(
    std::string state) {
  ESP_LOGI(TAG, "media_content_id_changed: %s changed to %s",
           this->entity_id_.c_str(), state.c_str());
  if (!can_update_from_api()) {
    return;
  }
  if (state.find("spdif") != std::string::npos) {
    mediaSource = TVRemotePlayerMediaSource;
  } else if (state.find("spotify") != std::string::npos) {
    mediaSource = SpotifyRemotePlayerMediaSource;
  } else {
    mediaSource = NoRemotePlayerMediaSource;
  }
  this->publish_state();
}

media_player::MediaPlayerTraits HomeAssistantSpeakerMediaPlayer::get_traits() {
  auto traits = media_player::MediaPlayerTraits();
  traits.set_supports_pause(true);
  return traits;
}

void HomeAssistantSpeakerMediaPlayer::control(
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
