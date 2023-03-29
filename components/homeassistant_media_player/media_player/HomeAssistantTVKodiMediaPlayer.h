#pragma once

#include <memory>
#include <string>
#include <vector>
#include "HomeAssistantTVMediaPlayer.h"

namespace esphome {
namespace homeassistant_media_player {

class HomeAssistantTVKodiMediaPlayer : public HomeAssistantTVMediaPlayer {
 public:
  void setup() override;

  void tvRemoteCommand(MediaPlayerTVRemoteCommand command) override;

 protected:
  std::string stringForRemoteCommand(MediaPlayerTVRemoteCommand command);
  void subscribe_source() override;
};
}  // namespace homeassistant_media_player
}  // namespace esphome
