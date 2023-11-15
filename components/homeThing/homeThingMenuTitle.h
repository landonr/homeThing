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
  appMenu,
  // sourcesMenu,
  // groupMenu,
  // mediaPlayersMenu,
  lightsDetailMenu,
  // nowPlayingMenu,
  settingsMenu,
  entityMenu
};

enum MenuTitleLeftIcon {
  NoMenuTitleLeftIcon,
  OffMenuTitleLeftIcon,
  OnMenuTitleLeftIcon,
  GroupedMenuTitleLeftIcon
};

enum MenuTitleRightIcon { NoMenuTitleRightIcon, ArrowMenuTitleRightIcon };

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

  std::string get_name() const {
    if (name_ != "") {
      return name_;
    } else {
      return entity_id_;
    }
  }

  static MenuTitleRightIcon menu_state_right_icon(MenuStates menu_state) {
    switch (menu_state) {
      case appMenu:
        return ArrowMenuTitleRightIcon;
      case lightsDetailMenu:
        return ArrowMenuTitleRightIcon;
      case rootMenu:
        return NoMenuTitleRightIcon;
      case bootMenu:
        return NoMenuTitleRightIcon;
      case settingsMenu:
        return ArrowMenuTitleRightIcon;
      case entityMenu:
        return NoMenuTitleRightIcon;
    }
    return NoMenuTitleRightIcon;
  }

  static std::string menu_state_title(MenuStates menu_state) {
    switch (menu_state) {
      case appMenu:
        return "App";
      case lightsDetailMenu:
        return "Light Detail";
      case rootMenu:
        return "Home";
      case bootMenu:
        return "Boot";
      case settingsMenu:
        return "Settings";
      case entityMenu:
        return "Entity";
    }
    return "";
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
  bool indentLine() const {
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

  float percent_value() const {
    float value_minus_min = sliderValue - value_min_;
    float old_range = value_max_ - value_min_;
    return value_minus_min / old_range;
  }

  static MenuTitleSlider* makeSlider(std::string title, std::string unit,
                                     std::string entity_id_, int min, int max,
                                     int value, int displayUnitMin,
                                     int displayUnitMax) {
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
    return new MenuTitleSlider(title.c_str(), entity_id_, NoMenuTitleRightIcon,
                               value, displayValue, unit, min, max);
  }
};

#ifdef USE_NOW_PLAYING  // now playing bottom menu

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

  static MenuTitlePlayer headerMediaPlayerTitle(
      homeassistant_media_player::HomeAssistantBaseMediaPlayer* active_player) {
    std::string friendlyName = active_player->get_name();
    if (active_player->get_player_type() !=
        homeassistant_media_player::RemotePlayerType::TVRemotePlayerType) {
      auto activeSpeaker = static_cast<
          homeassistant_media_player::HomeAssistantSpeakerMediaPlayer*>(
          active_player);
      if (activeSpeaker != NULL) {
        auto groupMembers = *(activeSpeaker->get_group_members());
        if (groupMembers.size() > 1) {
          friendlyName =
              friendlyName + " + " + to_string(groupMembers.size() - 1);
        }
      }
    }
    return MenuTitlePlayer(friendlyName, active_player->get_entity_id(),
                           NoMenuTitleLeftIcon, NoMenuTitleRightIcon,
                           active_player);
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

  static void activePlayerSourceTitles(
      std::vector<media_player_source::MediaPlayerSourceBase*>* sources,
      std::vector<MenuTitleBase*>* menu_titles) {
    for (auto& source : *sources) {
      auto new_menu_title =
          new MenuTitleBase(source->get_name(), "", NoMenuTitleRightIcon);
      (*menu_titles).push_back(new_menu_title);
    }
  }

  static void activePlayerSourceItemTitles(
      const std::vector<media_player_source::MediaPlayerSourceItem*>*
          sourceItems,
      std::vector<MenuTitleBase*>* menu_titles) {
    if (sourceItems->size() == 0) {
      ESP_LOGW(MENU_TITLE_TAG, "activePlayerSourceItemTitles: empty list");
      return;
    }
    for (auto& sourceItem : (*sourceItems)) {
      auto name = sourceItem->get_name();
      ESP_LOGD(MENU_TITLE_TAG, "activePlayerSourceItemTitles: name %s",
               name.c_str());
      auto new_menu_title =
          new MenuTitleSource(name, sourceItem->get_media_content_id(),
                              NoMenuTitleRightIcon, sourceItem);
      (*menu_titles).push_back(new_menu_title);
    }
  }
};

#endif  // now playing

}  // namespace homething_menu_base
}  // namespace esphome
