#include "homeThingMenuTitleToggle.h"

namespace esphome {
namespace homething_menu_base {

bool MenuTitleToggle::indentLine() const {
  switch (leftIconState) {
    case OffMenuTitleLeftIcon:
    case OnMenuTitleLeftIcon:
    case GroupedMenuTitleLeftIcon:
      return true;
    case NoMenuTitleLeftIcon:
      return false;
  }
  return false;
}

}  // namespace homething_menu_base
}  // namespace esphome
