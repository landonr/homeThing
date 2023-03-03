#pragma once
#include <string>
#include "esphome/core/color.h"

namespace esphome {

// enum RemotePlayerType { TVRemotePlayerType, SpeakerRemotePlayerType };

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
  LightMenuTitleType
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

// enum RemotePlayerMediaSource {
//   NoRemotePlayerMediaSource,
//   HomeRemotePlayerMediaSource,
//   YouTubeRemotePlayerMediaSource,
//   SpotifyRemotePlayerMediaSource,
//   NetflixRemotePlayerMediaSource,
//   PlexRemotePlayerMediaSource,
//   TVRemotePlayerMediaSource
// };

// enum RemotePlayerState {
//   NoRemotePlayerState,
//   UnavailableRemotePlayerState,
//   PowerOffRemotePlayerState,
//   StoppedRemotePlayerState,
//   PausedRemotePlayerState,
//   PlayingRemotePlayerState
// };

static std::string playerSourceStateString(
    RemotePlayerMediaSource playingState) {
  switch (playingState) {
    case NoRemotePlayerMediaSource:
      return "No Source";
    case HomeRemotePlayerMediaSource:
      return "Home";
    case YouTubeRemotePlayerMediaSource:
      return "YouTube";
    case SpotifyRemotePlayerMediaSource:
      return "Spotify";
    case NetflixRemotePlayerMediaSource:
      return "Netflix";
    case PlexRemotePlayerMediaSource:
      return "Plex";
    case TVRemotePlayerMediaSource:
      return "TV";
  }
  return "";
}

class MenuTitlePlayer : public MenuTitleToggle {
 public:
  RemotePlayerMediaSource mediaSource;
  RemotePlayerState playerState;

  MenuTitlePlayer(std::string new_name, std::string newEntityId,
                  MenuTitleLeftIcon newLeftIconState,
                  MenuTitleRightIcon newRightIconState,
                  RemotePlayerMediaSource newMediaSource,
                  RemotePlayerState newPlayerState)
      : MenuTitleToggle{new_name, newEntityId, newLeftIconState,
                        newRightIconState, PlayerMenuTitleType},
        mediaSource(newMediaSource),
        playerState(newPlayerState) {}

  std::string mediaSourceIcon() {
    switch (mediaSource) {
      case NoRemotePlayerMediaSource:
        return "󰐊";
      case HomeRemotePlayerMediaSource:
        return "󰋜";
      case YouTubeRemotePlayerMediaSource:
        return "󰗃";
      case SpotifyRemotePlayerMediaSource:
        return "󰓇";
      case NetflixRemotePlayerMediaSource:
        return "󰝆";
      case PlexRemotePlayerMediaSource:
        return "󰚺";
      case TVRemotePlayerMediaSource:
        return "󰔂";
    }
    return "";
  }

  Color mediaSourceIconColor(Color defaultColor) {
    switch (mediaSource) {
      case NoRemotePlayerMediaSource:
      case TVRemotePlayerMediaSource:
      case HomeRemotePlayerMediaSource:
        return defaultColor;
      case YouTubeRemotePlayerMediaSource:
        return Color(248, 0, 0);
      case SpotifyRemotePlayerMediaSource:
        return Color(27, 212, 96);
      case NetflixRemotePlayerMediaSource:
        return Color(179, 0, 10);
      case PlexRemotePlayerMediaSource:
        return Color(223, 156, 13);
    }
    return defaultColor;
  }
};

enum RemotePlayerSourceType {
  MusicRemotePlayerSourceType,
  FavoriteItemIDRemotePlayerSourceType,
  SourceRemotePlayerSourceType
};

class MenuTitleSource : public MenuTitleBase {
 public:
  RemotePlayerSourceType sourceType;

  MenuTitleSource(std::string new_name, std::string newEntityId,
                  MenuTitleRightIcon newRightIconState,
                  RemotePlayerSourceType newSourceType)
      : MenuTitleBase{new_name, newEntityId, newRightIconState},
        sourceType(newSourceType) {}

  std::string sourceTypeString() {
    switch (sourceType) {
      case MusicRemotePlayerSourceType:
        return "music";
      case FavoriteItemIDRemotePlayerSourceType:
        return "favorite_item_id";
      case SourceRemotePlayerSourceType:
        return "source";
    }
    return "";
  }
};

class MenuTitleSlider : public MenuTitleBase {
 public:
  bool slider_{false};
  bool currentState;
  int sliderValue;
  int displayValue;
  std::string sliderUnit;
  MenuTitleSlider(std::string newTitle, std::string newEntityId,
                  MenuTitleRightIcon newRightIconState, int newSliderValue,
                  int newDisplayValue, std::string newSliderUnit)
      : MenuTitleBase{newTitle, newEntityId, newRightIconState,
                      SliderMenuTitleType},
        sliderValue(newSliderValue),
        displayValue(newDisplayValue),
        sliderUnit(newSliderUnit) {}
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
}  // namespace esphome
