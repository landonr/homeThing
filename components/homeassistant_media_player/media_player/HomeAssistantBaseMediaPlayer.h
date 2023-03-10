#pragma once

#include <memory>
#include <string>
#include <vector>
#include "MediaPlayerSource.h"
#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/media_player/media_player.h"

namespace esphome {
namespace homeassistant_media_player {

enum RemotePlayerType { TVRemotePlayerType, SpeakerRemotePlayerType };

enum RemotePlayerState {
  NoRemotePlayerState,
  UnavailableRemotePlayerState,
  PowerOffRemotePlayerState,
  StoppedRemotePlayerState,
  PausedRemotePlayerState,
  PlayingRemotePlayerState
};

class HomeAssistantBaseMediaPlayer : public api::CustomAPIDevice,
                                     public media_player::MediaPlayer,
                                     public Component {
 protected:
  bool muted_ = false;

 public:
  std::string mediaTitle = "";
  std::string mediaArtist = "";
  RemotePlayerMediaSource mediaSource = NoRemotePlayerMediaSource;
  RemotePlayerState playerState = NoRemotePlayerState;
  std::vector<std::shared_ptr<MediaPlayerSource>> sources;
  int index;
  virtual RemotePlayerType get_player_type() { return player_type_; }
  void setupBase();
  void playSource(MediaPlayerSource source);
  void playPause();
  void nextTrack();
  std::string mediaTitleString();
  std::string mediaSubtitleString();
  virtual void clearSource();
  void clearMedia();
  std::string entity_id_;
  void set_entity_id(const std::string& entity_id) { entity_id_ = entity_id; }
  std::string get_entity_id() { return entity_id_; }
  bool is_muted() const override { return this->muted_; }

 private:
  void selectSource(MediaPlayerSource source);
  void playMedia(MediaPlayerSource source);
  void playerState_changed(std::string state);
  RemotePlayerType player_type_;
};
}  // namespace homeassistant_media_player
}  // namespace esphome
