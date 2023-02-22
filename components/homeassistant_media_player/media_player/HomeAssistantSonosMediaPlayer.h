#pragma once

#include <memory>
#include <string>
#include <vector>
#include "HomeAssistantBaseMediaPlayer.h"

namespace esphome {
namespace homeassistant_media_player {

class HomeAssistantSonosMediaPlayer : public HomeAssistantBaseMediaPlayer {
 public:
  float volumeStep = 0.04;
  bool shuffle = false;
  std::vector<std::string> groupMembers;
  std::string mediaPlaylist = "";
  std::string mediaAlbumName = "";
  int mediaDuration = -1;
  int mediaPosition = -1;
  HomeAssistantBaseMediaPlayer* tv = NULL;

  void setup() override;
  void ungroup();
  void joinGroup(std::string newSpeakerName);
  void toggleShuffle();
  void toggleMute();
  void increaseVolume();
  void decreaseVolume();
  void updateVolumeLevel();
  virtual void clearSource();
  media_player::MediaPlayerTraits get_traits();
  void control(const media_player::MediaPlayerCall& call);
  RemotePlayerType get_player_type() { return SpeakerRemotePlayerType; }
  bool is_muted() const override { return this->muted_; }

 private:
  bool muted_ = false;
  void player_media_title_changed(std::string state);
  void player_media_artist_changed(std::string state);
  void speaker_volume_changed(std::string state);
  void speaker_muted_changed(std::string state);
  void shuffle_changed(std::string state);
  void playlist_changed(std::string state);
  void media_album_changed(std::string state);
  void group_members_changed(std::string state);
  void media_duration_changed(std::string state);
  void media_position_changed(std::string state);
  void media_source_changed(std::string state);
};
}  // namespace homeassistant_media_player
}  // namespace esphome
