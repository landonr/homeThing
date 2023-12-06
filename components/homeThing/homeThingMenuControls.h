#pragma once
#include <tuple>
#ifdef USE_LIGHT
#include "esphome/components/homeThing/homeThingLightHelpers.h"
#endif

#include "esphome/components/homeThing/homeThingMenuScreen.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"

namespace esphome {
namespace homething_menu_base {

static const char* const MENU_CONTROLS_TAG = "homething.menu.controls";

enum MenuControlCommand : uint8_t {
  MENU_COMMAND_UP = 0,
  MENU_COMMAND_DOWN = 1,
  MENU_COMMAND_LEFT = 2,
  MENU_COMMAND_RIGHT = 3,
  MENU_COMMAND_SELECT = 4,
  MENU_COMMAND_SCROLL_CLOCKWISE = 5,
  MENU_COMMAND_SCROLL_COUNTER_CLOCKWISE = 6,
  MENU_COMMAND_BACK = 7,
  MENU_COMMAND_OPTION = 8,
  MENU_COMMAND_HOME = 9,
};

class HomeThingMenuControls {
 public:
  static bool editingScrollBack(
      const std::tuple<MenuItemType, EntityBase*>* entity, int menuIndex,
      bool editing_menu_item) {
    MenuItemType menu_item_type = std::get<0>(*entity);
    switch (menu_item_type) {
      case MenuItemTypeLight: {
#ifdef USE_LIGHT
        auto light = static_cast<light::LightState*>(std::get<1>(*entity));
        int indexOffset = supportsColorTemperature(light) ? 2 : 1;
        if (editing_menu_item && menuIndex == 0 && entity != NULL) {
          HomeThingLightHelpers::decBrightness(light);
          return true;
        } else if (editing_menu_item && menuIndex == 1 && entity != NULL &&
                   supportsColorTemperature(light)) {
          HomeThingLightHelpers::decTemperature(light);
          return true;
        } else if (editing_menu_item && menuIndex == indexOffset &&
                   entity != NULL) {
          HomeThingLightHelpers::decColor(light);
          return true;
        }
#endif
        break;
      }
      case MenuItemTypeNumber: {
#ifdef USE_NUMBER
        if (editing_menu_item) {
          auto number = static_cast<number::Number*>(std::get<1>(*entity));
          number->make_call().number_decrement(false).perform();
          return true;
        }
#endif
        break;
      }
      default:
        break;
    }
    return false;
  }

  static bool editingScrollForward(
      const std::tuple<MenuItemType, EntityBase*>* entity, int menuIndex,
      bool editing_menu_item) {
    MenuItemType menu_item_type = std::get<0>(*entity);
    switch (menu_item_type) {
      case MenuItemTypeLight: {
#ifdef USE_LIGHT
        auto light = static_cast<light::LightState*>(std::get<1>(*entity));
        int indexOffset = supportsColorTemperature(light) ? 2 : 1;
        if (editing_menu_item && menuIndex == 0 && entity != NULL) {
          HomeThingLightHelpers::incBrightness(light);
          return true;
        } else if (editing_menu_item && menuIndex == 1 && entity != NULL &&
                   supportsColorTemperature(light)) {
          HomeThingLightHelpers::incTemperature(light);
          return true;
        } else if (editing_menu_item && menuIndex == indexOffset &&
                   entity != NULL) {
          HomeThingLightHelpers::incColor(light);
          return true;
        }
#endif
        break;
      }
      case MenuItemTypeNumber: {
#ifdef USE_NUMBER
        if (editing_menu_item) {
          auto number = static_cast<number::Number*>(std::get<1>(*entity));
          number->make_call().number_increment(false).perform();
          return true;
        }
#endif
        break;
      }
      default:
        break;
    }
    return false;
  }

  static bool selectLightDetail(
      const std::tuple<MenuItemType, EntityBase*>* entity, int menuIndex,
      bool editing_menu_item) {
    MenuItemType menu_item_type = std::get<0>(*entity);
    switch (menu_item_type) {
      case MenuItemTypeLight: {
#ifdef USE_LIGHT
        auto light = static_cast<light::LightState*>(std::get<1>(*entity));
        if (light->supports_effects()) {
          int index =
              1 + supportsColorTemperature(light) + supportsColor(light);
          if (menuIndex > index) {
            auto effect = light->get_effects()[menuIndex - index - 1];
            light->turn_on().set_effect(effect->get_name()).perform();
            return true;
          }
        }
#endif
        return false;
      }
      default:
        break;
    }
    return false;
  }
};
}  // namespace homething_menu_base
}  // namespace esphome
