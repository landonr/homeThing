#pragma once
#include <memory>
#include <string>
#include <vector>
#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/media_player_source/MediaPlayerSourceBase.h"

namespace esphome {
namespace media_player_source_sonos {

class SonosSourceComponent : public media_player_source::MediaPlayerSourceAPI {
 public:
  void setup() override;

 private:
  void sonos_favorites_changed(std::string state);
};

}  // namespace media_player_source_sonos
}  // namespace esphome
