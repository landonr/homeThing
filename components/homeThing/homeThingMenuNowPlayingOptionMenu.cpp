#include "homeThingMenuNowPlayingOptionMenu.h"

namespace esphome {
namespace homething_menu_base {

void HomeThingMenuNowPlayingOptionMenu::set_active_menu(
    const HomeThingOptionMenuType type,
    homeassistant_media_player::HomeAssistantBaseMediaPlayer* player) {
  switch (type) {
    case noOptionMenu:
      clear_active_menu();
      break;
    case volumeOptionMenu:
    case playingNewSourceMenu:
      active_menu_ = new HomeThingOptionMenu(type);
      break;
    case tvOptionMenu:
    case speakerOptionMenu:
      auto active_menu = new HomeThingOptionMenu(type);
      auto circleOptions = get_supported_feature_options(player);
      active_menu->circle_options = circleOptions;
      active_menu_ = active_menu;
      break;
  }
}

std::vector<CircleOptionMenuItem>
HomeThingMenuNowPlayingOptionMenu::get_supported_feature_options(
    homeassistant_media_player::HomeAssistantBaseMediaPlayer* player) {
  auto supported_features = player->get_option_menu_features();
  auto out = std::vector<CircleOptionMenuItem>();
  auto max_index = min(static_cast<int>(supported_features.size()), 5);
  for (int i = 0; i < max_index; i++) {
    auto element = *(supported_features[i].get());
    auto newItem = CircleOptionMenuItem();
    newItem.position = static_cast<CircleOptionMenuPosition>(i);
    newItem.feature = element;
    out.push_back(newItem);
  }
  return out;
}

homeassistant_media_player::MediaPlayerSupportedFeature*
HomeThingMenuNowPlayingOptionMenu::tap_option_menu(
    CircleOptionMenuPosition position,
    homeassistant_media_player::HomeAssistantBaseMediaPlayer* player) {
  auto supported_features = get_supported_feature_options(player);
  if (supported_features.size() > position) {
    ESP_LOGD(TAG, "tap_option_menu: %d - %s", static_cast<int>(position),
             homeassistant_media_player::supported_feature_string(
                 supported_features[static_cast<int>(position)].feature)
                 .c_str());
    return &supported_features[static_cast<int>(position)].feature;
  }
  return nullptr;
}

}  // namespace homething_menu_base
}  // namespace esphome