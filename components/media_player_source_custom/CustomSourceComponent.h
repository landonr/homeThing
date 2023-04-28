#pragma once
#include <string>
#include "esphome/components/media_player_source/MediaPlayerSourceBase.h"

namespace esphome {
namespace media_player_source_custom {

class CustomSourceComponent : public media_player_source::MediaPlayerSourceBase,
                              public Component {
 public:
  void setup() override;

  void add_source(std::string name, std::string media_id,
                  media_player_source::MediaPlayerSourceType media_type);
};
}  // namespace media_player_source_custom
}  // namespace esphome
