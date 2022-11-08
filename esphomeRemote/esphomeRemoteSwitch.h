#include "esphome.h"
#include "DisplayUpdateInterface.h"
#include "MenuTitle.h"
#include "FriendlyNameEntity.h"

#pragma once

class SwitchService : public CustomAPIDevice, public Component {
 public:
  SwitchService(std::string newFriendlyName, std::string newEntityId, DisplayUpdateInterface &newCallback)
      : friendlyName(newFriendlyName), entityId(newEntityId), display(newCallback) {
    onState = false;
    subscribe_homeassistant_state(&SwitchService::state_changed, newEntityId.c_str());
  }
  std::string friendlyName;
  std::string entityId;
  DisplayUpdateInterface &display;
  bool onState;

  void toggleSwitch() { call_homeassistant_service("switch.toggle", {{"entity_id", entityId.c_str()}}); }

 private:
  void state_changed(std::string newOnState) {
    ESP_LOGI("Switch", "state changed to %s", newOnState.c_str());
    onState = newOnState == "on";
    display.updateDisplay(false);
  }
};

class SwitchGroupComponent : public CustomAPIDevice, public Component {
 public:
  explicit SwitchGroupComponent(DisplayUpdateInterface &newCallback) : display(newCallback) {}
  std::vector<SwitchService *> switches;

  void setup(std::vector<FriendlyNameEntity> newSwitches) {
    for (auto &swytch : newSwitches) {
      SwitchService *newService = new SwitchService(swytch.friendlyName, swytch.entityId, display);
      switches.push_back(newService);
    }
  }

  std::vector<std::shared_ptr<MenuTitleBase>> switchTitleSwitches() {
    std::vector<std::shared_ptr<MenuTitleBase>> out;
    for (auto &swytch : switches) {
      ESP_LOGI("Switch", "state %d", swytch->onState);
      MenuTitleState state = swytch->onState ? OnMenuTitleState : OffMenuTitleState;
      out.push_back(std::make_shared<MenuTitleBase>(swytch->friendlyName, swytch->entityId, state));
    }
    return out;
  }

  bool selectSwitch(int index) {
    if (index >= 0 && index < switches.size()) {
      SwitchService *swytch = switches[index];
      swytch->toggleSwitch();
      return false;
    }
    return true;
  }

 private:
  DisplayUpdateInterface &display;
};
