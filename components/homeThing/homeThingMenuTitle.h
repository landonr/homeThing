#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "esphome/core/color.h"

#ifdef USE_MEDIA_PLAYER_GROUP
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"
#endif

#include "esphome/core/log.h"
#include "version.h"

namespace esphome {
namespace homething_menu_base {

static const char* const MENU_TITLE_TAG = "homething.menutitle";

enum MenuStates {
  bootMenu,
  rootMenu,
  appMenu,
  lightsDetailMenu,
  customMenu,
  entityMenu
};

enum MenuTitleLeftIcon {
  NoMenuTitleLeftIcon,
  OffMenuTitleLeftIcon,
  OnMenuTitleLeftIcon,
  GroupedMenuTitleLeftIcon
};

enum MenuTitleRightIcon { NoMenuTitleRightIcon, ArrowMenuTitleRightIcon };

enum MenuTitleType {
  BaseMenuTitleType,
  PlayerMenuTitleType,
  SliderMenuTitleType,
  ToggleMenuTitleType,
  LightMenuTitleType,
  SourceMenuTitleType
};

enum SliderSelectionState {
  SliderSelectionStateNone,
  SliderSelectionStateActive,
  SliderSelectionStateHover
};

class MenuTitleBase {
 protected:
  std::string name_;

 public:
  std::string entity_id_;
  MenuTitleRightIcon rightIconState;
  MenuTitleType titleType;
  int rowHeight;
  MenuTitleBase(std::string new_name, std::string newEntityId,
                MenuTitleRightIcon newRightIconState,
                MenuTitleType newTitleType = BaseMenuTitleType,
                int newRowHeight = 1)
      : name_(new_name),
        entity_id_(newEntityId),
        rightIconState(newRightIconState),
        titleType(newTitleType),
        rowHeight(newRowHeight) {}

  std::string get_name() const {
    if (name_ != "") {
      return name_;
    } else {
      return entity_id_;
    }
  }

  static MenuTitleRightIcon menu_state_right_icon(MenuStates menu_state) {
    switch (menu_state) {
      case appMenu:
        return ArrowMenuTitleRightIcon;
      case lightsDetailMenu:
        return ArrowMenuTitleRightIcon;
      case rootMenu:
        return NoMenuTitleRightIcon;
      case bootMenu:
        return NoMenuTitleRightIcon;
      case customMenu:
        return ArrowMenuTitleRightIcon;
      case entityMenu:
        return NoMenuTitleRightIcon;
    }
    return NoMenuTitleRightIcon;
  }

  static std::string menu_state_title(MenuStates menu_state) {
    switch (menu_state) {
      case appMenu:
        return "App";
      case lightsDetailMenu:
        return "Light Detail";
      case rootMenu:
        return "Home";
      case bootMenu:
        return "Boot";
      case customMenu:
        return "Settings";
      case entityMenu:
        return "Entity";
    }
    return "";
  }
};

}  // namespace homething_menu_base
}  // namespace esphome
