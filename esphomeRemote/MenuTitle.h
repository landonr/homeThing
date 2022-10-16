#pragma once

enum MenuTitleState {
  NoMenuTitleState,
  OffMenuTitleState,
  OnMenuTitleState,
  ArrowMenuTitleState,
  PlayingMenuTitleState,
  PausedMenuTitleState,
  StoppedMenuTitleState,
  GroupedMenuTitleState,
  PowerOffMenuTitleState
};

class MenuTitleBase {
  public:
    std::string friendlyName;
    std::string entityId;
    MenuTitleState titleState;
    MenuTitleBase(std::string newFriendlyName, std::string newEntityId, MenuTitleState newTitleState) : friendlyName(newFriendlyName), entityId(newEntityId), titleState(newTitleState) { }
    
    bool indentLine() {
      switch (titleState) {
        case OffMenuTitleState:
        case OnMenuTitleState:
        case GroupedMenuTitleState:
          return true;
        default:
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

std::string playerSourceStateString(RemotePlayerMediaSource playingState) {
    switch(playingState) {
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

class MenuTitlePlayer: public MenuTitleBase {
  public:
    RemotePlayerMediaSource mediaSource;

    MenuTitlePlayer(
      std::string newFriendlyName, 
      std::string newEntityId, 
      MenuTitleState newTitleState,
      RemotePlayerMediaSource newMediaSource
    ) : MenuTitleBase { 
      newFriendlyName,
      newEntityId,
      newTitleState
    }, mediaSource(newMediaSource) {}

    std::string mediaSourceIcon() {
      switch(mediaSource) {
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
};

enum RemotePlayerSourceType {
  MusicRemotePlayerSourceType,
  FavoriteItemIDRemotePlayerSourceType,
  SourceRemotePlayerSourceType
};

class MenuTitleSource: public MenuTitleBase {
  public:
    RemotePlayerSourceType sourceType;

    MenuTitleSource(
      std::string newFriendlyName, 
      std::string newEntityId, 
      MenuTitleState newTitleState,
      RemotePlayerSourceType newSourceType
    ) : MenuTitleBase { 
      newFriendlyName,
      newEntityId,
      newTitleState
    }, sourceType(newSourceType) {}

    std::string sourceTypeString() {
      switch(sourceType) {
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
