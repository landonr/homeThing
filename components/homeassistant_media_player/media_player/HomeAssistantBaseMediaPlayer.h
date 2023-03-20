#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "MediaPlayerSource.h"
#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/media_player/media_player.h"

namespace esphome {
namespace homeassistant_media_player {

enum MediaPlayerSupportedFeature {
  PAUSE = 1,
  SEEK = 2,
  VOLUME_SET = 4,
  VOLUME_MUTE = 8,
  PREVIOUS_TRACK = 16,
  NEXT_TRACK = 32,
  TURN_ON = 128,
  TURN_OFF = 256,
  PLAY_MEDIA = 512,
  VOLUME_STEP = 1024,
  SELECT_SOURCE = 2048,
  STOP = 4096,
  CLEAR_PLAYLIST = 8192,
  PLAY = 16384,
  SHUFFLE_SET = 32768,
  SELECT_SOUND_MODE = 65536,
  BROWSE_MEDIA = 131072,
  REPEAT_SET = 262144,
  GROUPING = 524288
};

static std::map<MediaPlayerSupportedFeature, std::string>
    supported_feature_string_map = {
        {PAUSE, "PAUSE"},
        {SEEK, "SEEK"},
        {VOLUME_SET, "VOLUME_SET"},
        {VOLUME_MUTE, "VOLUME_MUTE"},
        {PREVIOUS_TRACK, "PREVIOUS_TRACK"},
        {NEXT_TRACK, "NEXT_TRACK"},
        {TURN_ON, "TURN_ON"},
        {TURN_OFF, "TURN_OFF"},
        {PLAY_MEDIA, "PLAY_MEDIA"},
        {VOLUME_STEP, "VOLUME_STEP"},
        {SELECT_SOURCE, "SELECT_SOURCE"},
        {STOP, "STOP"},
        {CLEAR_PLAYLIST, "CLEAR_PLAYLIST"},
        {PLAY, "PLAY"},
        {SHUFFLE_SET, "SHUFFLE_SET"},
        {SELECT_SOUND_MODE, "SELECT_SOUND_MODE"},
        {BROWSE_MEDIA, "BROWSE_MEDIA"},
        {REPEAT_SET, "REPEAT_SET"},
        {GROUPING, "GROUPING"},
};

enum RemotePlayerType { TVRemotePlayerType, SpeakerRemotePlayerType };

enum RemotePlayerState {
  NoRemotePlayerState,
  UnavailableRemotePlayerState,
  PowerOffRemotePlayerState,
  StoppedRemotePlayerState,
  PausedRemotePlayerState,
  PlayingRemotePlayerState
};

class HomeAssistantBaseMediaPlayer : public media_player::MediaPlayer,
                                     public Component {
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
  virtual void clearSource() {}
  void clearMedia();
  std::string entity_id_;
  void set_entity_id(const std::string& entity_id) { entity_id_ = entity_id; }
  std::string get_entity_id() { return entity_id_; }
  bool is_muted() const override { return this->muted_; }
  bool is_shuffling() const { return this->shuffle_; }
  void toggle_shuffle();
  void toggle_mute();
  int mediaDuration = -1;
  int mediaPosition = -1;
  std::string playlist_title = "";

 protected:
  bool muted_ = false;
  bool shuffle_ = false;
  float volume_step_ = 0.04;

  virtual void subscribe_sources() = 0;
  virtual void sources_changed(std::string state) = 0;
  virtual void group_members_changed(std::string state) = 0;

  void call_homeassistant_service(
      const std::string& service_name,
      const std::map<std::string, std::string>& data) {
    api::HomeassistantServiceResponse resp;
    resp.service = service_name;
    for (auto& it : data) {
      api::HomeassistantServiceMap kv;
      kv.key = it.first;
      kv.value = it.second;
      resp.data.push_back(kv);
    }
    api::global_api_server->send_homeassistant_service_call(resp);
  }

 private:
  RemotePlayerType player_type_;
  std::vector<std::shared_ptr<MediaPlayerSupportedFeature>> supported_features_;

  void selectSource(MediaPlayerSource source);
  void playMedia(MediaPlayerSource source);
  void playerState_changed(std::string state);
  void player_supported_features_changed(std::string state);
  void subscribe_player_state();
  void subscribe_muted();
  void subscribe_shuffle();
  void subscribe_volume();
  void subscribe_media_position();
  void subscribe_playlist();
  void subscribe_group_members();
  void muted_changed(std::string state);
  void shuffle_changed(std::string state);
  void volume_changed(std::string state);
  void media_duration_changed(std::string state);
  void media_position_changed(std::string state);
  void playlist_changed(std::string state);
};
}  // namespace homeassistant_media_player
}  // namespace esphome
