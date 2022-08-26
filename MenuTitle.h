
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
  GroupedMenuTitleState
};

class MenuTitle {
  public:
    MenuTitle(std::string newFriendlyName, std::string newEntityId, MenuTitleState newTitleState) : friendlyName(newFriendlyName), entityId(newEntityId), titleState(newTitleState) { }
    std::string friendlyName;
    std::string entityId;
    MenuTitleState titleState;
    
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

class FriendlyNameEntity
{
  public:
    FriendlyNameEntity(std::string newEntityId, std::string newFriendlyName) : entityId(newEntityId), friendlyName(newFriendlyName) { }
    std::string entityId;
    std::string friendlyName;
};

#endif