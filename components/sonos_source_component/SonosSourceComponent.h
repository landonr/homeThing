#pragma once
#include <memory>
#include <string>
#include <vector>
#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/media_player_source/MediaPlayerSourceBase.h"

namespace esphome {
namespace sonos_source_component {

class SonosSourceComponent : public media_player_source::MediaPlayerSourceBase,
                             public api::CustomAPIDevice {
 public:
  void setup() override;

 private:
  void sonos_favorites_changed(std::string state);
  std::vector<std::shared_ptr<media_player_source::MediaPlayerSourceItem>>
  parseJsonObject(std::string state);
};

}  // namespace sonos_source_component
}  // namespace esphome
