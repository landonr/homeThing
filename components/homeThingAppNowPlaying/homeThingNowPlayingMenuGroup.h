#pragma once

#include <string>
#include <vector>
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"

namespace esphome {
namespace homething_menu_now_playing {

class homeThingNowPlayingMenuGroup {
 public:
  static homething_menu_app::NavigationCoordination pop_menu(
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          media_player_group) {
    if (media_player_group->newSpeakerGroupParent == NULL) {
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationPop;
    }
    media_player_group->newSpeakerGroupParent = NULL;
    return homething_menu_app::NavigationCoordination::
        NavigationCoordinationReload;
  }

  static homething_menu_app::NavigationCoordination select_menu(
      int index, homeassistant_media_player::HomeAssistantMediaPlayerGroup*
                     media_player_group) {
    if (media_player_group->newSpeakerGroupParent == NULL &&
        index < media_player_group->get_media_players()->size()) {
      auto media_player = (*media_player_group->get_media_players())[index];
      if (media_player_group->selectGroup(media_player, index)) {
        return homething_menu_app::NavigationCoordination::
            NavigationCoordinationUpdate;
      }
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationReload;
    }
    if (media_player_group->selectGroup(nullptr, index)) {
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationUpdate;
    }
    return homething_menu_app::NavigationCoordination::
        NavigationCoordinationReload;
  }

  static void active_menu(
      std::vector<esphome::homething_menu_base::MenuTitleBase*>* menu_titles,
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          media_player_group) {
    if (media_player_group->newSpeakerGroupParent != NULL) {
      return groupTitleSwitches(media_player_group->get_media_players(),
                                media_player_group->newSpeakerGroupParent,
                                menu_titles);
      return;
    }
    groupTitleString(media_player_group->get_media_players(), menu_titles);
  }

 private:
  static void groupTitleSwitches(
      const std::vector<
          homeassistant_media_player::HomeAssistantBaseMediaPlayer*>*
          media_players,
      homeassistant_media_player::HomeAssistantBaseMediaPlayer*
          newSpeakerGroupParent,
      std::vector<esphome::homething_menu_base::MenuTitleBase*>* menu_titles) {
    std::vector<std::string> groupedMembers;
    (*menu_titles)
        .push_back(new esphome::homething_menu_base::MenuTitlePlayer(
            "Group " + newSpeakerGroupParent->get_name(),
            newSpeakerGroupParent->get_entity_id(),
            esphome::homething_menu_base::NoMenuTitleLeftIcon,
            esphome::homething_menu_base::ArrowMenuTitleRightIcon,
            newSpeakerGroupParent));

    for (auto& media_player : (*media_players)) {
      if (newSpeakerGroupParent->get_entity_id() ==
              media_player->get_entity_id() ||
          media_player->get_player_type() !=
              homeassistant_media_player::RemotePlayerType::
                  SpeakerRemotePlayerType ||
          !media_player->supports(homeassistant_media_player::
                                      MediaPlayerSupportedFeature::GROUPING)) {
        continue;
      } else {
        auto speaker = static_cast<
            homeassistant_media_player::HomeAssistantSpeakerMediaPlayer*>(
            media_player);
        auto groupMembers = speaker->get_group_members();
        if (std::find(groupMembers->begin(), groupMembers->end(),
                      newSpeakerGroupParent->get_entity_id()) !=
            groupMembers->end()) {
          (*menu_titles)
              .push_back(new esphome::homething_menu_base::MenuTitlePlayer(
                  speaker->get_name(), speaker->get_entity_id(),
                  esphome::homething_menu_base::OnMenuTitleLeftIcon,
                  esphome::homething_menu_base::NoMenuTitleRightIcon, speaker));
        } else {
          (*menu_titles)
              .push_back(new esphome::homething_menu_base::MenuTitlePlayer(
                  speaker->get_name(), speaker->get_entity_id(),
                  esphome::homething_menu_base::OffMenuTitleLeftIcon,
                  esphome::homething_menu_base::NoMenuTitleRightIcon, speaker));
        }
      }
    }
  }

  static void groupTitleString(
      const std::vector<
          homeassistant_media_player::HomeAssistantBaseMediaPlayer*>*
          media_players,
      std::vector<esphome::homething_menu_base::MenuTitleBase*>* menu_titles) {
    const char* const TAG = "homething.now_playing.menu.group";
    std::vector<std::string> groupedMembers;
    for (auto& media_player : (*media_players)) {
      ESP_LOGD(TAG, "groupTitleString: %s %s", media_player->get_name().c_str(),
               media_player->get_entity_id().c_str());
      if (std::find(groupedMembers.begin(), groupedMembers.end(),
                    media_player->get_entity_id()) != groupedMembers.end() ||
          media_player->get_player_type() !=
              homeassistant_media_player::RemotePlayerType::
                  SpeakerRemotePlayerType ||
          !media_player->supports(homeassistant_media_player::
                                      MediaPlayerSupportedFeature::GROUPING)) {
        // skip grouped members that were already found
        continue;
      }
      auto speaker = static_cast<
          homeassistant_media_player::HomeAssistantSpeakerMediaPlayer*>(
          media_player);
      auto groupMembers = speaker->get_group_members();
      if (groupMembers->size() > 1) {
        if ((*groupMembers)[0] != speaker->get_entity_id()) {
          ESP_LOGD(TAG, "%s not parent %s", (*groupMembers)[0].c_str(),
                   speaker->get_entity_id().c_str());
          // speaker isn't the group parent
          continue;
        }
        (*menu_titles)
            .push_back(new esphome::homething_menu_base::MenuTitlePlayer(
                speaker->get_name(), speaker->get_entity_id(),
                esphome::homething_menu_base::NoMenuTitleLeftIcon,
                esphome::homething_menu_base::ArrowMenuTitleRightIcon,
                speaker));
        for (auto& groupedSpeaker : (*groupMembers)) {
          if (groupedSpeaker != speaker->get_entity_id()) {
            groupedMembers.push_back(groupedSpeaker);
            std::string groupedSpeakerName =
                friendlyNameForEntityId(groupedSpeaker, media_players);
            if (groupedSpeakerName != "") {
              (*menu_titles)
                  .push_back(new esphome::homething_menu_base::MenuTitlePlayer(
                      groupedSpeakerName, groupedSpeaker,
                      esphome::homething_menu_base::GroupedMenuTitleLeftIcon,
                      esphome::homething_menu_base::ArrowMenuTitleRightIcon,
                      speaker));
            }
          }
        }
      } else {
        (*menu_titles)
            .push_back(new esphome::homething_menu_base::MenuTitlePlayer(
                speaker->get_name(), speaker->get_entity_id(),
                esphome::homething_menu_base::NoMenuTitleLeftIcon,
                esphome::homething_menu_base::ArrowMenuTitleRightIcon,
                speaker));
      }
    }
  }

  static std::string friendlyNameForEntityId(
      std::string speakerentityId,
      const std::vector<
          homeassistant_media_player::HomeAssistantBaseMediaPlayer*>*
          media_players) {
    for (auto& speaker : (*media_players)) {
      if (speaker->get_entity_id() == speakerentityId) {
        return speaker->get_name();
      }
    }
    return "";
  }
};
}  // namespace homething_menu_now_playing
}  // namespace esphome
