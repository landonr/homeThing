#pragma once

class FriendlyNameEntity {
 public:
  FriendlyNameEntity(std::string newEntityId, std::string newFriendlyName)
      : entityId(newEntityId), friendlyName(newFriendlyName) {}
  std::string entityId;
  std::string friendlyName;
};
