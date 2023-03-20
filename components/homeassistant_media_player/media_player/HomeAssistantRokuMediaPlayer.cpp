#include "HomeAssistantRokuMediaPlayer.h"
#include "JSONTextHelpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_media_player {

static const char* const TAG = "homeassistant.media_player_roku";

void HomeAssistantRokuMediaPlayer::setup() {
  setupBase();
  ESP_LOGI(TAG, "'%s': Subscribe states", get_name().c_str());

  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("source"),
      std::bind(&HomeAssistantRokuMediaPlayer::player_source_changed, this,
                std::placeholders::_1));
}

void HomeAssistantRokuMediaPlayer::subscribe_sources() {
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("source_list"),
      std::bind(&HomeAssistantRokuMediaPlayer::sources_changed, this,
                std::placeholders::_1));
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
}

void HomeAssistantRokuMediaPlayer::sources_changed(std::string state) {
  ESP_LOGI(TAG, "sources_changed: %s - %s", get_name().c_str(), state.c_str());
  auto newSources = parseJsonArray(replaceAll(state, "\\xa0", " "), "source");
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

media_player::MediaPlayerTraits HomeAssistantRokuMediaPlayer::get_traits() {
  auto traits = media_player::MediaPlayerTraits();
  traits.set_supports_pause(true);
  return traits;
}

void HomeAssistantRokuMediaPlayer::control(
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
