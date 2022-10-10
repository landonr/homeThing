
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
  PlexMenuTitlePlayingSourceState
};

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

    std::string playingSourceStateString() {
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