
#ifndef MENUTITLE
#define MENUTITLE

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

enum MenuTitlePlayingSourceState {
  NoMenuTitlePlayingSourceState,
  YouTubeMenuTitlePlayingSourceState,
  SpotifyMenuTitlePlayingSourceState,
  NetflixMenuTitlePlayingSourceState,
  PlexMenuTitlePlayingSourceState,
  TVMenuTitlePlayingSourceState
};

std::string playingSourceStateString(MenuTitlePlayingSourceState playingState) {
  switch(playingState) {
    case NoMenuTitlePlayingSourceState:
      return "No";
    case YouTubeMenuTitlePlayingSourceState:
      return "YouTube";
    case SpotifyMenuTitlePlayingSourceState:
      return "Spotify";
    case NetflixMenuTitlePlayingSourceState:
      return "Netflix";
    case PlexMenuTitlePlayingSourceState:
      return "Plex";
    case TVMenuTitlePlayingSourceState:
      return "TV";
  }
  return "";
}

class MenuTitle {
  public:
    MenuTitle(std::string newFriendlyName, std::string newEntityId, MenuTitleState newTitleState) : friendlyName(newFriendlyName), entityId(newEntityId), titleState(newTitleState) { }
    MenuTitle(std::string newFriendlyName, std::string newEntityId, MenuTitleState newTitleState, MenuTitlePlayingSourceState newPlayingState) : friendlyName(newFriendlyName), entityId(newEntityId), titleState(newTitleState), playingState(newPlayingState) { }
    std::string friendlyName;
    std::string entityId;
    MenuTitleState titleState;
    MenuTitlePlayingSourceState playingState = NoMenuTitlePlayingSourceState;
    
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

    std::string playingSourceStateIcon() {
      switch(playingState) {
        case NoMenuTitlePlayingSourceState:
          return "󰐊";
        case YouTubeMenuTitlePlayingSourceState:
          return "󰗃";
        case SpotifyMenuTitlePlayingSourceState:
          return "󰓇";
        case NetflixMenuTitlePlayingSourceState:
          return "󰝆";
        case PlexMenuTitlePlayingSourceState:
          return "󰚺";
        case TVMenuTitlePlayingSourceState:
          return "󰔂";
      }
      return "";
    }
};

class FriendlyNameEntity
{
  public:
    FriendlyNameEntity(std::string newEntityId, std::string newFriendlyName) : entityId(newEntityId), friendlyName(newFriendlyName) { }
    std::string entityId;
    std::string friendlyName;
};

#endif