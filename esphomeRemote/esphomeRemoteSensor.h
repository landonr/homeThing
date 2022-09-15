#include "esphome.h"
#include "MenuTitle.h"

#ifndef REMOTESENSOR
#define REMOTESENSOR

class SensorComponent : public CustomAPIDevice, public Component {
  public:
    SensorComponent(
      FriendlyNameEntity newFriendlyEntity
    ) : entityId(newFriendlyEntity.entityId), friendlyName(newFriendlyEntity.friendlyName) {
      subscribe_homeassistant_state(&SensorComponent::state_changed, newFriendlyEntity.entityId.c_str());
    }
    std::string entityId;
    std::string friendlyName;
    std::string state = "";


  private:
    void state_changed(std::string newState) {
      ESP_LOGI("Sensor", "state changed to %s", newState.c_str());
      state = newState;
    }
};

class SensorGroupComponent : public CustomAPIDevice, public Component {
  public:
    std::vector<SensorComponent*> sensors;

    void setup(std::vector<FriendlyNameEntity> newSensors) {
      for (auto &sensor: newSensors) {
        SensorComponent *newSensor = new SensorComponent(sensor);
        sensors.push_back(newSensor);
      }
    }

    std::vector<MenuTitle> sensorTitles() {
      std::vector<MenuTitle> out;
      for (auto &sensor: sensors) {
        ESP_LOGI("Sensor", "state %s", sensor->state.c_str());
        if(sensor->friendlyName != "") {
          out.push_back(MenuTitle(sensor->friendlyName + " " + sensor->state, sensor->entityId, NoMenuTitleState));
        } else {
          out.push_back(MenuTitle(sensor->state, sensor->entityId, NoMenuTitleState));
        }
      }
      return out;
    }
};

#endif