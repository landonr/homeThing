#pragma once

#include <memory>
#include <string>
#include <vector>
#include "HomeAssistantBaseMediaPlayer.h"

namespace esphome {
namespace homeassistant_media_player {

class HomeAssistantSpeakerMediaPlayer : public HomeAssistantBaseMediaPlayer {
 public:
  std::vector<std::string> groupMembers;
  std::string media_album_name = "";
  HomeAssistantBaseMediaPlayer* tv = NULL;

  void setup() override;
  void ungroup();
  void joinGroup(std::string newSpeakerName);
  void updateVolumeLevel() override;
  void clearSource() override;
  media_player::MediaPlayerTraits get_traits();
  void control(const media_player::MediaPlayerCall& call);
  RemotePlayerType get_player_type() { return SpeakerRemotePlayerType; }

 private:
  void subscribe_source() override;
  void group_members_changed(std::string state) override;
  void media_album_changed(std::string state);
  void media_content_id_changed(std::string state);
  void tokenize(std::string const& str, std::string delim,
                std::vector<std::string>* out);
  std::string filter(std::string str);
};
}  // namespace homeassistant_media_player
}  // namespace esphome
