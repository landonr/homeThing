#pragma once

#include <string>
#include "esphome/components/homeThing/homeThingMenuTitle.h"

namespace esphome {
namespace homething_menu_base {

class MenuTitleSelect : public MenuTitleBase {
 public:
  MenuTitleSelect(std::string new_name, std::string newEntityId,
                  MenuTitleRightIcon newRightIconState)
      : MenuTitleBase{new_name, newEntityId, newRightIconState,
                      SelectMenuTitleType, 1} {}
  bool indentLine() const;
};

}  // namespace homething_menu_base
}  // namespace esphome
