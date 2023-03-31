#include "HomeAssistantTVMediaPlayer.h"
#include "JSONTextHelpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_media_player {

static const char* const TAG = "homeassistant.media_player_tv";

void HomeAssistantTVMediaPlayer::setup() {
  HomeAssistantBaseMediaPlayer::setup();
  ESP_LOGI(TAG, "'%s': Subscribe states", get_name().c_str());
}

void HomeAssistantTVMediaPlayer::subscribe_source() {
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("source"),
      std::bind(&HomeAssistantTVMediaPlayer::player_source_changed, this,
                std::placeholders::_1));
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
  } else if (state.find("Roku") != std::string::npos) {
    mediaSource = HomeRemotePlayerMediaSource;
  } else if (state.find("tvshow") != std::string::npos) {
    mediaSource = TVRemotePlayerMediaSource;
  } else {
    mediaSource = NoRemotePlayerMediaSource;
  }
}

void HomeAssistantTVMediaPlayer::subscribe_sources() {
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("source_list"),
      std::bind(&HomeAssistantTVMediaPlayer::sources_changed, this,
                std::placeholders::_1));
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
