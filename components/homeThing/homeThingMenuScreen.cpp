#include "homeThingMenuScreen.h"

namespace esphome {
namespace homething_menu_base {

std::vector<std::shared_ptr<MenuTitleBase>> HomeThingMenuScreen::menu_titles() {
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
        auto switchObject = static_cast<switch_::Switch*>(std::get<1>(entity));
        ESP_LOGD(TAG, "switch state %d", switchObject->state);
        MenuTitleLeftIcon state =
            switchObject->state ? OnMenuTitleLeftIcon : OffMenuTitleLeftIcon;
        out.push_back(std::make_shared<MenuTitleToggle>(
            switchObject->get_name(), switchObject->get_object_id(), state,
            NoMenuTitleRightIcon));
#endif
        break;
      }
      case MenuItemTypeCover: {
#ifdef USE_COVER
        auto coverObject = static_cast<cover::Cover*>(std::get<1>(entity));
        ESP_LOGD(TAG, "cover state %d", coverObject->is_fully_closed());
        MenuTitleLeftIcon state = coverObject->is_fully_closed()
                                      ? OffMenuTitleLeftIcon
                                      : OnMenuTitleLeftIcon;
        out.push_back(std::make_shared<MenuTitleToggle>(
            coverObject->get_name(), coverObject->get_object_id(), state,
            NoMenuTitleRightIcon));
#endif
        break;
      }
      case MenuItemTypeTextSensor: {
#ifdef USE_TEXT_SENSOR
        auto textSensor =
            static_cast<text_sensor::TextSensor*>(std::get<1>(entity));
        ESP_LOGD(TAG, "text sensor state %s", textSensor->state.c_str());
        if (textSensor->get_name() != "") {
          out.push_back(std::make_shared<MenuTitleBase>(
              textSensor->get_name() + " " + textSensor->get_state(), "",
              NoMenuTitleRightIcon));
        } else {
          out.push_back(std::make_shared<MenuTitleBase>(textSensor->state, "",
                                                        NoMenuTitleRightIcon));
        }
#endif
        break;
      }
      case MenuItemTypeCommand: {
        auto command = static_cast<MenuCommand*>(std::get<1>(entity));
        if (command->get_name() != "") {
          out.push_back(std::make_shared<MenuTitleBase>(command->get_name(), "",
                                                        NoMenuTitleRightIcon));
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

bool HomeThingMenuScreen::select_menu(int index) {
  if (index == 0) {
    ESP_LOGI(TAG, "selected name %d", index);
    return false;
  }
  index -= 1;
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
    case MenuItemTypeTitle:
      return false;
    case MenuItemTypeSwitch: {
#ifdef USE_SWITCH
      ESP_LOGI(TAG, "selected switch %d", index);
      auto switchObject =
          static_cast<switch_::Switch*>(std::get<1>(entities_[index]));
      switchObject->toggle();
      return true;
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
      ESP_LOGI(TAG, "selected number %d", index);
      auto entity = &entities_[index];
      set_selected_entity(entity);
      return true;
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
  // name isnt an entity
  index -= 1;
#ifdef SHOW_VERSION
  if (show_version_) {
    index -= 1;
  }
#endif
  return &entities_[index];
}

}  // namespace homething_menu_base
}  // namespace esphome
