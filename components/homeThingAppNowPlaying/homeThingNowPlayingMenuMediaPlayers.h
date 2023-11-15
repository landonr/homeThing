#pragma once

#include <map>
#include <vector>
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/components/homeThingApp/homeThingApp.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"

namespace esphome {
namespace homething_menu_now_playing {

class homeThingNowPlayingMenuMediaPlayers {
 public:
  static homething_menu_app::NavigationCoordination select_menu(
      int index, homeassistant_media_player::HomeAssistantMediaPlayerGroup*
                     media_player_group) {
    auto media_players = media_player_group->get_media_players();
    if (media_players->size() == 0 || media_players->size() <= index) {
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationNone;
    }
    media_player_group->selectMediaPlayers((*media_players)[index]);
    return homething_menu_app::NavigationCoordination::
        NavigationCoordinationPop;
  }

  static void media_player_menu_titles(
      std::vector<homething_menu_base::MenuTitleBase*>* menu_titles,
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          media_player_group) {
    const char* const TAG = "homething.now_playing.menu.media_players";
    std::map<
        homeassistant_media_player::HomeAssistantBaseMediaPlayer*,
        std::vector<homeassistant_media_player::HomeAssistantBaseMediaPlayer*>>
        tree;

    for (const auto media_player : (*media_player_group->get_media_players())) {
      auto parent = media_player->get_parent_media_player();
      auto groupMembers = media_player->get_group_members();
      ESP_LOGD(
          TAG,
          "mediaPlayersTitleString: player %s parent set %d group members %d",
          media_player->get_entity_id().c_str(), parent != NULL,
          groupMembers->size());
      if (parent != NULL) {
        // ignore parent if its a soundbar because the tv is the root parent
        if (parent->mediaSource ==
                homeassistant_media_player::RemotePlayerMediaSource::
                    TVRemotePlayerMediaSource &&
            parent->get_parent_media_player() != NULL) {
          auto grand_parent = parent->get_parent_media_player();
          // grand parent is tv, parent is soundbar, mediaplayer is grouped speaker
          if (tree.find(grand_parent) == tree.end()) {
            ESP_LOGD(
                TAG,
                "mediaPlayersTitleString: adding grandparent1 %s player %s",
                grand_parent->get_entity_id().c_str(),
                media_player->get_entity_id().c_str());
            tree[grand_parent] = {media_player};
          } else {
            ESP_LOGD(
                TAG,
                "mediaPlayersTitleString: adding grandparent2 %s player %s",
                grand_parent->get_entity_id().c_str(),
                media_player->get_entity_id().c_str());
            tree[grand_parent].push_back(media_player);
          }
        } else if (tree.find(parent) == tree.end()) {
          // dont add soundbar to tv if it's not playing
          if (parent->get_player_type() ==
                  homeassistant_media_player::RemotePlayerType::
                      TVRemotePlayerType &&
              media_player->mediaSource !=
                  homeassistant_media_player::RemotePlayerMediaSource::
                      TVRemotePlayerMediaSource) {
            ESP_LOGD(TAG, "mediaPlayersTitleString: adding player %s no parent",
                     parent->get_entity_id().c_str());
            tree[media_player] = {};
          } else {
            ESP_LOGD(TAG,
                     "mediaPlayersTitleString: adding parent2 %s player %s",
                     parent->get_entity_id().c_str(),
                     media_player->get_entity_id().c_str());
            tree[parent] = {media_player};
          }
        } else {
          ESP_LOGD(TAG, "mediaPlayersTitleString: adding parent3 %s player %s",
                   parent->get_entity_id().c_str(),
                   media_player->get_entity_id().c_str());
          tree[parent].push_back(media_player);
        }
      } else {
        if (tree.find(media_player) == tree.end()) {
          ESP_LOGD(TAG, "mediaPlayersTitleString: player set %s",
                   media_player->get_entity_id().c_str());
          tree[media_player] = {};
        }
      }
    }
    ESP_LOGD(TAG, "mediaPlayersTitleString: ------");
    for (auto tree_item : tree) {
      auto parent = tree_item.first;
      menu_titles->push_back(new homething_menu_base::MenuTitlePlayer(
          parent->get_name(), parent->get_entity_id(),
          homething_menu_base::NoMenuTitleLeftIcon,
          homething_menu_base::NoMenuTitleRightIcon, parent));
      for (const auto media_player : tree_item.second) {
        menu_titles->push_back(new homething_menu_base::MenuTitlePlayer(
            media_player->get_name(), media_player->get_entity_id(),
            homething_menu_base::GroupedMenuTitleLeftIcon,
            homething_menu_base::NoMenuTitleRightIcon, media_player));
      }
    }
  }
};
}  // namespace homething_menu_now_playing
}  // namespace esphome
