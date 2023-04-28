#include "SpotifySourceComponent.h"
#include "esphome/core/log.h"

namespace esphome {
namespace media_player_source_spotify {

static const char* const TAG = "media.player.source.spotify";

void SpotifySourceComponent::setup() {
  subscribe_homeassistant_state(&SpotifySourceComponent::playlists_changed,
                                "sensor.playlists_sensor", "playlists");
}

void SpotifySourceComponent::stripUnicode(std::string* str) {
  str->erase(remove_if(str->begin(), str->end(),
                       [](char c) { return !(c >= 0 && c < 128); }),
             str->end());
}

void SpotifySourceComponent::playlists_changed(std::string state) {
  stripUnicode(&state);
  ESP_LOGI(TAG, "Spotify playlists changes to %s", state.c_str());
  auto sources = MediaPlayerSourceBase::parseJsonObject(state, "name", "uri");
  sources_ = sources;
}
}  // namespace media_player_source_spotify
}  // namespace esphome