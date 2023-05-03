#pragma once
#include <string>
#include <unordered_map>
#include "esphome/components/homeassistant/text_sensor/homeassistant_text_sensor.h"
#include "esphome/components/homeassistant_light_group/HomeAssistantLightGroup.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"
#include "esphome/components/homeassistant_service_group/HomeAssistantServiceGroup.h"
#include "esphome/components/homeassistant_switch_group/HomeAssistantSwitchGroup.h"
#include "esphome/core/color.h"
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
  scenesMenu,
  lightsMenu,
  lightsDetailMenu,
  switchesMenu,
  nowPlayingMenu,
  sensorsMenu,
  backlightMenu,
  sleepMenu,
  aboutMenu
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
    case backlightMenu:
      return "Backlight";
    case sleepMenu:
      return "Sleep";
    case mediaPlayersMenu:
      return "Media Players";
    case lightsMenu:
      return "Lights";
    case lightsDetailMenu:
      return "Light Detail";
    case switchesMenu:
      return "Switches";
    case scenesMenu:
      return "Scenes and Actions";
    case rootMenu:
      return "Home";
    case groupMenu:
      return "Speaker Group";
    case sensorsMenu:
      return "Sensors";
    case bootMenu:
      return "Boot";
    case aboutMenu:
      return COMPONENTS_HOMETHING_VERSION;
  }
  return "";
}

