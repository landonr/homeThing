#pragma once

enum MenuTitleLeftIcon { NoMenuTitleLeftIcon, OffMenuTitleLeftIcon, OnMenuTitleLeftIcon, GroupedMenuTitleLeftIcon };

enum MenuTitleRightIcon {
  NoMenuTitleRightIcon,
  ArrowMenuTitleRightIcon
};

enum MenuTitleType {
  BaseMenuTitleType,
  PlayerMenuTitleType,
  LightMenuTitleType
};

class MenuTitleBase {
 public:
  std::string friendlyName;
  std::string entityId;
  MenuTitleLeftIcon leftIconState;
  MenuTitleRightIcon rightIconState;
  MenuTitleType titleType;
  MenuTitleBase(std::string newFriendlyName, std::string newEntityId, MenuTitleLeftIcon newLeftIconState,
                MenuTitleRightIcon newRightIconState, MenuTitleType newTitleType = BaseMenuTitleType)
      : friendlyName(newFriendlyName),
        entityId(newEntityId),
        leftIconState(newLeftIconState),
        rightIconState(newRightIconState),
        titleType(newTitleType) {}

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

enum RemotePlayerMediaSource {
  NoRemotePlayerMediaSource,
  HomeRemotePlayerMediaSource,
  YouTubeRemotePlayerMediaSource,
  SpotifyRemotePlayerMediaSource,
  NetflixRemotePlayerMediaSource,
  PlexRemotePlayerMediaSource,
  TVRemotePlayerMediaSource
};

enum RemotePlayerState {
  NoRemotePlayerState,
  UnavailableRemotePlayerState,
  PowerOffRemotePlayerState,
  StoppedRemotePlayerState,
  PausedRemotePlayerState,
  PlayingRemotePlayerState
};

std::string playerSourceStateString(RemotePlayerMediaSource playingState) {
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

class MenuTitlePlayer : public MenuTitleBase {
 public:
  RemotePlayerMediaSource mediaSource;
  RemotePlayerState playerState;

  MenuTitlePlayer(std::string newFriendlyName, std::string newEntityId, MenuTitleLeftIcon newLeftIconState,
                  MenuTitleRightIcon newRightIconState, RemotePlayerMediaSource newMediaSource,
                  RemotePlayerState newPlayerState)
      : MenuTitleBase{newFriendlyName, newEntityId, newLeftIconState, newRightIconState, PlayerMenuTitleType},
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

  Color mediaSourceIconColor() {
    switch (mediaSource) {
      case NoRemotePlayerMediaSource:
      case TVRemotePlayerMediaSource:
      case HomeRemotePlayerMediaSource:
        return id(color_accent_primary);
      case YouTubeRemotePlayerMediaSource:
        return Color(248, 0, 0);
      case SpotifyRemotePlayerMediaSource:
        return Color(27, 212, 96);
      case NetflixRemotePlayerMediaSource:
        return Color(179, 0, 10);
      case PlexRemotePlayerMediaSource:
        return Color(223, 156, 13);
    }
    return id(color_accent_primary);
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

  MenuTitleSource(std::string newFriendlyName, std::string newEntityId, MenuTitleLeftIcon newLeftIconState,
                  MenuTitleRightIcon newRightIconState, RemotePlayerSourceType newSourceType)
      : MenuTitleBase{newFriendlyName, newEntityId, newLeftIconState, newRightIconState}, sourceType(newSourceType) {}

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
  int slider_width;
  bool current_state;
  std::string title_extra;
  MenuTitleSlider(std::string newTitle, std::string newTitleExtra, std::string newEntityId,
                  MenuTitleLeftIcon newLeftIconState,
                  MenuTitleRightIcon newRightIconState, int sliderWidth)
      : MenuTitleBase{newTitle, newEntityId, newLeftIconState, newRightIconState, LightMenuTitleType},
        slider_width(sliderWidth),
        title_extra(newTitleExtra) {}
};
