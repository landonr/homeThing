#pragma once

#include "esphome/components/homeThing/homeThingMenuTitle.h"

#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif

#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif

#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif

#ifdef SHOW_VERSION
#include "version.h"
#endif

#ifdef USE_LIGHT
#include "esphome/components/homeThing/homeThingMenuTitleLight.h"
#endif

#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif

namespace esphome {
namespace homething_menu_base {

static const char* const MENU_TITLE_SCREEN_TAG = "homething.menutitle.screen";

class MenuCommand : public EntityBase {
 public:
  void on_command() { this->on_command_callbacks_.call(); }
  void add_on_command_callback(std::function<void()>&& callback) {
    this->on_command_callbacks_.add(std::move(callback));
  }

  std::string get_name() const { return name_; }

  void set_name(std::string val) { this->name_ = val; }

 private:
  CallbackManager<void()> on_command_callbacks_{};
  std::string name_;
};

enum MenuItemType {
  MenuItemTypeNone,
  MenuItemTypeTitle,
  MenuItemTypeSwitch,
  MenuItemTypeTextSensor,
  MenuItemTypeCommand,
  MenuItemTypeSensor,
  MenuItemTypeLight,
  MenuItemTypeNumber
};

class HomeThingMenuScreen {
 public:
  HomeThingMenuScreen(std::string name) : name_(name) {}
  void set_name(std::string name) { name_ = name; }
  std::string get_name() { return name_; }
  void set_index(int index) { index_ = index; }
  int get_index() { return index_; }
  void set_show_version(bool show_version) { show_version_ = show_version; }
  void set_selected_entity(
      const std::tuple<MenuItemType, EntityBase*>* entity) {
    selected_entity_ = entity;
  }
  const std::tuple<MenuItemType, EntityBase*>* get_selected_entity() {
    return selected_entity_;
  }

#ifdef USE_SWITCH
  void register_switch(switch_::Switch* new_switch) {
    entities_.push_back(std::make_tuple(MenuItemTypeSwitch, new_switch));
    // new_switch->add_on_state_callback(
    //     [this, new_switch](bool state) { this->publish_state(0); });
  }
#endif

#ifdef USE_TEXT_SENSOR
  void register_text_sensor(text_sensor::TextSensor* new_text_sensor) {
    entities_.push_back(
        std::make_tuple(MenuItemTypeTextSensor, new_text_sensor));
    // new_text_sensor->add_on_state_callback(
    //     [this, new_text_sensor](std::string state) { this->publish_state(0); });
  }
#endif

#ifdef USE_COMMAND
  void register_command(MenuCommand* new_command) {
    entities_.push_back(std::make_tuple(MenuItemTypeCommand, new_command));
  }
#endif

#ifdef USE_SENSOR
  void register_sensor(sensor::Sensor* new_sensor) {
    entities_.push_back(std::make_tuple(MenuItemTypeSensor, new_sensor));
  }
#endif

#ifdef USE_LIGHT
  void register_light(light::LightState* new_light) {
    entities_.push_back(std::make_tuple(MenuItemTypeLight, new_light));
  }
#endif

#ifdef USE_NUMBER
  void register_number(number::Number* new_number) {
    entities_.push_back(std::make_tuple(MenuItemTypeNumber, new_number));
  }
#endif

