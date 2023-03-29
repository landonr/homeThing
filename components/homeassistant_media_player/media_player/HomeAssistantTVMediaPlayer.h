#pragma once

#include <memory>
#include <string>
#include <vector>
#include "HomeAssistantBaseMediaPlayer.h"

namespace esphome {
namespace homeassistant_media_player {

enum MediaPlayerTVRemoteCommand {
  UP,
  DOWN,
  LEFT,
  RIGHT,
  SELECT,
  BACK,
  HOME,
  POWER,
  VOLUME_UP,
  VOLUME_DOWN,
  PAUSE_COMMAND
};

class HomeAssistantTVMediaPlayer : public HomeAssistantBaseMediaPlayer {
 public:
  HomeAssistantBaseMediaPlayer* speaker;

  void setup() override;

  media_player::MediaPlayerTraits get_traits() {
    auto traits = media_player::MediaPlayerTraits();
    traits.set_supports_pause(true);
    return traits;
  }

  virtual void tvRemoteCommand(MediaPlayerTVRemoteCommand command);
  void increaseVolume() override;
  void decreaseVolume() override;
  void control(const media_player::MediaPlayerCall& call);
  RemotePlayerType get_player_type() { return TVRemotePlayerType; }
  void set_soundbar(HomeAssistantBaseMediaPlayer* new_sound_bar) {
    speaker = new_sound_bar;
  }
  void player_source_changed(std::string state);

 protected:
  void subscribe_source() override;
  void sources_changed(std::string state) override;
  void subscribe_sources() override;
};
}  // namespace homeassistant_media_player
}  // namespace esphome
