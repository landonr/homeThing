#include "CustomSourceComponent.h"
#include <memory>
#include <vector>
#include "esphome/core/log.h"

namespace esphome {
namespace media_player_source_custom {

static const char* const TAG = "media.player.source.custom";

void CustomSourceComponent::setup() {}

void CustomSourceComponent::add_source(
    std::string name, std::string media_id,
    media_player_source::MediaPlayerSourceType media_type) {
  ESP_LOGI(TAG, "add_source %s", name.c_str());
  auto new_source =
      std::make_shared<media_player_source::MediaPlayerSourceItem>(
          name, media_id, media_type);
  sources_.push_back(new_source);
}
}  // namespace media_player_source_custom
}  // namespace esphome
