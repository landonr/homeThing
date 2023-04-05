#pragma once
#include <memory>
#include <string>
#include <vector>
#include "esphome/components/api/custom_api_device.h"
#include "esphome/core/component.h"

namespace esphome {
namespace media_player_source {

class MediaPlayerSourceItem {
 public:
  MediaPlayerSourceItem(std::string new_name, std::string new_media_content_id,
                        std::string new_media_type)
      : name_(new_name),
        media_content_id_(new_media_content_id),
        media_type_(new_media_type) {}
  std::string get_name() { return name_; }
  std::string get_media_content_id() { return media_content_id_; }
  std::string get_media_type() { return media_type_; }
  void set_name(const std::string& name) { name_ = name; }
  void set_media_content_id(const std::string& media_content_id) {
    media_content_id_ = media_content_id;
  }
  void set_media_type(const std::string& media_type) {
    media_type_ = media_type;
  }

 protected:
  std::string name_;
  std::string media_content_id_;
  std::string media_type_;
};

class MediaPlayerSourceBase : public Component, public EntityBase {
 public:
  void set_entity_id(const std::string& entity_id) { entity_id_ = entity_id; }
  std::string get_entity_id() { return entity_id_; }
  float get_setup_priority() const override { return setup_priority::LATE; }
  std::vector<std::shared_ptr<MediaPlayerSourceItem>> get_sources() {
    return sources_;
  }

 protected:
  std::string entity_id_;
  std::vector<std::shared_ptr<MediaPlayerSourceItem>> sources_{nullptr};
};

}  // namespace media_player_source
}  // namespace esphome
