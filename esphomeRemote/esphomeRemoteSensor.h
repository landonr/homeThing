#include <cwchar>
#include "esphome.h"
#include "MenuTitle.h"
#include "FriendlyNameEntity.h"

#pragma once

#include <string>
#include "HTTPClient.h"
#include "esphome/core/application.h"
#include <ArduinoJson.h>
#include "esphome/components/http_request/http_request.h"
#include <map>
#include "esphomeRemoteGrapher.h"

std::map<uint32_t, float> temperature_data;

// TODO: don't copy full vector!
std::vector<float> get_sensor_data() {
  std::vector<float> values;
  const char *url =
      "http://<HA_IP>/api/history/period/"
      "2023-02-02T12:00:00+00:00?filter_entity_id=sensor.<HA_SENSOR_NAME>&minimal_response&no_attributes";
  const char *token = "Bearer <HA_TOKEN>";

  // Make the request
  HTTPClient http;
  http.begin(url);
  http.addHeader("Authorization", token);

  // Send the request and get the response
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    DynamicJsonDocument doc(4096);
    std::string s = http.getString().c_str();
    auto stream = http.getStream();
    auto error = deserializeJson(doc, s.c_str());
    // auto error = deserializeJson(doc, http.getString().c_str());
    if (error) {
      ESP_LOGD("Sensor", "oh no parsing failed");
    } else {
      ESP_LOGD("Sensor", "parsing successful");
      JsonArray root = doc[0];

      for(int v = 0; v< root.size();++v){

      float bla = root[v]["state"].as<float>();
    values.push_back(float(bla));
      }
    }
  } else {
    ESP_LOGD("Sensor", "http request failed");
  }
  http.end();
  return values;
}

class SensorComponent : public CustomAPIDevice, public Component {
 public:
  SensorComponent(FriendlyNameEntity newFriendlyEntity)
      : entityId(newFriendlyEntity.entityId), friendlyName(newFriendlyEntity.friendlyName) {
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
  SensorGroupComponent(DisplayBuffer *db) : _db(db){};
  std::vector<SensorComponent *> sensors;

  void setup(std::vector<FriendlyNameEntity> newSensors) {
    for (auto &sensor : newSensors) {
      SensorComponent *newSensor = new SensorComponent(sensor);
      sensors.push_back(newSensor);
    }
  }

  std::vector<std::shared_ptr<MenuTitleBase>> sensorTitles() {
    std::vector<std::shared_ptr<MenuTitleBase>> out;
    for (auto &sensor : sensors) {
      if (sensor->friendlyName != "") {
        out.push_back(std::make_shared<MenuTitleBase>(sensor->friendlyName + " " + sensor->state, sensor->entityId,
                                                      NoMenuTitleRightIcon));
      } else {
        out.push_back(std::make_shared<MenuTitleBase>(sensor->state, sensor->entityId, NoMenuTitleRightIcon));
      }
    }
    return out;
  }

  void selectSensorAtIndex(int index) {
    if (sensors.size() < index) {
      ESP_LOGE("sensor", "selecting out of bounds sensor");
      return;
    }
    _activeSensor = sensors[index];
  }

  void clearActiveSensor() { _activeSensor = NULL; }

  SensorComponent *getActiveSensor() { return _activeSensor; }

  void draw() {
    if (_alreadyDrawn) {
      // return;
    }
    _alreadyDrawn = true;
    ESP_LOGW("WARNING", "SensorGroupComponent draws");
    std::string msg = "Show graph for: " + _activeSensor->friendlyName;
    // std::string msg = "Show graph for: ";
    id(_db).printf(4, 20 + 1, &id(medium_font), id(my_white), msg.c_str());

    Graph graph = Graph(_db, get_sensor_data());
    graph.draw();
  }
  bool sensorDetailSelected = false;

 private:
  SensorComponent *_activeSensor = NULL;
  DisplayBuffer *_db;
  bool _alreadyDrawn = false;
};
