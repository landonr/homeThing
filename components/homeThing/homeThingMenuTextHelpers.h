
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
  int drawTextWrapped(int xPos, int yPos, display::Font* font, Color color,
                      display::TextAlign alignment, std::string text,
                      int maxLines);

 private:
  display::DisplayBuffer* display_buffer_{nullptr};
  HomeThingMenuDisplayState* display_state_{nullptr};
  const char* const TAG = "homething.menu.text_helpers";

  void tokenize(std::string const& str, std::string delim,
                std::vector<std::string>* out) {
    size_t start;
    size_t end = 0;

    while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
      end = str.find(delim, start);
      out->push_back(str.substr(start, end - start));
    }
  }
};
}  // namespace homething_menu_base
}  // namespace esphome
