
#ifndef MENUTITLE
#define MENUTITLE

enum MenuTitleState {
  NoMenuTitleState,
  OffMenuTitleState,
  OnMenuTitleState,
  ArrowMenuTitleState
};

class MenuTitle {
  public:
    MenuTitle(std::string newFriendlyName, std::string newEntityName, MenuTitleState newTitleState) : friendlyName(newFriendlyName), entityName(newEntityName), titleState(newTitleState) { }
    std::string friendlyName;
    std::string entityName;
    MenuTitleState titleState;
    
    bool indentLine() {
      switch (titleState) {
        case NoMenuTitleState:
        case ArrowMenuTitleState:
          return false;
        case OffMenuTitleState:
        case OnMenuTitleState:
          return true;
      }
      return false;
    }
};

class FriendEntity
{
  public:
    FriendEntity(std::string newEntityId, std::string newFriendlyName) : entityId(newEntityId), friendlyName(newFriendlyName) { }
    std::string entityId;
    std::string friendlyName;
};

#endif