#pragma once
#include <string>
#include <unordered_map>
#include "esphome/core/color.h"

#ifdef USE_MEDIA_PLAYER_GROUP
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"
#endif

#include "esphome/core/log.h"
#include "version.h"

namespace esphome {
namespace homething_menu_base {

static const char* const MENU_TITLE_TAG = "homething.menutitle";

enum MenuStates {
  bootMenu,
  rootMenu,
  sourcesMenu,
  groupMenu,
  mediaPlayersMenu,
  lightsDetailMenu,
  nowPlayingMenu,
  settingsMenu
};

enum MenuTitleLeftIcon {
  NoMenuTitleLeftIcon,
  OffMenuTitleLeftIcon,
  OnMenuTitleLeftIcon,
  GroupedMenuTitleLeftIcon
};

enum MenuTitleRightIcon { NoMenuTitleRightIcon, ArrowMenuTitleRightIcon };

static std::string menu_state_title(MenuStates menu_state) {
  switch (menu_state) {
    case nowPlayingMenu:
      return "Now Playing";
    case sourcesMenu:
      return "Sources";
    case mediaPlayersMenu:
      return "Media Players";
    case lightsDetailMenu:
      return "Light Detail";
    case rootMenu:
      return "Home";
    case groupMenu:
      return "Speaker Group";
    case bootMenu:
      return "Boot";
    case settingsMenu:
      return "Settings";
  }
  return "";
}

static MenuTitleRightIcon menu_state_right_icon(MenuStates menu_state) {
  switch (menu_state) {
    case nowPlayingMenu:
      return ArrowMenuTitleRightIcon;
    case sourcesMenu:
      return ArrowMenuTitleRightIcon;
    case mediaPlayersMenu:
      return ArrowMenuTitleRightIcon;
    case lightsDetailMenu:
      return ArrowMenuTitleRightIcon;
    case rootMenu:
      return NoMenuTitleRightIcon;
    case groupMenu:
      return ArrowMenuTitleRightIcon;
    case bootMenu:
      return NoMenuTitleRightIcon;
    case settingsMenu:
      return ArrowMenuTitleRightIcon;
  }
  return NoMenuTitleRightIcon;
}

enum MenuTitleType {
  BaseMenuTitleType,
  PlayerMenuTitleType,
  SliderMenuTitleType,
  ToggleMenuTitleType,
  LightMenuTitleType,
  SourceMenuTitleType
};

enum SliderSelectionState {
  SliderSelectionStateNone,
  SliderSelectionStateActive,
  SliderSelectionStateHover
};

class MenuTitleBase {
 protected:
  std::string name_;

 public:
  std::string entity_id_;
  MenuTitleRightIcon rightIconState;
  MenuTitleType titleType;
  MenuTitleBase(std::string new_name, std::string newEntityId,
                MenuTitleRightIcon newRightIconState,
                MenuTitleType newTitleType = BaseMenuTitleType)
      : name_(new_name),
        entity_id_(newEntityId),
        rightIconState(newRightIconState),
        titleType(newTitleType) {}

  std::string get_name() {
    if (name_ != "") {
      return name_;
    } else {
      return entity_id_;
    }
  }
};

class MenuTitleToggle : public MenuTitleBase {
 public:
  MenuTitleLeftIcon leftIconState;

  MenuTitleToggle(std::string new_name, std::string newEntityId,
                  MenuTitleLeftIcon newLeftIconState,
                  MenuTitleRightIcon newRightIconState,
                  MenuTitleType newTitleType = ToggleMenuTitleType)
      : MenuTitleBase{new_name, newEntityId, newRightIconState, newTitleType},
        leftIconState(newLeftIconState) {}
  bool indentLine() {
    switch (leftIconState) {
      case OffMenuTitleLeftIcon:
      case OnMenuTitleLeftIcon:
      case GroupedMenuTitleLeftIcon:
        return true;
      case NoMenuTitleLeftIcon:
        return false;
    }
    return false;
  }
};

class MenuTitleSlider : public MenuTitleBase {
 public:
  bool slider_{false};
  bool currentState;
  int sliderValue;
  int displayValue;
  std::string sliderUnit;
  int value_min_;
  int value_max_;
  MenuTitleSlider(std::string newTitle, std::string newEntityId,
                  MenuTitleRightIcon newRightIconState, int newSliderValue,
                  int newDisplayValue, std::string newSliderUnit, int value_min,
                  int value_max)
      : MenuTitleBase{newTitle, newEntityId, newRightIconState,
                      SliderMenuTitleType},
        sliderValue(newSliderValue),
        displayValue(newDisplayValue),
        sliderUnit(newSliderUnit),
        value_min_(value_min),
        value_max_(value_max) {}

