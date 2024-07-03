#pragma once

#include "homeThingMenuTitle.h"

#include <string>

namespace esphome {
namespace homething_menu_base {
class MenuTitleValue : public MenuTitleBase {
 public:
  std::string value;
  MenuTitleValue(std::string newTitle, std::string newEntityId,
                 MenuTitleRightIcon newRightIconState, std::string newValue)
      : MenuTitleBase{newTitle, newEntityId, newRightIconState,
                      ValueMenuTitleType, 1},
        value(newValue) {}
};
}  // namespace homething_menu_base
}  // namespace esphome
