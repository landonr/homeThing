#pragma once

#include <string>
#include "esphome/components/homeThing/homeThingMenuTitle.h"

namespace esphome {
namespace homething_menu_base {

class MenuTitleToggle : public MenuTitleBase {
 public:
  MenuTitleLeftIcon leftIconState;

  MenuTitleToggle(std::string new_name, std::string newEntityId,
                  MenuTitleLeftIcon newLeftIconState,
                  MenuTitleRightIcon newRightIconState,
                  MenuTitleType newTitleType = ToggleMenuTitleType)
      : MenuTitleBase{new_name, newEntityId, newRightIconState, newTitleType,
                      1},
        leftIconState(newLeftIconState) {}
  bool indentLine() const;
};
}  // namespace homething_menu_base
}  // namespace esphome
