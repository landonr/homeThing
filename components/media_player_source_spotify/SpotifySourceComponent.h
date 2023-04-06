#pragma once
#include <memory>
#include <string>
#include <vector>
#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/media_player_source/MediaPlayerSourceBase.h"

namespace esphome {
namespace media_player_source_spotify {

class SpotifySourceComponent
    : public media_player_source::MediaPlayerSourceBase,
      public api::CustomAPIDevice {
 public:
  void setup() override;

 private:
  void playlists_changed(std::string state);
  std::vector<std::shared_ptr<media_player_source::MediaPlayerSourceItem>>
  parseJsonDictionary(std::string state, std::string nameKey,
                      std::string valueKey);
  void stripUnicode(std::string* str);
};
}  // namespace media_player_source_spotify
}  // namespace esphome