  float percent_value() {
    float value_minus_min = sliderValue - value_min_;
    float old_range = value_max_ - value_min_;
    return value_minus_min / old_range;
  }
};

#ifdef USE_MEDIA_PLAYER_GROUP  // now playing bottom menu

class MenuTitlePlayer : public MenuTitleToggle {
 public:
  homeassistant_media_player::HomeAssistantBaseMediaPlayer* media_player_;

  MenuTitlePlayer(std::string new_name, std::string newEntityId,
                  MenuTitleLeftIcon newLeftIconState,
                  MenuTitleRightIcon newRightIconState,
                  homeassistant_media_player::HomeAssistantBaseMediaPlayer*
                      new_media_player)
      : MenuTitleToggle{new_name, newEntityId, newLeftIconState,
                        newRightIconState, PlayerMenuTitleType},
        media_player_(new_media_player) {}

  std::string mediaSourceIcon() {
    switch (media_player_->mediaSource) {
      case homeassistant_media_player::RemotePlayerMediaSource::
          NoRemotePlayerMediaSource:
        return "󰐊";
      case homeassistant_media_player::RemotePlayerMediaSource::
          HomeRemotePlayerMediaSource:
        return "󰋜";
      case homeassistant_media_player::RemotePlayerMediaSource::
          YouTubeRemotePlayerMediaSource:
        return "󰗃";
      case homeassistant_media_player::RemotePlayerMediaSource::
          SpotifyRemotePlayerMediaSource:
        return "󰓇";
      case homeassistant_media_player::RemotePlayerMediaSource::
          NetflixRemotePlayerMediaSource:
        return "󰝆";
      case homeassistant_media_player::RemotePlayerMediaSource::
          PlexRemotePlayerMediaSource:
        return "󰚺";
      case homeassistant_media_player::RemotePlayerMediaSource::
          TVRemotePlayerMediaSource:
        return "󰔂";
    }
    return "";
  }

  Color mediaSourceIconColor(Color defaultColor) {
    switch (media_player_->mediaSource) {
      case homeassistant_media_player::RemotePlayerMediaSource::
          NoRemotePlayerMediaSource:
      case homeassistant_media_player::RemotePlayerMediaSource::
          TVRemotePlayerMediaSource:
      case homeassistant_media_player::RemotePlayerMediaSource::
          HomeRemotePlayerMediaSource:
        return defaultColor;
      case homeassistant_media_player::RemotePlayerMediaSource::
          YouTubeRemotePlayerMediaSource:
        return Color(248, 0, 0);
      case homeassistant_media_player::RemotePlayerMediaSource::
          SpotifyRemotePlayerMediaSource:
        return Color(27, 212, 96);
      case homeassistant_media_player::RemotePlayerMediaSource::
          NetflixRemotePlayerMediaSource:
        return Color(179, 0, 10);
      case homeassistant_media_player::RemotePlayerMediaSource::
          PlexRemotePlayerMediaSource:
        return Color(223, 156, 13);
    }
    return defaultColor;
  }
};

class MenuTitleSource : public MenuTitleBase {
 public:
  media_player_source::MediaPlayerSourceItem* media_source_;

