#pragma once

#include <memory>
#include <string>
#include <vector>
#include "DisplayUpdateInterface.h"
#include "MenuTitle.h"
#include "TextHelpers.h"
#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/media_player/media_player.h"

namespace esphome {
namespace homeassistant_media_player {

class HomeAssistantBaseMediaPlayer : public api::CustomAPIDevice,
                                     public media_player::MediaPlayer,
                                     public Component {
 protected:
  DisplayUpdateInterface* display;

 public:
  std::string mediaTitle = "";
  std::string mediaArtist = "";
  RemotePlayerMediaSource mediaSource = NoRemotePlayerMediaSource;
  RemotePlayerState playerState = NoRemotePlayerState;
  std::vector<std::shared_ptr<MenuTitleSource>> sources;
  int index;
  virtual RemotePlayerType get_player_type() { return player_type_; }
  void setupBase();
  void playSource(MenuTitleSource source);
  void playPause();
  void nextTrack();
  std::string mediaTitleString();
  std::string mediaSubtitleString();
  virtual void clearSource();
  void clearMedia();
  std::string entity_id_;
  void set_entity_id(const std::string& entity_id) { entity_id_ = entity_id; }
  bool is_muted() const override { return this->muted_; }

 private:
  void selectSource(MenuTitleSource source);
  void playMedia(MenuTitleSource source);
  void playerState_changed(std::string state);
  RemotePlayerType player_type_;
  bool muted_ = false;
};
}  // namespace homeassistant_media_player
}  // namespace esphome
