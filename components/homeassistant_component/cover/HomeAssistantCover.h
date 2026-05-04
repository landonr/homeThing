#include <map>
#include <string>
#include "esphome/components/cover/cover.h"
#include "esphome/components/homeassistant_component/HomeAssistantComponent.h"

namespace esphome {
namespace homeassistant_cover {
static const char* const TAG = "homeassistant.cover";

class HomeAssistantCover
    : public cover::Cover,
      public homeassistant_component::HomeAssistantComponent {

  void publish_api_state(cover::CoverCall call) {
    next_api_publish_ = false;
    ignore_api_updates_with_seconds(2);
    ESP_LOGI(TAG, "publish_api_state: '%s'", this->get_name().c_str());
    std::map<std::string, std::string> data = {{"entity_id", entity_id_}};
    if (call.get_stop()) {
      ESP_LOGI(TAG, "control: '%s' stopping", this->get_name().c_str());
      std::map<std::string, std::string> data = {{"entity_id", entity_id_}};
      call_homeassistant_service("cover.stop", data);
    } else if (call.get_toggle().has_value()) {
      if (this->current_operation != cover::COVER_OPERATION_IDLE) {
        ESP_LOGI(TAG, "control: '%s' toggle", this->get_name().c_str());
        std::map<std::string, std::string> data = {{"entity_id", entity_id_}};
        call_homeassistant_service("cover.toggle", data);
      } else {
        if (this->position == cover::COVER_CLOSED) {
          // this->last_operation_ == cover::COVER_OPERATION_CLOSING) {
          ESP_LOGI(TAG, "control: '%s' opening", this->get_name().c_str());
          std::map<std::string, std::string> data = {{"entity_id", entity_id_}};
          call_homeassistant_service("cover.open_cover", data);
        } else {
          ESP_LOGI(TAG, "control: '%s' closing", this->get_name().c_str());
          std::map<std::string, std::string> data = {{"entity_id", entity_id_}};
          call_homeassistant_service("cover.close_cover", data);
        }
      }
    } else if (call.get_position().has_value()) {
      auto pos = *call.get_position();
      if (pos == this->position) {
        // already at target
      } else {
        ESP_LOGI(TAG, "control: '%s' stopping", this->get_name().c_str());
        std::map<std::string, std::string> data = {
            {"entity_id", entity_id_}, {"position", to_string(pos)}};
        call_homeassistant_service("cover.stop", data);
      }
    }
  }

  void state_changed(std::string state) {
    next_api_publish_ = false;
    if (can_update_from_api()) {
      if (strcmp(state.c_str(), "open") == 0) {
        this->make_call().set_command_open().perform();
        ESP_LOGI(TAG, "state_changed: '%s' to open %s",
                 this->get_name().c_str(), state.c_str());
      } else if (strcmp(state.c_str(), "closed") == 0) {
        this->make_call().set_command_close().perform();
        ESP_LOGI(TAG, "state_changed: '%s' to closed %s",
                 this->get_name().c_str(), state.c_str());
      } else {
        ESP_LOGW(TAG, "state_changed: '%s' to unknown %s",
                 this->get_name().c_str(), state.c_str());
      }
      this->publish_state();
    }
    next_api_publish_ = true;
  }

  void position_changed(std::string state) {
    next_api_publish_ = false;
    ESP_LOGI(TAG, "position_changed: '%s' to %s", this->get_name().c_str(),
             state.c_str());
    if (can_update_from_api()) {
      auto new_position = atof(state.c_str()) / 100.0f;
      this->make_call().set_position(new_position).perform();
      this->publish_state();
    }
    next_api_publish_ = true;
  }

  void setup() {
    ESP_LOGI(TAG, "'%s': Setup", get_name().c_str());
    subscribe_homeassistant_state(&HomeAssistantCover::state_changed,
                                  this->entity_id_);
    subscribe_homeassistant_state(&HomeAssistantCover::position_changed,
                                  this->entity_id_, "current_position");
  }

  void control(const cover::CoverCall& call) {
    if (call.get_stop()) {
      ESP_LOGI(TAG, "control: '%s', can update: %d, stopping",
               this->get_name().c_str(), can_update_from_api());
      this->publish_state();
    }
    if (call.get_toggle().has_value()) {
      if (this->current_operation != cover::COVER_OPERATION_IDLE) {
        ESP_LOGI(TAG, "control: '%s', can update: %d, toggle",
                 this->get_name().c_str(), can_update_from_api());
      } else {
        if (this->position == cover::COVER_CLOSED) {
          ESP_LOGI(TAG, "control: '%s', can update: %d, opening %f",
                   this->get_name().c_str(), can_update_from_api(),
                   this->position);
        } else {
          ESP_LOGI(TAG, "control: '%s', can update: %d, closing %f",
                   this->get_name().c_str(), can_update_from_api(),
                   this->position);
        }
      }
    }
    if (call.get_position().has_value()) {
      auto pos = *call.get_position();
      if (pos == this->position) {
        // already at target
      } else {
        ESP_LOGI(
            TAG,
            "control: '%s', can update: %d, setting position new: %f old: %f",
            this->get_name().c_str(), can_update_from_api(), pos,
            this->position);
        this->position = pos;
      }
    }
    this->publish_state();
    if (next_api_publish_) {
      publish_api_state(call);
    }
    this->state_callback_.call();
  }

  cover::CoverTraits get_traits() {
    auto traits = cover::CoverTraits();
    traits.set_supports_stop(true);
    traits.set_supports_position(true);
    traits.set_supports_toggle(true);
    return traits;
  }
};
}  // namespace homeassistant_cover
}  // namespace esphome
