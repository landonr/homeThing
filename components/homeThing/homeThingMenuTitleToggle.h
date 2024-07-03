#pragma once

#include <string>
#include "esphome/components/homeThing/homeThingMenuTitle.h"

namespace esphome {
namespace homething_menu_base {

class MenuTitleToggle : public MenuTitleBase {
 public:
  MenuTitleLeftIcon leftIconState;
  std::string value;
  MenuTitleToggle(std::string newName, std::string newEntityId,
                  std::string newValue, MenuTitleLeftIcon newLeftIconState,
                  MenuTitleRightIcon newRightIconState,
                  MenuTitleType newTitleType = ToggleMenuTitleType)
      : MenuTitleBase{newName, newEntityId, newRightIconState, newTitleType, 1},
        leftIconState(newLeftIconState),
        value(newValue) {}

  MenuTitleToggle(std::string newName, std::string newEntityId,
                  MenuTitleLeftIcon newLeftIconState,
                  MenuTitleRightIcon newRightIconState,
                  MenuTitleType newTitleType = ToggleMenuTitleType)
      : MenuTitleBase{newName, newEntityId, newRightIconState, newTitleType, 1},
        leftIconState(newLeftIconState),
        value("") {}
  bool indentLine() const;
};
}  // namespace homething_menu_base
}  // namespace esphome
