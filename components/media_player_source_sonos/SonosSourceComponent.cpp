#include "SonosSourceComponent.h"
#include "esphome/core/log.h"

namespace esphome {
namespace media_player_source_sonos {

static const char* const TAG = "media.player.source.sonos";

void SonosSourceComponent::setup() {
  subscribe_homeassistant_state(&SonosSourceComponent::sonos_favorites_changed,
                                "sensor.sonos_favorites", "items");
}

void SonosSourceComponent::sonos_favorites_changed(std::string state) {
  ESP_LOGI(TAG, "Sonos Favorites changes to %s", state.c_str());
  auto sources = MediaPlayerSourceBase::parseJsonDictionary(state);
  sources_ = sources;
}
}  // namespace media_player_source_sonos
}  // namespace esphome