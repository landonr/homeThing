#include "esphome.h"
#include "MenuTitle.h"
#include "FriendlyNameEntity.h"

#pragma once

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
      ESP_LOGD("Sensor", "%s state changed to %s", friendlyName.c_str(), newState.c_str());
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

    std::vector<MenuTitleBase*> sensorTitles() {
      std::vector<MenuTitleBase*> out;
      for (auto &sensor: sensors) {
        if(sensor->friendlyName != "") {
          out.push_back(new MenuTitleBase(sensor->friendlyName + " " + sensor->state, sensor->entityId, NoMenuTitleState));
        } else {
          out.push_back(new MenuTitleBase(sensor->state, sensor->entityId, NoMenuTitleState));
        }
      }
      return out;
    }
};
