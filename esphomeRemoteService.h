#include "esphome.h"
#include "DisplayUpdateInterface.h"
#include "MenuTitle.h"

#ifndef REMOTESERVICE
#define REMOTESERVICE

class LightService: public CustomAPIDevice, public Component {
  public:
    LightService(std::string newFriendlyName, std::string newEntityId, DisplayUpdateInterface& newCallback) : friendlyName(newFriendlyName), entityId(newEntityId), display(newCallback) {
      onState = false;
      subscribe_homeassistant_state(&LightService::state_changed, newEntityId.c_str());
    }
    std::string friendlyName;
    std::string entityId;
    DisplayUpdateInterface& display;
    bool onState;

    void toggleLight() {
      call_homeassistant_service("light.toggle", {
        {"entity_id", entityId.c_str()}
      });
    }

  private:
    void state_changed(std::string newOnState) {
      ESP_LOGD("Light", "state changed to %s", newOnState.c_str());
      onState = newOnState == "on";
      display.updateDisplay(false);
    }
};

class LightGroupComponent : public CustomAPIDevice, public Component {
  public:
    LightGroupComponent(DisplayUpdateInterface& newCallback) : display(newCallback) { }
    std::vector<LightService*> lights;

    void setup(std::vector<FriendEntity> newLights) {
      for (auto &light: newLights) {
        LightService *newService = new LightService(light.friendlyName, light.entityId, display);
        lights.push_back(newService);
      }
    }

    std::vector<MenuTitle> lightTitleSwitches() {
      std::vector<MenuTitle> out;
      for (auto &light: lights) {
        ESP_LOGD("Light", "state %d", light->onState);
        MenuTitleState state = light->onState ? OnMenuTitleState : OffMenuTitleState;
        out.push_back(MenuTitle(light->friendlyName, light->entityId, state));
      }
      return out;
    }

    bool selectLight(int index) {
     if(index >= 0 && index < lights.size()) {
        LightService *light = lights[index];
        light->toggleLight();
        return false;
      }
      return true;
    }
  private:
    DisplayUpdateInterface& display;
};

class BaseService: public CustomAPIDevice {
  public:
    std::string entityId;
    std::string friendlyName;
    std::string serviceType;
    std::string getEntityId() { return entityId; }
    std::string getFriendlyName() { return friendlyName; }
    void callService() {
      if (serviceType == "script") {
        call_homeassistant_service(entityId.c_str());
      } else {
        call_homeassistant_service("scene.turn_on", {
          {"entity_id", entityId.c_str()}
        });
      }
    }

    void superSetup(std::string newEntityId, std::string newFriendlyName) {
      entityId = newEntityId;
      friendlyName = newFriendlyName;
    }
};

class SceneService: public BaseService {
  public:
    SceneService(std::string newEntityId, std::string newFriendlyName) {
      serviceType = "scene";
      superSetup(newEntityId, newFriendlyName); 
   }
};

class ScriptService: public BaseService {
  public:
    ScriptService(std::string newEntityId, std::string newFriendlyName) {
      serviceType = "script";
      superSetup(newEntityId, newFriendlyName); 
   }
};

class SceneGroupComponent : public CustomAPIDevice, public Component {
 public:
  std::vector<BaseService> services;

  void setup(std::vector<BaseService> newServices) {
    services = newServices;
  }

  std::vector<MenuTitle> sceneTitleStrings() {
    std::vector<MenuTitle> out;
    for (auto &service: services) {
      out.push_back(MenuTitle(service.getFriendlyName(), "", NoMenuTitleState));
    }
    return out;
  }

  bool selectScene(int index) {
   if(index >= 0 && index < services.size()) {
      BaseService service = services[index];
      ESP_LOGD("Scene", "Service called %s", service.getFriendlyName().c_str());
      service.callService();
      return false;
    }
    return true;
  }
};

#endif