#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include "HomeAssistantBaseMediaPlayer.h"

namespace esphome {
namespace homeassistant_media_player {

class HomeAssistantSpeakerMediaPlayer : public HomeAssistantBaseMediaPlayer {
 public:
  std::vector<std::string> groupMembers;
  std::string mediaAlbumName = "";
  HomeAssistantBaseMediaPlayer* tv = NULL;

  void setup() override;
  void ungroup();
  void joinGroup(std::string newSpeakerName);
  void increaseVolume();
  void decreaseVolume();
  void updateVolumeLevel();
  void clearSource() override;
  media_player::MediaPlayerTraits get_traits();
  void control(const media_player::MediaPlayerCall& call);
  RemotePlayerType get_player_type() { return SpeakerRemotePlayerType; }

 private:
  void subscribe_sources() override {}
  void group_members_changed(std::string state) override;
  void sources_changed(std::string state) override {}
  void player_media_title_changed(std::string state);
  void player_media_artist_changed(std::string state);
  void media_album_changed(std::string state);
  void media_source_changed(std::string state);
  void tokenize(std::string const& str, std::string delim,
                std::vector<std::string>* out);
  std::string filter(std::string str);
};
}  // namespace homeassistant_media_player
}  // namespace esphome
