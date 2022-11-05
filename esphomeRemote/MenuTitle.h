#pragma once

enum MenuTitleState {
  NoMenuTitleState,
  OffMenuTitleState,
  OnMenuTitleState,
  ArrowMenuTitleState,
  GroupedMenuTitleState
};

enum MenuTitleType { BaseMenuTitleType, PlayerMenuTitleType, LightMenuTitleType };

class MenuTitleBase {
 public:
  std::string friendlyName;
  std::string entityId;
  MenuTitleState titleState;
  MenuTitleType titleType;
  MenuTitleBase(std::string newFriendlyName, std::string newEntityId, MenuTitleState newTitleState,
                MenuTitleType newTitleType = BaseMenuTitleType)
      : friendlyName(newFriendlyName), entityId(newEntityId), titleState(newTitleState), titleType(newTitleType) {}

  bool indentLine() {
    switch (titleState) {
      case OffMenuTitleState:
      case OnMenuTitleState:
      case GroupedMenuTitleState:
        return true;
      case NoMenuTitleState:
      case ArrowMenuTitleState:
        return false;
    }
    return false;
  }
};

enum RemotePlayerMediaSource {
  NoRemotePlayerMediaSource,
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
      return "No";
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

  MenuTitlePlayer(std::string newFriendlyName, std::string newEntityId, MenuTitleState newTitleState,
                  RemotePlayerMediaSource newMediaSource, RemotePlayerState newPlayerState)
      : MenuTitleBase{newFriendlyName, newEntityId, newTitleState, PlayerMenuTitleType},
        mediaSource(newMediaSource),
        playerState(newPlayerState) {}

  std::string mediaSourceIcon() {
    switch (mediaSource) {
      case NoRemotePlayerMediaSource:
        return "󰐊";
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

  MenuTitleSource(std::string newFriendlyName, std::string newEntityId, MenuTitleState newTitleState,
                  RemotePlayerSourceType newSourceType)
      : MenuTitleBase{newFriendlyName, newEntityId, newTitleState}, sourceType(newSourceType) {}

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
                  MenuTitleState newTitleState, int sliderWidth)
      : MenuTitleBase{newTitle, newEntityId, newTitleState, LightMenuTitleType},
        slider_width(sliderWidth),
        title_extra(newTitleExtra) {}
};
