#include "SonosSourceComponent.h"
#include "esphome/components/json/json_util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace media_player_source_sonos {

static const char* const TAG = "media.player.source.sonos";

void SonosSourceComponent::setup() {
  subscribe_homeassistant_state(&SonosSourceComponent::sonos_favorites_changed,
                                "sensor.sonos_favorites", "items");
}

std::vector<std::shared_ptr<media_player_source::MediaPlayerSourceItem>>
SonosSourceComponent::parseJsonObject(std::string state) {
  StaticJsonDocument<3072> doc;
  DeserializationError err = deserializeJson(doc, state);
  std::vector<std::shared_ptr<media_player_source::MediaPlayerSourceItem>>
      sources;
  if (err) {
    ESP_LOGE("JSON", "deserializeJson() failed: ");
    ESP_LOGE("JSON", err.c_str());
    return sources;
  }
  JsonObject array = doc.as<JsonObject>();
  for (JsonPair v : array) {
    std::string value = v.value().as<std::string>();
    std::string key = v.key().c_str();
    ESP_LOGD("group", "new JSON key value %s %s", key.c_str(), value.c_str());
    auto newsource =
        std::make_shared<media_player_source::MediaPlayerSourceItem>(
            value, key,
            media_player_source::FavoriteItemIDRemotePlayerSourceType);
    sources.push_back(newsource);
  }
  return sources;
}

void SonosSourceComponent::sonos_favorites_changed(std::string state) {
  ESP_LOGI(TAG, "Sonos Favorites changes to %s", state.c_str());
  auto sources = parseJsonObject(state);
  sources_ = sources;
}
}  // namespace media_player_source_sonos
}  // namespace esphome