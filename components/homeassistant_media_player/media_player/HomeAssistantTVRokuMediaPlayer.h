#pragma once

#include <memory>
#include <string>
#include <vector>
#include "HomeAssistantTVMediaPlayer.h"

namespace esphome {
namespace homeassistant_media_player {

class HomeAssistantTVRokuMediaPlayer : public HomeAssistantTVMediaPlayer {
 public:
  void setup() override;

  void tvRemoteCommand(MediaPlayerTVRemoteCommand command) override;
  void increaseVolume() override;
  void decreaseVolume() override;

 private:
  std::string stringForRemoteCommand(MediaPlayerTVRemoteCommand command);
};
}  // namespace homeassistant_media_player
}  // namespace esphome
