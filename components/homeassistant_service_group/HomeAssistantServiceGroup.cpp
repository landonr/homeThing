#include "HomeAssistantServiceGroup.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_service_group {

static const char* const TAG = "homeassistant.service";

void HomeAssistantServiceGroup::register_service_call(
    HomeAssistantServiceCommand* new_service) {
  services.push_back(new_service);
  ESP_LOGI(TAG, "Service added %s",
           new_service->get_text<std::string>().c_str());
}

bool HomeAssistantServiceGroup::select_service(int index) {
  if (index >= 0 && index < services.size()) {
    HomeAssistantServiceCommand* service = services[index];
    ESP_LOGI(TAG, "Service called %s",
             service->get_text<std::string>().c_str());
    service->on_command();
    return false;
  }
  return true;
}

}  // namespace homeassistant_service_group
}  // namespace esphome
