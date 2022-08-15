
#ifndef MENUTITLESWTCH
#define MENUTITLESWTCH

class MenuTitleSwitch {
  public:
    MenuTitleSwitch(std::string newFriendlyName, std::string newEntityName, int newToggleState) : friendlyName(newFriendlyName), entityName(newEntityName), toggleState(newToggleState) { }
    std::string friendlyName;
    std::string entityName;
    int toggleState;
};

class FriendEntity
{
  public:
    FriendEntity(std::string newEntityId, std::string newFriendlyName) : entityId(newEntityId), friendlyName(newFriendlyName) { }
    std::string entityId;
    std::string friendlyName;
};
#endif