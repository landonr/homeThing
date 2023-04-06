#include "SpotifySourceComponent.h"
#include "esphome/components/json/json_util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace media_player_source_spotify {

static const char* const TAG = "media.player.source.spotify";

void SpotifySourceComponent::setup() {
  subscribe_homeassistant_state(&SpotifySourceComponent::playlists_changed,
                                "sensor.playlists_sensor", "playlists");
}

std::vector<std::shared_ptr<media_player_source::MediaPlayerSourceItem>>
SpotifySourceComponent::parseJsonDictionary(std::string state,
                                            std::string nameKey,
                                            std::string valueKey) {
  StaticJsonDocument<3072> doc;
  DeserializationError err = deserializeJson(doc, state);
  std::vector<std::shared_ptr<media_player_source::MediaPlayerSourceItem>>
      sources;
  if (err) {
    ESP_LOGE("JSON", "deserializeJson() failed: ");
    ESP_LOGE("JSON", err.c_str());
    return sources;
  }
  JsonArray array = doc.as<JsonArray>();
  for (JsonVariant v : array) {
    std::string key = v[nameKey].as<std::string>();
    std::string value = v[valueKey].as<std::string>();
    ESP_LOGD("JSON", "new JSON object value %s %s", value.c_str(), key.c_str());
    auto newsource =
        std::make_shared<media_player_source::MediaPlayerSourceItem>(
            key, value,
            media_player_source::MediaPlayerSourceType::
                PlaylistRemotePlayerSourceType);
    sources.push_back(newsource);
  }
  return sources;
}

void SpotifySourceComponent::stripUnicode(std::string* str) {
  str->erase(remove_if(str->begin(), str->end(),
                       [](char c) { return !(c >= 0 && c < 128); }),
             str->end());
}

void SpotifySourceComponent::playlists_changed(std::string state) {
  stripUnicode(&state);
  ESP_LOGI(TAG, "Spotify playlists changes to %s", state.c_str());
  auto sources = parseJsonDictionary(state, "name", "uri");
  sources_ = sources;
}
}  // namespace media_player_source_spotify
}  // namespace esphome