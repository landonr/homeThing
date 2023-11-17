#pragma once

#include <string>
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
      homething_menu_base::MenuTitleSource::activePlayerSourceTitles(
          sources, menu_titles);
      return;
    } else if (index == -1 && sources->size() == 1) {
      auto playerSources = (*sources)[0]->get_sources();
      homething_menu_base::MenuTitleSource::activePlayerSourceItemTitles(
          playerSources, menu_titles);
      return;
    } else if (sources->size() > 1) {
      auto playerSources = (*sources)[index]->get_sources();
      homething_menu_base::MenuTitleSource::activePlayerSourceItemTitles(
          playerSources, menu_titles);
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

    auto player_source_index =
        media_player_group->get_active_player_source_index();
    const auto sources = media_player_group->activePlayerSources();
    if (player_source_index == -1 && sources->size() > 1) {
      ESP_LOGI(TAG, "select_source_menu: set player source index %d", index);
      media_player_group->set_active_player_source_index(index);
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationReload;
    }

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

  static void bottomMenuTitles(
      homeassistant_media_player::HomeAssistantBaseMediaPlayer* player,
      bool bottomMenu,
      std::vector<esphome::homething_menu_base::MenuTitleBase*>* menu_titles) {
    const char* const TAG = "homething.nowplaying.control";
    auto supported_features = player->get_option_menu_features(bottomMenu);
    for (auto iter = supported_features->begin();
         iter != supported_features->end(); ++iter) {
      auto item = (*iter)->get_feature();
      auto entity_id =
          homeassistant_media_player::supported_feature_string_map[item];
      auto title = (*iter)->get_title();
      ESP_LOGI(TAG, "speakerNowPlayingMenuStates: %s %s draw bottom %d",
               title.c_str(), entity_id.c_str(), bottomMenu);
      switch (item) {
        case homeassistant_media_player::MediaPlayerSupportedFeature::PLAY:
          break;
        case homeassistant_media_player::MediaPlayerSupportedFeature::PAUSE: {
          std::string playString =
              player->playerState ==
                      homeassistant_media_player::RemotePlayerState::
                          PlayingRemotePlayerState
                  ? "Pause"
                  : "Play";
          // if (bottomMenu)
          (*menu_titles)
              .push_back(new esphome::homething_menu_base::MenuTitleBase(
                  playString, entity_id,
                  esphome::homething_menu_base::NoMenuTitleRightIcon));
          break;
        }
        case homeassistant_media_player::MediaPlayerSupportedFeature::
            SHUFFLE_SET: {
          std::string shuffle_string =
              player->is_shuffling() ? "Shuffling" : "Shuffle";
          (*menu_titles)
              .push_back(new esphome::homething_menu_base::MenuTitleBase(
                  shuffle_string, entity_id,
                  esphome::homething_menu_base::NoMenuTitleRightIcon));
          break;
        }
        case homeassistant_media_player::MediaPlayerSupportedFeature::
            CUSTOM_COMMAND: {
          auto command = (*iter)->get_command();
          if (command != nullptr) {
            auto command_name = command->get_name();
            (*menu_titles)
                .push_back(new esphome::homething_menu_base::MenuTitleBase(
                    command_name, entity_id,
                    esphome::homething_menu_base::NoMenuTitleRightIcon));
          }
          break;
        }
        default:
          (*menu_titles)
              .push_back(new esphome::homething_menu_base::MenuTitleBase(
                  title, entity_id,
                  esphome::homething_menu_base::NoMenuTitleRightIcon));
          break;
      }
    }
  }
};

}  // namespace homething_menu_now_playing
}  // namespace esphome
