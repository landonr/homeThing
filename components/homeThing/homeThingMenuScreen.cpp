#include "homeThingMenuScreen.h"

#ifdef USE_LIGHT
#include "esphome/components/homeThing/homeThingMenuTitleLight.h"
#endif

namespace esphome {
namespace homething_menu_base {

std::string HomeThingMenuScreen::entity_name_at_index(int index) {
  auto entity = entities_[index];
  switch (std::get<0>(entity)) {
    case MenuItemTypeNone:
    case MenuItemTypeTitle:
    case MenuItemTypeSwitch:
    case MenuItemTypeCover:
    case MenuItemTypeLight:
    case MenuItemTypeButton:
      break;
    case MenuItemTypeTextSensor: {
#ifdef USE_TEXT_SENSOR
      auto textSensor =
          static_cast<text_sensor::TextSensor*>(std::get<1>(entity));
      if (textSensor->get_name() != "") {
        return textSensor->get_name() + " " + textSensor->get_state();
      } else {
        return textSensor->state;
      }
#endif
      break;
    }
    case MenuItemTypeCommand: {
      auto command = static_cast<MenuCommand*>(std::get<1>(entity));
      if (command->get_name() != "") {
        return command->get_name();
      } else {
        return command->get_object_id();
      }
    }
    case MenuItemTypeSensor: {
#ifdef USE_SENSOR
      auto sensor = static_cast<sensor::Sensor*>(std::get<1>(entity));
      auto state = to_string(static_cast<int>(sensor->get_state())).c_str();
      if (sensor->get_name() != "") {
        return sensor->get_name() + ": " + state;
      } else {
        return sensor->get_object_id() + ": " + state;
      }
#endif
      break;
    }
    case MenuItemTypeNumber: {
#ifdef USE_NUMBER
      auto number = static_cast<number::Number*>(std::get<1>(entity));
      auto name = number->get_name() == "" ? number->get_object_id()
                                           : number->get_name();
      auto title = name + ": " + value_accuracy_to_string(number->state, 1);
      return title;
#endif
      break;
    }
  }
  return std::get<1>(entity)->get_name();
}

void HomeThingMenuScreen::menu_titles(std::vector<MenuTitleBase*>* menu_titles,
                                      bool show_name) {
  if (show_name) {
    ESP_LOGI(TAG, "menu_titles name %s", this->get_name().c_str());
    menu_titles->push_back(
        new MenuTitleBase(this->get_name().c_str(), "", NoMenuTitleRightIcon));
    show_name_ = true;
  }
#ifdef SHOW_VERSION
  if (show_version_) {
    auto versionString = COMPONENTS_HOMETHING_VERSION;
    menu_titles->push_back(
        new MenuTitleBase(versionString, "", NoMenuTitleRightIcon));
  }
#endif
  int index = 0;
  for (const auto entity : entities_) {
    auto title = entity_name_at_index(index);
    switch (std::get<0>(entity)) {
      case MenuItemTypeNone:
        break;
        menu_titles->push_back(new MenuTitleBase(
            std::get<1>(entity)->get_name(), "", NoMenuTitleRightIcon));
        break;
      case MenuItemTypeLight: {
#ifdef USE_LIGHT
        auto light = static_cast<light::LightState*>(std::get<1>(entity));
        MenuTitleLeftIcon state = light->remote_values.is_on()
                                      ? OnMenuTitleLeftIcon
                                      : OffMenuTitleLeftIcon;
        MenuTitleRightIcon rightIcon = supportsBrightness(light)
                                           ? ArrowMenuTitleRightIcon
                                           : NoMenuTitleRightIcon;
        menu_titles->push_back(new MenuTitleLight(title, "", state, rightIcon,
                                                  light::rgbLightColor(light)));
        break;
#endif
      }
      case MenuItemTypeCover: {
#ifdef USE_COVER
        auto coverObject = static_cast<cover::Cover*>(std::get<1>(entity));
        ESP_LOGD(TAG, "cover state %d", coverObject->is_fully_closed());
        MenuTitleLeftIcon state = coverObject->is_fully_closed()
                                      ? OffMenuTitleLeftIcon
                                      : OnMenuTitleLeftIcon;
        menu_titles->push_back(new MenuTitleToggle(
            title, coverObject->get_object_id(), state, NoMenuTitleRightIcon));
        break;
#endif
      }
      case MenuItemTypeSwitch: {
#ifdef USE_SWITCH
        auto switchObject = static_cast<switch_::Switch*>(std::get<1>(entity));
        ESP_LOGD(TAG, "switch state %d", switchObject->state);
        MenuTitleLeftIcon state =
            switchObject->state ? OnMenuTitleLeftIcon : OffMenuTitleLeftIcon;
        menu_titles->push_back(new MenuTitleToggle(
            title, switchObject->get_object_id(), state, NoMenuTitleRightIcon));
#endif
        break;
      }
      case MenuItemTypeSensor: {
#ifdef USE_SENSOR
        auto sensor = static_cast<sensor::Sensor*>(std::get<1>(entity));
        auto state = to_string(static_cast<int>(sensor->get_state())).c_str();
        menu_titles->push_back(new MenuTitleBase(
            sensor->get_name() + ": " + state, "", NoMenuTitleRightIcon));

#endif
        break;
      }
      case MenuItemTypeNumber:
      case MenuItemTypeButton:
      case MenuItemTypeTextSensor:
      case MenuItemTypeTitle:
      case MenuItemTypeCommand:
        menu_titles->push_back(
            new MenuTitleBase(title, "", NoMenuTitleRightIcon));
        break;
    }
    index++;
  }
}

bool HomeThingMenuScreen::select_menu(int index) {
  if (show_name_) {
    if (index == 0) {
      ESP_LOGI(TAG, "selected name %d", index);
      return false;
    }
    index -= 1;
  }
#ifdef SHOW_VERSION
  if (show_version_) {
    if (index == 0) {
      ESP_LOGI(TAG, "selected version %d", index);
      return false;
    }
    index -= 1;
  }
#endif
  auto entity = entities_[index];
  switch (std::get<0>(entity)) {
    case MenuItemTypeNone:
      return false;
    case MenuItemTypeTitle:
      return false;
    case MenuItemTypeSwitch: {
#ifdef USE_SWITCH
      ESP_LOGI(TAG, "selected switch %d", index);
      auto switchObject =
          static_cast<switch_::Switch*>(std::get<1>(entities_[index]));
      switchObject->toggle();
      return false;
#endif
      break;
    }
    case MenuItemTypeCover: {
#ifdef USE_COVER
      ESP_LOGI(TAG, "selected cover %d", index);
      auto coverObject =
          static_cast<cover::Cover*>(std::get<1>(entities_[index]));
      coverObject->make_call().set_command_toggle().perform();
      return true;
#endif
      break;
    }
    case MenuItemTypeTextSensor: {
      ESP_LOGI(TAG, "selected text sensor %d", index);
      return false;
    }
    case MenuItemTypeCommand: {
      ESP_LOGI(TAG, "selected command %d", index);
      auto command = static_cast<MenuCommand*>(std::get<1>(entities_[index]));
      command->on_command();
      return true;
    }
    case MenuItemTypeSensor: {
      ESP_LOGI(TAG, "selected sensor %d", index);
      return false;
    }
    case MenuItemTypeLight: {
      ESP_LOGI(TAG, "selected light %d", index);
#ifdef USE_LIGHT
      auto light =
          static_cast<light::LightState*>(std::get<1>(entities_[index]));
      light->toggle().perform();
#endif
      return true;
    }
    case MenuItemTypeNumber: {
#ifdef USE_NUMBER
      ESP_LOGI(TAG, "selected number %d", index);
      auto entity = &entities_[index];
      set_selected_entity(entity);
      return true;
#endif
      break;
    }
    case MenuItemTypeButton: {
#ifdef USE_BUTTON
      ESP_LOGI(TAG, "selected button %d", index);
      auto button = static_cast<button::Button*>(std::get<1>(entity));
      button->press();
#endif
      return false;
    }
  }
  return false;
}

bool HomeThingMenuScreen::select_menu_hold(int index) {
  if (index == 0) {
    ESP_LOGI(TAG, "select hold name %d", index);
    return false;
  }
  index -= 1;
#ifdef SHOW_VERSION
  if (show_version_) {
    if (index == 0) {
      ESP_LOGI(TAG, "select hold version %d", index);
      return false;
    }
    index -= 1;
  }
#endif
  return false;
}

const std::tuple<MenuItemType, EntityBase*>* HomeThingMenuScreen::get_menu_item(
    int index) {
  if (show_name_) {
    // name isnt an entity
    index -= 1;
  }
#ifdef SHOW_VERSION
  if (show_version_) {
    index -= 1;
  }
#endif
  return &entities_[index];
}

}  // namespace homething_menu_base
}  // namespace esphome