static MenuTitleRightIcon menu_state_right_icon(MenuStates menu_state) {
  switch (menu_state) {
    case nowPlayingMenu:
      return ArrowMenuTitleRightIcon;
    case sourcesMenu:
      return ArrowMenuTitleRightIcon;
    case backlightMenu:
      return NoMenuTitleRightIcon;
    case sleepMenu:
      return NoMenuTitleRightIcon;
    case mediaPlayersMenu:
      return ArrowMenuTitleRightIcon;
    case lightsMenu:
      return ArrowMenuTitleRightIcon;
    case lightsDetailMenu:
      return ArrowMenuTitleRightIcon;
    case switchesMenu:
      return ArrowMenuTitleRightIcon;
    case scenesMenu:
      return ArrowMenuTitleRightIcon;
    case rootMenu:
      return NoMenuTitleRightIcon;
    case groupMenu:
      return ArrowMenuTitleRightIcon;
    case sensorsMenu:
      return ArrowMenuTitleRightIcon;
    case bootMenu:
      return NoMenuTitleRightIcon;
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

 protected:
  std::string name_;
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

class MenuTitleSlider : public MenuTitleBase {
 public:
  bool slider_{false};
  bool currentState;
  int sliderValue;
  int displayValue;
  int value_min_;
  int value_max_;
  std::string sliderUnit;
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

class MenuTitleLight : public MenuTitleToggle {
 public:
  Color lightColor;

  MenuTitleLight(std::string new_name, std::string newEntityId,
                 MenuTitleLeftIcon newLeftIconState,
                 MenuTitleRightIcon newRightIconState, Color newLightColor)
      : MenuTitleToggle{new_name, newEntityId, newLeftIconState,
                        newRightIconState, LightMenuTitleType},
        lightColor(newLightColor) {}
};

// media

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
    std::vector<media_player_source::MediaPlayerSourceBase*> sources) {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  for (auto& source : sources) {
    auto new_menu_title = std::make_shared<MenuTitleBase>(
        source->get_name(), "", NoMenuTitleRightIcon);
    out.push_back(new_menu_title);
  }
  return out;
}

static std::vector<std::shared_ptr<MenuTitleSource>>
activePlayerSourceItemTitles(
    std::vector<std::shared_ptr<media_player_source::MediaPlayerSourceItem>>
        sourceItems) {
  if (sourceItems.size() == 0) {
    ESP_LOGW(MENU_TITLE_TAG, "activePlayerSourceItemTitles: empty list");
    return {};
  }
  std::vector<std::shared_ptr<MenuTitleSource>> out;
  for (auto& sourceItem : sourceItems) {
    auto new_menu_title = std::make_shared<MenuTitleSource>(
        sourceItem->get_name(), "", NoMenuTitleRightIcon, sourceItem.get());
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

// switch

static std::vector<std::shared_ptr<MenuTitleBase>> switchTitleSwitches(
    const std::vector<homeassistant_switch::HomeAssistantSwitch*>& switches) {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  for (const auto switchObject : switches) {
    ESP_LOGD(MENU_TITLE_TAG, "switch state %d", switchObject->state);
    MenuTitleLeftIcon state =
        switchObject->state ? OnMenuTitleLeftIcon : OffMenuTitleLeftIcon;
    out.push_back(std::make_shared<MenuTitleToggle>(
        switchObject->get_name(), switchObject->get_entity_id(), state,
        NoMenuTitleRightIcon));
  }
  return out;
}

// service

static std::vector<std::shared_ptr<MenuTitleBase>> sceneTitleStrings(
    const std::vector<
        homeassistant_service_group::HomeAssistantServiceCommand*>& services) {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  for (auto& service : services) {
    ESP_LOGD(MENU_TITLE_TAG, "MENU Service %s",
             service->get_text<std::string>().c_str());
    std::string service_text = service->get_text<std::string>();
    out.push_back(std::make_shared<MenuTitleBase>(service_text, "2",
                                                  NoMenuTitleRightIcon));
  }
  return out;
}

// light

static std::vector<std::shared_ptr<MenuTitleBase>> lightTitleSwitches(
    const std::vector<homeassistant_light::HomeAssistantLightState*>& lights) {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  for (auto& light : lights) {
    auto output = static_cast<homeassistant_light::HomeAssistantLight*>(
        light->get_output());
    ESP_LOGD(MENU_TITLE_TAG, "state %d (%s)", output->get_state(),
             light->get_name().c_str());
    MenuTitleLeftIcon state =
        output->get_state() ? OnMenuTitleLeftIcon : OffMenuTitleLeftIcon;
    MenuTitleRightIcon rightIcon = output->supportsBrightness()
                                       ? ArrowMenuTitleRightIcon
                                       : NoMenuTitleRightIcon;
    out.push_back(std::make_shared<MenuTitleLight>(
        light->get_name(), "", state, rightIcon, output->rgbLightColor()));
  }
  return out;
}

static std::shared_ptr<MenuTitleSlider> makeSlider(
    std::string title, std::string unit, std::string entity_id_, int min,
    int max, int value, int displayUnitMin, int displayUnitMax) {
  int displayValue = value;
  float oldRange = max - min;
  float valueMinusMin = value - min;
  if (value > 0) {
    float displayNewRange = displayUnitMax - displayUnitMin;
    displayValue =
        static_cast<float>(((valueMinusMin * displayNewRange) / oldRange)) +
        displayUnitMin;
  }

  // float newMin = display_state_->get_slider_margin_size();
  float newMin = 8;
  // float newRange = displayWidth - 4 * newMin;
  float newRange = 100;
  int sliderValue = ((valueMinusMin * newRange) / oldRange) + newMin;
  return std::make_shared<MenuTitleSlider>(title.c_str(), entity_id_,
                                           NoMenuTitleRightIcon, value,
                                           displayValue, unit, min, max);
}

static std::vector<std::shared_ptr<MenuTitleBase>> lightTitleItems(
    homeassistant_light::HomeAssistantLight* light) {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  if (light->supportsBrightness()) {
    auto is_on = light->get_light_state_()->remote_values.is_on();
    int brightness =
        !is_on ? 0
               : static_cast<int>(
                     light->get_light_state_()->remote_values.get_brightness() *
                     255);
    out.push_back(makeSlider("Brightness", "%%", light->get_entity_id(), 0,
                             MAX_BRIGHTNESS, brightness, 0, 100));
  }
  if (light->supportsColorTemperature()) {
    auto max_mireds = light->get_traits().get_max_mireds();
    auto min_mireds = light->get_traits().get_min_mireds();
    out.push_back(makeSlider(
        "Temperature", "K", light->get_entity_id(), min_mireds, max_mireds,
        light->get_light_state_()->remote_values.get_color_temperature(),
        1000000 / min_mireds, 1000000 / max_mireds));
  }
  if (light->supportsColor()) {
    out.push_back(makeSlider("Color", "", light->get_entity_id(), 0, 360,
                             light->get_hsv_color(), 0, 360));
  }
  return out;
}

// sensor

static std::vector<std::shared_ptr<MenuTitleBase>> sensorTitles(
    const std::vector<esphome::homeassistant::HomeassistantTextSensor*>&
        sensors) {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  for (auto& sensor : sensors) {
    if (sensor->get_name() != "") {
      out.push_back(std::make_shared<MenuTitleBase>(
          sensor->get_name() + " " + sensor->get_state(), "",
          NoMenuTitleRightIcon));
    } else {
      out.push_back(std::make_shared<MenuTitleBase>(sensor->state, "",
                                                    NoMenuTitleRightIcon));
    }
  }
  return out;
}

// now playing bottom menu

enum NowPlayingMenuState {
  pauseNowPlayingMenuState,
  volumeUpNowPlayingMenuState,
  volumeDownNowPlayingMenuState,
  nextNowPlayingMenuState,
  menuNowPlayingMenuState,
  shuffleNowPlayingMenuState,
  backNowPlayingMenuState,
  TVPowerNowPlayingMenuState,
  homeNowPlayingMenuState,
  groupNowPlayingMenuState
};

static std::vector<std::shared_ptr<MenuTitleBase>> speakerNowPlayingMenuStates(
    homeassistant_media_player::HomeAssistantBaseMediaPlayer* player) {
  std::string playString =
      player->playerState == homeassistant_media_player::RemotePlayerState::
                                 PlayingRemotePlayerState
          ? "Pause"
          : "Play";

  std::vector<std::shared_ptr<MenuTitleBase>> out;
  auto supported_features = player->get_features();
  for (auto iter = supported_features.begin(); iter != supported_features.end();
       ++iter) {
    homeassistant_media_player::MediaPlayerSupportedFeature item =
        *(iter->get());
    auto entity_id =
        homeassistant_media_player::supported_feature_string_map[item];
    switch (*(iter->get())) {
      case homeassistant_media_player::MediaPlayerSupportedFeature::
          SHUFFLE_SET: {
        std::string shuffle_string =
            player->is_shuffling() ? "Shuffling" : "Shuffle";
        out.push_back(std::make_shared<MenuTitleBase>(shuffle_string, entity_id,
                                                      NoMenuTitleRightIcon));
        break;
      }
      default:
        auto title = homeassistant_media_player::supported_feature_string(
            *(iter->get()));
        out.push_back(std::make_shared<MenuTitleBase>(title, entity_id,
                                                      NoMenuTitleRightIcon));
        break;
    }
  }
  return out;
  // return {
  //     std::make_shared<MenuTitleBase>(playString, "play", NoMenuTitleRightIcon),
  //     std::make_shared<MenuTitleBase>("Vl Up", "volume_up",
  //                                     NoMenuTitleRightIcon),
  //     std::make_shared<MenuTitleBase>("Vl Down", "volume_down",
  //                                     NoMenuTitleRightIcon),
  //     std::make_shared<MenuTitleBase>("Next", "next", NoMenuTitleRightIcon),

  //     std::make_shared<MenuTitleBase>("Group", "group", NoMenuTitleRightIcon),
  //     std::make_shared<MenuTitleBase>("Menu", "menu", NoMenuTitleRightIcon),
  // };
}

static std::vector<std::shared_ptr<MenuTitleBase>> TVNowPlayingMenuStates() {
  // return {pauseNowPlayingMenuState,      volumeUpNowPlayingMenuState,
  //         volumeDownNowPlayingMenuState, backNowPlayingMenuState,
  //         TVPowerNowPlayingMenuState,    homeNowPlayingMenuState,
  //         menuNowPlayingMenuState};
  return {};
}

static std::vector<std::shared_ptr<MenuTitleBase>> getNowPlayingMenuStates(
    homeassistant_media_player::HomeAssistantBaseMediaPlayer* player) {
  // if (player->get_player_type() ==
  //     homeassistant_media_player::RemotePlayerType::TVRemotePlayerType) {
  //   return TVNowPlayingMenuStates();
  // }
  return speakerNowPlayingMenuStates(player);
}

}  // namespace homething_menu_base
}  // namespace esphome
