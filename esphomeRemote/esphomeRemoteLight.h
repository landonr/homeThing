#include "esphome.h"
#include "DisplayUpdateInterface.h"
#include "MenuTitle.h"
#include "FriendlyNameEntity.h"

#pragma once

class LightService : public CustomAPIDevice, public Component {
 public:
  LightService(std::string newFriendlyName, std::string newEntityId, DisplayUpdateInterface &newCallback)
      : friendlyName(newFriendlyName), entityId(newEntityId), display(newCallback) {
    onState = false;
    subscribe_homeassistant_state(&LightService::state_changed, newEntityId.c_str());
  }
  std::string friendlyName;
  std::string entityId;
  DisplayUpdateInterface &display;
  bool onState;

  void toggleLight() { call_homeassistant_service("light.toggle", {{"entity_id", entityId.c_str()}}); }

 private:
  void state_changed(std::string newOnState) {
    ESP_LOGI("Light", "state changed to %s", newOnState.c_str());
    onState = newOnState == "on";
    display.updateDisplay(false);
  }
};

class LightGroupComponent : public CustomAPIDevice, public Component {
 public:
  explicit LightGroupComponent(DisplayUpdateInterface &newCallback) : display(newCallback) {}
  std::vector<LightService *> lights;

  void setup(std::vector<FriendlyNameEntity> newLights) {
    for (auto &light : newLights) {
      LightService *newService = new LightService(light.friendlyName, light.entityId, display);
      lights.push_back(newService);
    }
  }

  std::vector<std::shared_ptr<MenuTitleBase>> lightTitleSwitches() {
    std::vector<std::shared_ptr<MenuTitleBase>> out;
    for (auto &light : lights) {
      ESP_LOGI("Light", "state %d", light->onState);
      MenuTitleState state = light->onState ? OnMenuTitleState : OffMenuTitleState;
      out.push_back(std::make_shared<MenuTitleBase>(light->friendlyName, light->entityId, state));
    }
    return out;
  }

  bool selectLight(int index) {
    if (index >= 0 && index < lights.size()) {
      LightService *light = lights[index];
      light->toggleLight();
      return false;
    }
    return true;
  }

 private:
  DisplayUpdateInterface &display;
};
