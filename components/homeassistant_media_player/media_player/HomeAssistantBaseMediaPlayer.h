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

struct RemotePlayerStateUpdatedInterface {
 public:
  virtual void stateUpdated(RemotePlayerState state) {}
  virtual ~RemotePlayerStateUpdatedInterface() {}
};

class HomeAssistantBaseMediaPlayer : public api::CustomAPIDevice,
                                     public media_player::MediaPlayer,
                                     public Component {
 protected:
  DisplayUpdateInterface* display;
  RemotePlayerStateUpdatedInterface* stateCallback;

 public:
  std::string mediaTitle = "";
  std::string mediaArtist = "";
  RemotePlayerMediaSource mediaSource = NoRemotePlayerMediaSource;
  RemotePlayerState playerState = NoRemotePlayerState;
  std::vector<std::shared_ptr<MenuTitleSource>> sources;
  int index;
  RemotePlayerType playerType;
  void setup();
  void playSource(MenuTitleSource source);
  void playPause();
  void nextTrack();
  std::string mediaTitleString();
  std::string mediaSubtitleString();
  virtual void clearSource();
  void clearMedia();
  std::string entity_id_;
  void set_entity_id(const std::string& entity_id) { entity_id_ = entity_id; }

 private:
  void selectSource(MenuTitleSource source);
  void playMedia(MenuTitleSource source);
  void playerState_changed(std::string state);
};
}  // namespace homeassistant_media_player
}  // namespace esphome