  std::vector<std::shared_ptr<MenuTitleBase>> menu_titles() {
    std::vector<std::shared_ptr<MenuTitleBase>> out;
    out.push_back(std::make_shared<MenuTitleBase>(this->get_name(), "",
                                                  NoMenuTitleRightIcon));
#ifdef SHOW_VERSION
    if (show_version_) {
      auto versionString = COMPONENTS_HOMETHING_VERSION;
      out.push_back(std::make_shared<MenuTitleBase>(versionString, "",
                                                    NoMenuTitleRightIcon));
    }
#endif

    for (const auto entity : entities_) {
      switch (std::get<0>(entity)) {
        case MenuItemTypeTitle:
          out.push_back(std::make_shared<MenuTitleBase>(
              std::get<1>(entity)->get_name(), "", NoMenuTitleRightIcon));
          break;
        case MenuItemTypeSwitch: {
#ifdef USE_SWITCH
          auto switchObject =
              static_cast<switch_::Switch*>(std::get<1>(entity));
          ESP_LOGD(MENU_TITLE_SCREEN_TAG, "switch state %d",
                   switchObject->state);
          MenuTitleLeftIcon state =
              switchObject->state ? OnMenuTitleLeftIcon : OffMenuTitleLeftIcon;
          out.push_back(std::make_shared<MenuTitleToggle>(
              switchObject->get_name(), switchObject->get_object_id(), state,
              NoMenuTitleRightIcon));
#endif
          break;
        }
        case MenuItemTypeTextSensor: {
#ifdef USE_TEXT_SENSOR
          auto textSensor =
              static_cast<text_sensor::TextSensor*>(std::get<1>(entity));
          ESP_LOGD(MENU_TITLE_SCREEN_TAG, "text sensor state %s",
                   textSensor->state.c_str());
          if (textSensor->get_name() != "") {
            out.push_back(std::make_shared<MenuTitleBase>(
                textSensor->get_name() + " " + textSensor->get_state(), "",
                NoMenuTitleRightIcon));
          } else {
            out.push_back(std::make_shared<MenuTitleBase>(
                textSensor->state, "", NoMenuTitleRightIcon));
          }
#endif
          break;
        }
        case MenuItemTypeCommand: {
          auto command = static_cast<MenuCommand*>(std::get<1>(entity));
          if (command->get_name() != "") {
            out.push_back(std::make_shared<MenuTitleBase>(
                command->get_name(), "", NoMenuTitleRightIcon));
          } else {
            out.push_back(std::make_shared<MenuTitleBase>(
                command->get_object_id(), "", NoMenuTitleRightIcon));
          }
          break;
        }
        case MenuItemTypeSensor: {
#ifdef USE_SENSOR
          auto sensor = static_cast<sensor::Sensor*>(std::get<1>(entity));
          auto state = to_string(static_cast<int>(sensor->get_state())).c_str();
          if (sensor->get_name() != "") {
            out.push_back(std::make_shared<MenuTitleBase>(
                sensor->get_name() + ": " + state, "", NoMenuTitleRightIcon));
          } else {
            out.push_back(std::make_shared<MenuTitleBase>(
                sensor->get_object_id() + ": " + state, "",
                NoMenuTitleRightIcon));
          }
#endif
          break;
        }
        case MenuItemTypeLight: {
#ifdef USE_LIGHT
          auto light = static_cast<light::LightState*>(std::get<1>(entity));
          // auto output = static_cast<light::LightOutput*>(light->get_output());
          MenuTitleLeftIcon state = light->remote_values.is_on()
                                        ? OnMenuTitleLeftIcon
                                        : OffMenuTitleLeftIcon;
          MenuTitleRightIcon rightIcon = supportsBrightness(light)
                                             ? ArrowMenuTitleRightIcon
                                             : NoMenuTitleRightIcon;
          out.push_back(std::make_shared<MenuTitleLight>(
              light->get_name(), "", state, rightIcon,
              light::rgbLightColor(light)));
#endif
          break;
        }
        case MenuItemTypeNumber: {
#ifdef USE_NUMBER
          auto number = static_cast<number::Number*>(std::get<1>(entity));
          auto state = to_string(number->state).c_str();
          if (number->get_name() != "") {
            out.push_back(std::make_shared<MenuTitleBase>(
                number->get_name() + ": " + state, "", NoMenuTitleRightIcon));
          } else {
            out.push_back(std::make_shared<MenuTitleBase>(
                number->get_object_id() + ": " + state, "",
                NoMenuTitleRightIcon));
          }
#endif
          break;
        }
      }
    }
    return out;
  }

  bool select_menu(int index) {
    if (index == 0) {
      ESP_LOGI(MENU_TITLE_SCREEN_TAG, "selected name %d", index);
      return false;
    }
    index -= 1;
#ifdef SHOW_VERSION
    if (show_version_) {
      if (index == 0) {
        ESP_LOGI(MENU_TITLE_SCREEN_TAG, "selected version %d", index);
        return false;
      }
      index -= 1;
    }
#endif
    auto entity = entities_[index];
    switch (std::get<0>(entity)) {
      case MenuItemTypeTitle:
        return false;
      case MenuItemTypeSwitch: {
#ifdef USE_SWITCH
        ESP_LOGI(MENU_TITLE_SCREEN_TAG, "selected switch %d", index);
        auto switchObject =
            static_cast<switch_::Switch*>(std::get<1>(entities_[index]));
        switchObject->toggle();
        return true;
#endif
      }
      case MenuItemTypeTextSensor: {
        ESP_LOGI(MENU_TITLE_SCREEN_TAG, "selected text sensor %d", index);
        return false;
      }
      case MenuItemTypeCommand: {
        ESP_LOGI(MENU_TITLE_SCREEN_TAG, "selected command %d", index);
        auto command = static_cast<MenuCommand*>(std::get<1>(entities_[index]));
        command->on_command();
        return true;
      }
      case MenuItemTypeSensor: {
        ESP_LOGI(MENU_TITLE_SCREEN_TAG, "selected sensor %d", index);
        return false;
      }
      case MenuItemTypeLight: {
        ESP_LOGI(MENU_TITLE_SCREEN_TAG, "selected light %d", index);
#ifdef USE_LIGHT
        auto light =
            static_cast<light::LightState*>(std::get<1>(entities_[index]));
        light->toggle().perform();
#endif
        return true;
      }
      case MenuItemTypeNumber: {
        ESP_LOGI(MENU_TITLE_SCREEN_TAG, "selected number %d", index);
        return false;
      }
    }
    return false;
  }

  bool select_menu_hold(int index) {
    if (index == 0) {
      ESP_LOGI(MENU_TITLE_SCREEN_TAG, "select hold name %d", index);
      return false;
    }
    index -= 1;
#ifdef SHOW_VERSION
    if (show_version_) {
      if (index == 0) {
        ESP_LOGI(MENU_TITLE_SCREEN_TAG, "select hold version %d", index);
        return false;
      }
      index -= 1;
    }
#endif
    return false;
  }

  const std::tuple<MenuItemType, EntityBase*>* get_menu_item(int index) {
    // name isnt an entity
    index -= 1;
#ifdef SHOW_VERSION
    if (show_version_) {
      index -= 1;
    }
#endif
    return &entities_[index];
  }

 private:
  int index_;
  bool show_version_ = false;
  std::string name_;
  std::vector<std::tuple<MenuItemType, EntityBase*>> entities_;
  const std::tuple<MenuItemType, EntityBase*>* selected_entity_;
};

}  // namespace homething_menu_base
}  // namespace esphome
