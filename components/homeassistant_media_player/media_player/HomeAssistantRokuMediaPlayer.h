#pragma once

#include <memory>
#include <string>
#include <vector>
#include "HomeAssistantBaseMediaPlayer.h"

namespace esphome {
namespace homeassistant_media_player {

class HomeAssistantRokuMediaPlayer : public HomeAssistantBaseMediaPlayer {
 public:
  HomeAssistantBaseMediaPlayer* speaker;

  void setup();

  void player_source_changed(std::string state);

  void player_source_list_changed(std::string state);
  void tvRemoteCommand(std::string command);
  void increaseVolume();
  void decreaseVolume();
};
}  // namespace homeassistant_media_player
}  // namespace esphome
