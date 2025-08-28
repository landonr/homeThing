#pragma once

#include <string>
#include <vector>
#include "esphome/components/display/display.h"
#include "esphome/components/font/font.h"
#include "esphome/core/color.h"

namespace esphome {
namespace homething_display_state {

class HomeThingMenuTextHelpers {
 public:
  Color primaryTextColor(bool dark_mode, Color lightColor, Color darkColor);
  Color secondaryTextColor(bool dark_mode, Color lightColor, Color darkColor);
  int getCharacterLimit(int xPos, int fontSize, display::TextAlign alignment,
                        int displayWidth, float widthRatio);
  int getTextWidth(int fontSize, int characterCount, float widthRatio);
  int drawTextWrapped(int xPos, int yPos, font::Font* font, Color color,
                      display::TextAlign alignment, std::string text,
                      int maxLines, display::Display* display,
                      float widthRatio);
  void drawTextMarquee(int xPos, int yPos, font::Font* font, Color color,
                       display::TextAlign alignment, std::string text,
                       int animationTick, display::Display* display,
                       float widthRatio);

 private:
  const char* const TAG = "homething.menu.text_helpers";

  std::string textWrap(std::string text, unsigned per_line);
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

}  // namespace homething_display_state
}  // namespace esphome