  MenuTitleSource(std::string new_name, std::string newEntityId,
                  MenuTitleRightIcon newRightIconState,
                  media_player_source::MediaPlayerSourceItem* new_media_source)
      : MenuTitleBase{new_name, newEntityId, newRightIconState,
                      SourceMenuTitleType},
        media_source_(new_media_source) {}

  std::string sourceTypeString() { return media_source_->sourceTypeString(); }
};

static std::vector<MenuTitlePlayer*> mediaPlayersTitleString(
    const std::vector<
        homeassistant_media_player::HomeAssistantBaseMediaPlayer*>&
        media_players) {
  std::vector<MenuTitlePlayer*> out;

  std::map<
      homeassistant_media_player::HomeAssistantBaseMediaPlayer*,
      std::vector<homeassistant_media_player::HomeAssistantBaseMediaPlayer*>>
      tree;

  for (const auto media_player : media_players) {
    auto parent = media_player->get_parent_media_player();
    ESP_LOGD(
        MENU_TITLE_TAG,
        "mediaPlayersTitleString: player %s parent set %d group members %d",
        media_player->get_entity_id().c_str(), parent != NULL,
        media_player->groupMembers.size());
    if (parent != NULL) {
      // ignore parent if its a soundbar because the tv is the root parent
      if (parent->mediaSource ==
              homeassistant_media_player::RemotePlayerMediaSource::
                  TVRemotePlayerMediaSource &&
          parent->get_parent_media_player() != NULL) {
        auto grand_parent = parent->get_parent_media_player();
        // grand parent is tv, parent is soundbar, mediaplayer is grouped speaker
        if (tree.find(grand_parent) == tree.end()) {
          ESP_LOGD(MENU_TITLE_TAG,
                   "mediaPlayersTitleString: adding grandparent1 %s player %s",
                   grand_parent->get_entity_id().c_str(),
                   media_player->get_entity_id().c_str());
          tree[grand_parent] = {media_player};
        } else {
          ESP_LOGD(MENU_TITLE_TAG,
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
          ESP_LOGD(MENU_TITLE_TAG,
                   "mediaPlayersTitleString: adding player %s no parent",
                   parent->get_entity_id().c_str(),
                   media_player->get_entity_id().c_str());
          tree[media_player] = {};
        } else {
          ESP_LOGD(MENU_TITLE_TAG,
                   "mediaPlayersTitleString: adding parent2 %s player %s",
                   parent->get_entity_id().c_str(),
                   media_player->get_entity_id().c_str());
          tree[parent] = {media_player};
        }
      } else {
        ESP_LOGD(MENU_TITLE_TAG,
                 "mediaPlayersTitleString: adding parent3 %s player %s",
                 parent->get_entity_id().c_str(),
                 media_player->get_entity_id().c_str());
        tree[parent].push_back(media_player);
      }
    } else {
      if (tree.find(media_player) == tree.end()) {
        ESP_LOGD(MENU_TITLE_TAG, "mediaPlayersTitleString: player set %s",
                 media_player->get_entity_id().c_str());
        tree[media_player] = {};
      }
    }
  }
  ESP_LOGD(MENU_TITLE_TAG, "mediaPlayersTitleString: ------");
  for (auto tree_item : tree) {
    auto parent = tree_item.first;
    out.push_back(
        new MenuTitlePlayer(parent->get_name(), parent->get_entity_id(),
                            NoMenuTitleLeftIcon, NoMenuTitleRightIcon, parent));
    for (const auto media_player : tree_item.second) {
      out.push_back(new MenuTitlePlayer(
          media_player->get_name(), media_player->get_entity_id(),
          GroupedMenuTitleLeftIcon, NoMenuTitleRightIcon, media_player));
    }
  }
  return out;
}

static std::vector<std::shared_ptr<MenuTitleBase>> activePlayerSourceTitles(
    std::vector<media_player_source::MediaPlayerSourceBase*>* sources) {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  for (auto& source : *sources) {
    auto new_menu_title = std::make_shared<MenuTitleBase>(
        source->get_name(), "", NoMenuTitleRightIcon);
    out.push_back(new_menu_title);
  }
  return out;
}

static std::vector<std::shared_ptr<MenuTitleSource>>
activePlayerSourceItemTitles(
    std::vector<std::shared_ptr<media_player_source::MediaPlayerSourceItem>>*
        sourceItems) {
  if (sourceItems->size() == 0) {
    ESP_LOGW(MENU_TITLE_TAG, "activePlayerSourceItemTitles: empty list");
    return {};
  }
  std::vector<std::shared_ptr<MenuTitleSource>> out;
  for (auto& sourceItem : (*sourceItems)) {
    auto name = sourceItem->get_name();
    ESP_LOGD(MENU_TITLE_TAG, "activePlayerSourceItemTitles: name %s",
             name.c_str());
    auto new_menu_title = std::make_shared<MenuTitleSource>(
        name, "", NoMenuTitleRightIcon, sourceItem.get());
    out.push_back(new_menu_title);
  }
  return out;
}

static std::vector<std::shared_ptr<MenuTitleBase>> groupTitleSwitches(
    const std::vector<
        homeassistant_media_player::HomeAssistantBaseMediaPlayer*>&
        media_players,
    homeassistant_media_player::HomeAssistantBaseMediaPlayer*
        newSpeakerGroupParent) {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  std::vector<std::string> groupedMembers;
  out.push_back(std::make_shared<MenuTitlePlayer>(
      "Group " + newSpeakerGroupParent->get_name(),
      newSpeakerGroupParent->get_entity_id(), NoMenuTitleLeftIcon,
      ArrowMenuTitleRightIcon, newSpeakerGroupParent));

  for (auto& media_player : media_players) {
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
      if (std::find(speaker->groupMembers.begin(), speaker->groupMembers.end(),
                    newSpeakerGroupParent->get_entity_id()) !=
          speaker->groupMembers.end()) {
        out.push_back(std::make_shared<MenuTitlePlayer>(
            speaker->get_name(), speaker->get_entity_id(), OnMenuTitleLeftIcon,
            NoMenuTitleRightIcon, speaker));
      } else {
        out.push_back(std::make_shared<MenuTitlePlayer>(
            speaker->get_name(), speaker->get_entity_id(), OffMenuTitleLeftIcon,
            NoMenuTitleRightIcon, speaker));
      }
    }
  }
  return out;
}

static std::string friendlyNameForEntityId(
    std::string speakerentityId,
    const std::vector<
        homeassistant_media_player::HomeAssistantBaseMediaPlayer*>&
        media_players) {
  for (auto& speaker : media_players) {
    if (speaker->get_entity_id() == speakerentityId) {
      return speaker->get_name();
    }
  }
  return "";
}

static std::vector<std::shared_ptr<MenuTitleBase>> speakerNowPlayingMenuStates(
    homeassistant_media_player::HomeAssistantBaseMediaPlayer* player,
    bool bottomMenu) {
  std::vector<std::shared_ptr<MenuTitleBase>> out;

  auto supported_features = player->get_option_menu_features(bottomMenu);
  for (auto iter = supported_features->begin();
       iter != supported_features->end(); ++iter) {
    auto item = (*iter)->get_feature();
    auto entity_id =
        homeassistant_media_player::supported_feature_string_map[item];
    auto title = (*iter)->get_title();
    ESP_LOGI(MENU_TITLE_TAG,
             "speakerNowPlayingMenuStates: %s %s draw bottom %d", title.c_str(),
             entity_id.c_str(), bottomMenu);
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
        out.push_back(std::make_shared<MenuTitleBase>(playString, entity_id,
                                                      NoMenuTitleRightIcon));
        break;
      }
      case homeassistant_media_player::MediaPlayerSupportedFeature::VOLUME_SET:
        if (bottomMenu) {
          out.push_back(std::make_shared<MenuTitleBase>(
              "Vol Up",
              homeassistant_media_player::supported_feature_string_map
                  [homeassistant_media_player::MediaPlayerSupportedFeature::
                       VOLUME_UP],
              NoMenuTitleRightIcon));
          out.push_back(std::make_shared<MenuTitleBase>(
              "Vol Dn",
              homeassistant_media_player::supported_feature_string_map
                  [homeassistant_media_player::MediaPlayerSupportedFeature::
                       VOLUME_DOWN],
              NoMenuTitleRightIcon));
          break;
        }
        break;
      case homeassistant_media_player::MediaPlayerSupportedFeature::
          SHUFFLE_SET: {
        std::string shuffle_string =
            player->is_shuffling() ? "Shuffling" : "Shuffle";
        out.push_back(std::make_shared<MenuTitleBase>(shuffle_string, entity_id,
                                                      NoMenuTitleRightIcon));
        break;
      }
      case homeassistant_media_player::MediaPlayerSupportedFeature::
          CUSTOM_COMMAND: {
        auto command = (*iter)->get_command();
        if (command != nullptr) {
          auto command_name = command->get_name();
          out.push_back(std::make_shared<MenuTitleBase>(command_name, entity_id,
                                                        NoMenuTitleRightIcon));
        }
        break;
      }
      default:
        out.push_back(std::make_shared<MenuTitleBase>(title, entity_id,
                                                      NoMenuTitleRightIcon));
        break;
    }
  }
  return out;
}

