#include "homeThingMenuNowPlayingOptionMenu.h"
#ifdef USE_MEDIA_PLAYER_GROUP

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
  int i_offset = 0;
  for (int i = 0; i < max_index; i++) {
    if (i - i_offset >= max_index) {
      return out;
    }
    auto feature = *(supported_features[i].get());
    ESP_LOGD(
        TAG, "get_supported_feature_options: %s index %d of %d, state: %d - %s",
        player->get_name().c_str(), i, max_index, player->playerState,
        homeassistant_media_player::supported_feature_string(feature).c_str());
    if (feature == homeassistant_media_player::TURN_ON &&
        player->playerState > homeassistant_media_player::RemotePlayerState::
                                  PowerOffRemotePlayerState) {
      i_offset++;
      max_index++;
      continue;
    } else if (feature == homeassistant_media_player::TURN_OFF &&
               player->playerState <=
                   homeassistant_media_player::RemotePlayerState::
                       PowerOffRemotePlayerState) {
      i_offset++;
      max_index++;
      continue;
    }
    auto newItem = CircleOptionMenuItem();
    newItem.position = static_cast<CircleOptionMenuPosition>(i - i_offset);
    newItem.feature = feature;
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
#endif
