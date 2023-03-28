#include "HomeAssistantTVRokuMediaPlayer.h"
#include "JSONTextHelpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_media_player {

static const char* const TAG = "homeassistant.media_player_roku";

void HomeAssistantTVRokuMediaPlayer::setup() {
  supported_features_.push_back(
      std::make_shared<MediaPlayerSupportedFeature>(TV_BACK));
  supported_features_.push_back(
      std::make_shared<MediaPlayerSupportedFeature>(TV_HOME));
  supported_features_.push_back(
      std::make_shared<MediaPlayerSupportedFeature>(MENU_HOME));
  supported_features_.push_back(
      std::make_shared<MediaPlayerSupportedFeature>(REMOTE_MODE));
  HomeAssistantTVMediaPlayer::setup();
}

std::string HomeAssistantTVRokuMediaPlayer::stringForRemoteCommand(
    MediaPlayerTVRemoteCommand command) {
  switch (command) {
    case UP:
      return "up";
    case DOWN:
      return "down";
    case LEFT:
      return "left";
    case RIGHT:
      return "right";
    case SELECT:
      return "select";
    case BACK:
      return "back";
    case HOME:
      return "home";
    case POWER:
      return "power";
    case VOLUME_UP:
      return "volume_up";
    case VOLUME_DOWN:
      return "volume_down";
    case PAUSE_COMMAND:
      return "pause";
  }
  return "";
}

void HomeAssistantTVRokuMediaPlayer::tvRemoteCommand(
    MediaPlayerTVRemoteCommand command) {
  std::string remoteName = entity_id_.substr(12).insert(0, "remote");
  auto commandString = stringForRemoteCommand(command);
  ESP_LOGI(TAG, "tvRemoteCommand: %s, %s", commandString.c_str(),
           remoteName.c_str());
  call_homeassistant_service("remote.send_command",
                             {
                                 {"entity_id", remoteName},
                                 {"command", commandString.c_str()},
                             });
}

void HomeAssistantTVRokuMediaPlayer::increaseVolume() {
  tvRemoteCommand(VOLUME_UP);
}

void HomeAssistantTVRokuMediaPlayer::decreaseVolume() {
  tvRemoteCommand(VOLUME_DOWN);
}

}  // namespace homeassistant_media_player
}  // namespace esphome
