#pragma once
#include <memory>
#include <string>
#include <vector>
#include "esphome/components/api/custom_api_device.h"
#include "esphome/core/component.h"

namespace esphome {
namespace media_player_source {

enum MediaPlayerSourceType {
  MusicRemotePlayerSourceType,
  FavoriteItemIDRemotePlayerSourceType,
  SourceRemotePlayerSourceType,
  PlaylistRemotePlayerSourceType
};

class MediaPlayerSourceItem {
 public:
  MediaPlayerSourceItem(std::string new_name, std::string new_media_content_id,
                        MediaPlayerSourceType new_media_type)
      : name_(new_name),
        media_content_id_(new_media_content_id),
        media_type_(new_media_type) {}
  std::string get_name() { return name_; }
  std::string get_media_content_id() { return media_content_id_; }
  MediaPlayerSourceType get_media_type() { return media_type_; }
  void set_name(const std::string& name) { name_ = name; }
  void set_media_content_id(const std::string& media_content_id) {
    media_content_id_ = media_content_id;
  }
  void set_media_type(const MediaPlayerSourceType& media_type) {
    media_type_ = media_type;
  }

  std::string sourceTypeString() {
    switch (media_type_) {
      case MusicRemotePlayerSourceType:
        return "music";
      case FavoriteItemIDRemotePlayerSourceType:
        return "favorite_item_id";
      case SourceRemotePlayerSourceType:
        return "source";
      case PlaylistRemotePlayerSourceType:
        return "playlist";
    }
    return "";
  }

 protected:
  std::string name_ = "no";
  std::string media_content_id_ = "no";
  MediaPlayerSourceType media_type_;
};

class MediaPlayerSourceBase : public EntityBase {
 public:
  // void set_entity_id(const std::string& entity_id) { entity_id_ = entity_id; }
  // std::string get_entity_id() { return entity_id_; }
  std::vector<std::shared_ptr<MediaPlayerSourceItem>> get_sources() {
    return sources_;
  }

  std::vector<std::shared_ptr<media_player_source::MediaPlayerSourceItem>>
  parseJsonArray(std::string state);

  std::vector<std::shared_ptr<media_player_source::MediaPlayerSourceItem>>
  parseJsonDictionary(std::string state);

  std::vector<std::shared_ptr<media_player_source::MediaPlayerSourceItem>>
  parseJsonObject(std::string state, std::string nameKey, std::string valueKey);

 protected:
  // std::string entity_id_;
  std::vector<std::shared_ptr<MediaPlayerSourceItem>> sources_ = {};
};

class MediaPlayerSourceAPI : public MediaPlayerSourceBase,
                             public Component,
                             public api::CustomAPIDevice {
 public:
  void set_entity_id(const std::string& entity_id) { entity_id_ = entity_id; }
  std::string get_entity_id() { return entity_id_; }
  float get_setup_priority() const override { return setup_priority::LATE; }

 protected:
  std::string entity_id_;
};

class MediaPlayerSourceInternal : public MediaPlayerSourceBase {
 public:
  void set_sources(
      const std::vector<std::shared_ptr<MediaPlayerSourceItem>>& sources) {
    sources_ = sources;
  }
};

}  // namespace media_player_source
}  // namespace esphome
