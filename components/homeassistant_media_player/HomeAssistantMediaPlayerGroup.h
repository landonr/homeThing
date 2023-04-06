#pragma once
#include <memory>
#include <string>
#include <vector>
#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/media_player_source/MediaPlayerSourceBase.h"
#include "esphome/components/sensor/sensor.h"
#include "media_player/HomeAssistantBaseMediaPlayer.h"
#include "media_player/HomeAssistantSpeakerMediaPlayer.h"
#include "media_player/HomeAssistantTVMediaPlayer.h"
#include "media_player/JSONTextHelpers.h"

namespace esphome {
namespace homeassistant_media_player {

class SpeakerSetup {
 public:
  SpeakerSetup(std::string newEntityId, std::string newFriendlyName)
      : entity_id_(newEntityId), friendlyName(newFriendlyName) {}
  std::string entity_id_;
  std::string friendlyName;
};

class TVSetup {
 public:
  TVSetup(std::string newEntityId, std::string newFriendlyName,
          SpeakerSetup* newSoundBar)
      : entity_id_(newEntityId),
        friendlyName(newFriendlyName),
        soundBar(newSoundBar) {}
  std::string entity_id_;
  std::string friendlyName;
  SpeakerSetup* soundBar;
};

class HomeAssistantMediaPlayerGroup : public api::CustomAPIDevice,
                                      public Component,
                                      public sensor::Sensor {
 public:
  HomeAssistantBaseMediaPlayer* active_player_ = NULL;
  std::vector<HomeAssistantBaseMediaPlayer*> media_players_;
  bool playerSearchFinished = false;
  std::string playingNewSourceText = "";
  int loadedPlayers = 0;
  HomeAssistantBaseMediaPlayer* newSpeakerGroupParent = NULL;

  int totalPlayers() { return media_players_.size(); }
  void selectFirstActivePlayer();
  void setup();
  HomeAssistantBaseMediaPlayer* get_active_player() { return active_player_; }
  void findActivePlayer(bool background = false);
  bool selectMediaPlayers(HomeAssistantBaseMediaPlayer* selected_media_player);
  void setActivePlayer(HomeAssistantBaseMediaPlayer* newActivePlayer);
  // void setup(std::vector<TVSetup> newTVSetups,
  //            std::vector<SpeakerSetup> newSpeakerSetups);
  void register_media_player(HomeAssistantBaseMediaPlayer* new_media_player);
  std::vector<std::string> groupNames();
  void stripUnicode(std::string* str);
  void increaseSpeakerVolume();
  void decreaseSpeakerVolume();
  bool mediaShuffling();
  void toggle_shuffle();
  void toggle_mute();
  std::string shuffle_string();
  std::string muteString();
  double getVolumeLevel();
  void selectGroup(HomeAssistantBaseMediaPlayer* selected_group_parent,
                   int menuIndex);
  bool updateMediaPosition();
  void selectNextMediaPlayer();
  std::string playTitleString();
  std::string mediaTitleString();
  std::string mediaSubtitleString();
  void sendActivePlayerRemoteCommand(MediaPlayerTVRemoteCommand command);
  void call_feature(MediaPlayerSupportedFeature feature);
  std::vector<media_player_source::MediaPlayerSourceBase*>
  activePlayerSources();
  void syncActivePlayer(RemotePlayerState state);

  void playSource(media_player_source::MediaPlayerSourceItem source);
  float get_setup_priority() const override { return setup_priority::LATE; }
  void set_active_player_source_index(int active_player_source_index) {
    active_player_source_index_ = active_player_source_index;
  }
  int get_active_player_source_index() { return active_player_source_index_; }

 private:
  void state_updated(RemotePlayerState state);
  bool sync_active_player = false;
  int active_player_source_index_ = -1;
  // bool sonos_active = false;
  // bool spotify_active = false;
};

}  // namespace homeassistant_media_player
}  // namespace esphome
