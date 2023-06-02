#pragma once

#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/script/script.h"
#include "version.h"

namespace esphome {
namespace homething_menu_base {

static const char* const MENU_TITLE_SCREEN_TAG = "homething.menutitle.screen";

class MenuCommand : public EntityBase {
 public:
  void on_command() { this->on_command_callbacks_.call(); }
  void add_on_command_callback(std::function<void()>&& callback) {
    this->on_command_callbacks_.add(std::move(callback));
  }
  
  std::string get_name() const {
    return name_;
  }
  
  void set_name(std::string val) {
    this->name_ = val;
  }

 private:
  CallbackManager<void()> on_command_callbacks_{};
  std::string name_;
};

class HomeThingMenuScreen {
 public:
  HomeThingMenuScreen(std::string name)
      : name_(name) {}
  void set_name(std::string name) { name_ = name; }
  std::string get_name() { return name_; }
  void set_index(int index) { index_ = index; }
  int get_index() { return index_; }

  void register_switch(switch_::Switch* new_switch) {
    switches_.push_back(new_switch);
    // new_switch->add_on_state_callback(
    //     [this, new_switch](bool state) { this->publish_state(0); });
  }

  void register_text_sensor(text_sensor::TextSensor* new_text_sensor) {
    text_sensors_.push_back(new_text_sensor);
    // new_text_sensor->add_on_state_callback(
    //     [this, new_text_sensor](std::string state) { this->publish_state(0); });
  }

  void register_command(MenuCommand* new_command) {
    menu_commands_.push_back(new_command);
  }

  std::vector<std::shared_ptr<MenuTitleBase>> menu_titles() {
    std::vector<std::shared_ptr<MenuTitleBase>> out;
    out.push_back(std::make_shared<MenuTitleBase>(this->get_name(), "",
                                                  NoMenuTitleRightIcon));

    auto versionString = COMPONENTS_HOMETHING_VERSION;
    out.push_back(std::make_shared<MenuTitleBase>(versionString, "",
                                                  NoMenuTitleRightIcon));
    for (const auto textSensor : text_sensors_) {
      ESP_LOGD(MENU_TITLE_SCREEN_TAG, "text sensor state %s",
               textSensor->state.c_str());
      if (textSensor->get_name() != "") {
        out.push_back(std::make_shared<MenuTitleBase>(
            textSensor->get_name() + " " + textSensor->get_state(), "",
            NoMenuTitleRightIcon));
      } else {
        out.push_back(std::make_shared<MenuTitleBase>(textSensor->state, "",
                                                      NoMenuTitleRightIcon));
      }
    }

    for (const auto switchObject : switches_) {
      ESP_LOGD(MENU_TITLE_SCREEN_TAG, "switch state %d", switchObject->state);
      MenuTitleLeftIcon state =
          switchObject->state ? OnMenuTitleLeftIcon : OffMenuTitleLeftIcon;
      out.push_back(std::make_shared<MenuTitleToggle>(
          switchObject->get_name(), switchObject->get_object_id(), state,
          NoMenuTitleRightIcon));
    }

    for (auto& command : menu_commands_) {
      if (command->get_name() != "") {
        out.push_back(std::make_shared<MenuTitleBase>(
            command->get_name(), "",
            NoMenuTitleRightIcon));
      } else {
        out.push_back(std::make_shared<MenuTitleBase>(command->get_object_id(), "",
                                                      NoMenuTitleRightIcon));
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
    if (index == 0) {
      ESP_LOGI(MENU_TITLE_SCREEN_TAG, "selected version %d", index);
      return false;
    }
    index -= 1;
    if (index < text_sensors_.size()) {
      ESP_LOGI(MENU_TITLE_SCREEN_TAG, "selected text sensor %d", index);
      return false;
    }
    index -= text_sensors_.size();
    if (index < switches_.size()) {
      ESP_LOGI(MENU_TITLE_SCREEN_TAG, "selected switch %d", index);
      auto switchObject = switches_[index];
      switchObject->toggle();
      return true;
    }
    index -= switches_.size();
    if (index < menu_commands_.size()) {
      ESP_LOGI(MENU_TITLE_SCREEN_TAG, "selected command %d", index);
      auto command = menu_commands_[index];
      command->on_command();
      return true;
    }
    return false;
  }

 private:
  int index_;
  std::string name_;
  std::vector<switch_::Switch*> switches_;
  std::vector<text_sensor::TextSensor*> text_sensors_;
  std::vector<MenuCommand*> menu_commands_;
};
}  // namespace homething_menu_base
}  // namespace esphome
