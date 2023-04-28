#include "MediaPlayerSourceBase.h"
#include "esphome/components/json/json_util.h"

namespace esphome {
namespace media_player_source {

static const char* const TAG = "media.player.source.base";

std::vector<std::shared_ptr<media_player_source::MediaPlayerSourceItem>>
MediaPlayerSourceBase::parseJsonArray(std::string state) {
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
    std::string sourceName = v.as<std::string>();
    ESP_LOGD("JSON", "new JSON array value %s %s", sourceName.c_str());
    auto newsource =
        std::make_shared<media_player_source::MediaPlayerSourceItem>(
            sourceName, sourceName,
            media_player_source::MediaPlayerSourceType::
                SourceRemotePlayerSourceType);
    sources.push_back(newsource);
  }
  return sources;
}

std::vector<std::shared_ptr<media_player_source::MediaPlayerSourceItem>>
MediaPlayerSourceBase::parseJsonObject(std::string state, std::string nameKey,
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

std::vector<std::shared_ptr<media_player_source::MediaPlayerSourceItem>>
MediaPlayerSourceBase::parseJsonDictionary(std::string state) {
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

}  // namespace media_player_source
}  // namespace esphome