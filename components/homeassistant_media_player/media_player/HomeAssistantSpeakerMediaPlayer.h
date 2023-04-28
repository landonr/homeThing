#pragma once

#include <memory>
#include <string>
#include <vector>
#include "HomeAssistantBaseMediaPlayer.h"

namespace esphome {
namespace homeassistant_media_player {

class HomeAssistantSpeakerMediaPlayer : public HomeAssistantBaseMediaPlayer {
 public:
  // std::vector<std::string> groupMembers;
  std::string media_album_name = "";
  HomeAssistantBaseMediaPlayer* tv{nullptr};

  void setup() override;
  void ungroup();
  void joinGroup(std::string newSpeakerName);
  void updateVolumeLevel() override;
  void clearSource() override;
  media_player::MediaPlayerTraits get_traits();
  void control(const media_player::MediaPlayerCall& call);
  RemotePlayerType get_player_type() { return SpeakerRemotePlayerType; }
  void set_tv(HomeAssistantBaseMediaPlayer* new_tv) { tv = new_tv; }
  virtual HomeAssistantBaseMediaPlayer* get_parent_media_player() {
    if (tv != NULL)
      return tv;
    return HomeAssistantBaseMediaPlayer::get_parent_media_player();
  }

 private:
  void subscribe_source() override;
  void media_album_changed(std::string state);
  void media_content_id_changed(std::string state);
};
}  // namespace homeassistant_media_player
}  // namespace esphome
