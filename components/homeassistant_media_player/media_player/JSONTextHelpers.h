#pragma once

#include "esphome/components/json/json_util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_media_player {

static std::vector<std::shared_ptr<MediaPlayerSource>> parseJsonDictionary(
    std::string state, std::string nameKey, std::string valueKey) {
  StaticJsonDocument<3072> doc;
  DeserializationError err = deserializeJson(doc, state);
  std::vector<std::shared_ptr<MediaPlayerSource>> sources;
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
    auto newsource = std::make_shared<MediaPlayerSource>(
        PlaylistRemotePlayerSourceType, key, value);
    sources.push_back(newsource);
  }
  return sources;
}

static std::vector<std::shared_ptr<MediaPlayerSource>> parseJsonObject(
    std::string state) {
  StaticJsonDocument<3072> doc;
  DeserializationError err = deserializeJson(doc, state);
  std::vector<std::shared_ptr<MediaPlayerSource>> sources;
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
    auto newsource = std::make_shared<MediaPlayerSource>(
        FavoriteItemIDRemotePlayerSourceType, value, key);
    sources.push_back(newsource);
  }
  return sources;
}

static std::vector<std::shared_ptr<MediaPlayerSource>> parseJsonArray(
    std::string state, std::string entityId) {
  StaticJsonDocument<3072> doc;
  DeserializationError err = deserializeJson(doc, state);
  std::vector<std::shared_ptr<MediaPlayerSource>> sources;
  if (err) {
    ESP_LOGE("JSON", "deserializeJson() failed: ");
    ESP_LOGE("JSON", err.c_str());
    return sources;
  }
  JsonArray array = doc.as<JsonArray>();
  for (JsonVariant v : array) {
    std::string sourceName = v.as<std::string>();
    ESP_LOGD("JSON", "new JSON array value %s %s", sourceName.c_str(),
             entityId.c_str());
    auto newsource = std::make_shared<MediaPlayerSource>(
        SourceRemotePlayerSourceType, sourceName, entityId);
    sources.push_back(newsource);
  }
  return sources;
}

static std::string replaceAll(std::string str, const std::string& from,
                              const std::string& to) {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos +=
        to.length();  // Handles case where 'to' is a substring of 'from'
  }
  return str;
}
}  // namespace homeassistant_media_player
}  // namespace esphome