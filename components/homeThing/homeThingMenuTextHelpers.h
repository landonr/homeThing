
#pragma once

#include <string>
#include <vector>
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/homeThing/homeThingMenuDisplayState.h"
#include "esphome/core/color.h"

namespace esphome {
namespace homething_menu_base {

class HomeThingMenuTextHelpers {
 public:
  // HomeThingMenuTextHelpers(display::DisplayBuffer* new_display_buffer,
  //                          HomeThingMenuDisplayState* new_display_state)
  //     : display_buffer_(new_display_buffer),
  //       display_state_(new_display_state) {}
  Color primaryTextColor(bool dark_mode);
  Color secondaryTextColor(bool dark_mode);
  int getCharacterLimit(int xPos, int fontSize, display::TextAlign alignment);
  int getTextWidth(int fontSize, int characterCount);
  std::string textWrap(std::string text, unsigned per_line);
  void set_display_state(HomeThingMenuDisplayState* new_display_state) {
    display_state_ = new_display_state;
  }
  void set_display_buffer(display::DisplayBuffer* new_display_buffer) {
    display_buffer_ = new_display_buffer;
  }

 private:
  display::DisplayBuffer* display_buffer_{nullptr};
  HomeThingMenuDisplayState* display_state_{nullptr};
};
}  // namespace homething_menu_base
}  // namespace esphome
