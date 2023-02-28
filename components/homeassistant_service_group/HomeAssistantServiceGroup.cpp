#include "HomeAssistantServiceGroup.h"

namespace esphome {
namespace homeassistant_service_group {

static const char* const TAG = "homeassistant.service";

void HomeAssistantServiceGroup::register_service_call(
    HomeAssistantServiceCommand* new_service) {
  services.push_back(new_service);
  ESP_LOGI(TAG, "Service added %s",
           new_service->get_text<std::string>().c_str());
}

std::vector<std::shared_ptr<MenuTitleBase>>
HomeAssistantServiceGroup::sceneTitleStrings() {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  for (auto& service : services) {
    ESP_LOGD(TAG, "MENU Service %s", service->get_text<std::string>().c_str());
    std::string service_text = service->get_text<std::string>();
    out.push_back(std::make_shared<MenuTitleBase>(service_text, "2",
                                                  NoMenuTitleRightIcon));
  }
  return out;
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
