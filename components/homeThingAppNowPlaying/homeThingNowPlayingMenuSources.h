#pragma once

#include <vector>
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/components/homeThing/homeThingOptionMenu.h"
#include "esphome/components/homeThingApp/homeThingApp.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"

namespace esphome {
namespace homething_menu_now_playing {

class homeThingNowPlayingMenuSources {
 public:
  static homething_menu_app::NavigationCoordination pop_menu(
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          media_player_group) {
    const auto index = media_player_group->get_active_player_source_index();
    {
      if (index == -1) {
        return homething_menu_app::NavigationCoordination::
            NavigationCoordinationPop;
      }
      media_player_group->set_active_player_source_index(-1);
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationReload;
    }
  }

  static void sourceMenuTitles(
      std::vector<homething_menu_base::MenuTitleBase*>* menu_titles,
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          media_player_group) {
    if (media_player_group == nullptr) {
      return;
    }
    if (media_player_group->active_player_ == nullptr) {
      media_player_group->selectNextMediaPlayer();
      if (media_player_group->active_player_ == nullptr) {
        return;
      }
    }
    const auto sources = media_player_group->activePlayerSources();
    const auto index = media_player_group->get_active_player_source_index();
    if (index == -1 && sources->size() > 1) {
      activePlayerSourceTitles(sources, menu_titles);
      return;
    } else if (index == -1 && sources->size() == 1) {
      auto playerSources = (*sources)[0]->get_sources();
      activePlayerSourceItemTitles(playerSources, menu_titles);
      return;
    } else if (sources->size() > 1) {
      auto playerSources = (*sources)[index]->get_sources();
      activePlayerSourceItemTitles(playerSources, menu_titles);
      return;
    }
  }

  static homething_menu_app::NavigationCoordination select_source_menu(
      int index,
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          media_player_group,
      HomeThingMenuNowPlayingOptionMenu* circle_menu,
      NowPlayingMenuState* menu_state) {
    const char* const TAG = "homething.nowplaying.control";
    // const auto sourceTitleState = static_cast<MenuTitleSource*>(activeMenuTitle);
    // auto source = sourceTitleState->media_source_;
    auto player_source_index =
        media_player_group->get_active_player_source_index();
    if (player_source_index == -1) {
      ESP_LOGI(TAG, "select_source_menu: set player source index %d", index);
      media_player_group->set_active_player_source_index(index);
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationReload;
    }

    const auto sources = media_player_group->activePlayerSources();
    if (sources->size() == 0) {
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationReload;
    }
    int active_player_source_index =
        sources->size() == 1 ? 0 : player_source_index;
    auto playerSources = (*sources)[active_player_source_index]->get_sources();
    media_player_source::MediaPlayerSourceItem* source =
        (*playerSources)[index];
    media_player_group->playSource(source);
    ESP_LOGI(TAG, "select_source_menu: set player source index %d",
             active_player_source_index);
    circle_menu->set_active_menu(playingNewSourceMenu,
                                 media_player_group->active_player_);
    (*menu_state) = NOW_PLAYING_MENU_STATE_NOW_PLAYING;
    return homething_menu_app::NavigationCoordination::
        NavigationCoordinationReload;
  }
};
}  // namespace homething_menu_now_playing
}  // namespace esphome