#pragma once

#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/components/homeassistant_component/LightExtensions.h"

namespace esphome {
namespace homething_menu_base {

static const char* const MENU_TITLE_LIGHT_TAG = "homething.menutitle.light";

#define MAX_BRIGHTNESS 255.0f

class MenuTitleLight : public MenuTitleToggle {

 public:
  Color lightColor;

  MenuTitleLight(std::string new_name, std::string newEntityId,
                 MenuTitleLeftIcon newLeftIconState,
                 MenuTitleRightIcon newRightIconState, Color newLightColor)
      : MenuTitleToggle{new_name, newEntityId, newLeftIconState,
                        newRightIconState, LightMenuTitleType},
        lightColor(newLightColor) {}
};

void lightTitleSwitches(const std::vector<light::LightState*>& lights,
                        std::vector<MenuTitleBase*>* menu_titles);

void lightTitleItems(light::LightState* light,
                     std::vector<MenuTitleBase*>* menu_titles);

}  // namespace homething_menu_base
}  // namespace esphome
