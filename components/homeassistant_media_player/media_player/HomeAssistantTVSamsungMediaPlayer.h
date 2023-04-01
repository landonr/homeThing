#pragma once

#include <memory>
#include <string>
#include <vector>
#include "HomeAssistantTVMediaPlayer.h"

namespace esphome {
namespace homeassistant_media_player {

class HomeAssistantTVSamsungMediaPlayer : public HomeAssistantTVMediaPlayer {
 public:
  void setup() override;
  void tvRemoteCommand(MediaPlayerTVRemoteCommand command) override;

 private:
  void subscribe_media_artist() override;
  void subscribe_source() override;
  std::string stringForRemoteCommand(MediaPlayerTVRemoteCommand command);
};
}  // namespace homeassistant_media_player
}  // namespace esphome
