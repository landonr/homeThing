#include "HomeAssistantTVMediaPlayer.h"
#include "JSONTextHelpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_media_player {

static const char* const TAG = "homeassistant.media_player_tv";

void HomeAssistantTVMediaPlayer::setup() {
  HomeAssistantBaseMediaPlayer::setup();
}

void HomeAssistantTVMediaPlayer::subscribe_source() {
  ESP_LOGI(TAG, "subscribe_source: %s", this->entity_id_.c_str());
  subscribe_homeassistant_state(
      &HomeAssistantTVMediaPlayer::player_source_changed, "source",
      this->entity_id_);
}

void HomeAssistantTVMediaPlayer::player_source_changed(std::string state) {
  ESP_LOGI(TAG, "player_source_changed: %s changed to %s", get_name().c_str(),
           state.c_str());
  if (state.find("YouTube") != std::string::npos) {
    mediaSource = YouTubeRemotePlayerMediaSource;
  } else if (state.find("Netflix") != std::string::npos) {
    mediaSource = NetflixRemotePlayerMediaSource;
  } else if (state.find("Plex") != std::string::npos) {
    mediaSource = PlexRemotePlayerMediaSource;
  } else if (state.find("Roku") != std::string::npos ||
             state.find("idle") != std::string::npos) {
    mediaSource = HomeRemotePlayerMediaSource;
    mediaTitle = "";
  } else if (state.find("tvshow") != std::string::npos ||
             state.find("playing") != std::string::npos) {
    mediaSource = TVRemotePlayerMediaSource;
  } else {
    mediaSource = NoRemotePlayerMediaSource;
  }
}

void HomeAssistantTVMediaPlayer::subscribe_sources() {
  ESP_LOGI(TAG, "subscribe_sources: %s", this->entity_id_.c_str());
  subscribe_homeassistant_state(&HomeAssistantTVMediaPlayer::sources_changed,
                                "source_list", this->entity_id_);
}

void HomeAssistantTVMediaPlayer::sources_changed(std::string state) {
  ESP_LOGI(TAG, "sources_changed: %s - %s", get_name().c_str(), state.c_str());
  auto newSources = parseJsonArray(replaceAll(state, "\\xa0", " "), "source");
  sources.assign(newSources.begin(), newSources.end());
}

void HomeAssistantTVMediaPlayer::tvRemoteCommand(
    MediaPlayerTVRemoteCommand command) {
  ESP_LOGI(TAG, "tvRemoteCommand: %d", command);
}

void HomeAssistantTVMediaPlayer::increaseVolume() {
  tvRemoteCommand(VOLUME_UP);
}

void HomeAssistantTVMediaPlayer::decreaseVolume() {
  tvRemoteCommand(VOLUME_DOWN);
}

void HomeAssistantTVMediaPlayer::control(
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