static std::vector<std::shared_ptr<MenuTitleBase>> groupTitleString(
    const std::vector<
        homeassistant_media_player::HomeAssistantBaseMediaPlayer*>&
        media_players) {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  std::vector<std::string> groupedMembers;
  for (auto& media_player : media_players) {
    ESP_LOGD(MENU_TITLE_TAG, "groupTitleString: %s %s",
             media_player->get_name().c_str(),
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
    if (speaker->groupMembers.size() > 1) {
      if (speaker->groupMembers[0] != speaker->get_entity_id()) {
        ESP_LOGD(MENU_TITLE_TAG, "%s not parent %s",
                 speaker->groupMembers[0].c_str(),
                 speaker->get_entity_id().c_str());
        // speaker isn't the group parent
        continue;
      }
      out.push_back(std::make_shared<MenuTitlePlayer>(
          speaker->get_name(), speaker->get_entity_id(), NoMenuTitleLeftIcon,
          ArrowMenuTitleRightIcon, speaker));
      for (auto& groupedSpeaker : speaker->groupMembers) {
        if (groupedSpeaker != speaker->get_entity_id()) {
          groupedMembers.push_back(groupedSpeaker);
          std::string groupedSpeakerName =
              friendlyNameForEntityId(groupedSpeaker, media_players);
          if (groupedSpeakerName != "") {
            out.push_back(std::make_shared<MenuTitlePlayer>(
                groupedSpeakerName, groupedSpeaker, GroupedMenuTitleLeftIcon,
                ArrowMenuTitleRightIcon, speaker));
          }
        }
      }
    } else {
      out.push_back(std::make_shared<MenuTitlePlayer>(
          speaker->get_name(), speaker->get_entity_id(), NoMenuTitleLeftIcon,
          ArrowMenuTitleRightIcon, speaker));
    }
  }
  return out;
}

static MenuTitlePlayer headerMediaPlayerTitle(
    homeassistant_media_player::HomeAssistantBaseMediaPlayer* active_player) {
  std::string friendlyName = active_player->get_name();
  if (active_player->get_player_type() !=
      homeassistant_media_player::RemotePlayerType::TVRemotePlayerType) {
    auto activeSpeaker = static_cast<
        homeassistant_media_player::HomeAssistantSpeakerMediaPlayer*>(
        active_player);
    if (activeSpeaker != NULL) {
      if (activeSpeaker->groupMembers.size() > 1) {
        friendlyName = friendlyName + " + " +
                       to_string(activeSpeaker->groupMembers.size() - 1);
      }
    }
  }
  return MenuTitlePlayer(friendlyName, active_player->get_entity_id(),
                         NoMenuTitleLeftIcon, NoMenuTitleRightIcon,
                         active_player);
}

#endif  // now playing

}  // namespace homething_menu_base
}  // namespace esphome
