#pragma once

#include "esphome/components/homeThing/homeThingLightHelpers.h"
#include "esphome/components/homeThing/homeThingMenuScreen.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"

namespace esphome {
namespace homething_menu_base {

static const char* const MENU_CONTROLS_TAG = "homething.menu.controls";

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
        if (editing_menu_item && menuIndex == 0 && entity != NULL) {
          HomeThingLightHelpers::decBrightness(light);
          return true;
        } else if (editing_menu_item && menuIndex == 1 && entity != NULL) {
          HomeThingLightHelpers::decTemperature(light);
          return true;
        } else if (editing_menu_item && menuIndex == 2 && entity != NULL) {
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
        if (editing_menu_item && menuIndex == 0 && entity != NULL) {
          HomeThingLightHelpers::incBrightness(light);
          return true;
        } else if (editing_menu_item && menuIndex == 1 && entity != NULL) {
          HomeThingLightHelpers::incTemperature(light);
          return true;
        } else if (editing_menu_item && menuIndex == 2 && entity != NULL) {
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
};
}  // namespace homething_menu_base
}  // namespace esphome
