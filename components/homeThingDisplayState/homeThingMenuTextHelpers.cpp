#include "homeThingMenuTextHelpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_display_state {

Color HomeThingMenuTextHelpers::primaryTextColor(bool dark_mode,
                                                 Color lightColor,
                                                 Color darkColor) {
  if (dark_mode) {
    return lightColor;
  } else {
    return darkColor;
  }
}

Color HomeThingMenuTextHelpers::secondaryTextColor(bool dark_mode,
                                                   Color lightColor,
                                                   Color darkColor) {
  if (dark_mode) {
    return lightColor;
  } else {
    return darkColor;
  }
}

int HomeThingMenuTextHelpers::getCharacterLimit(int xPos, int fontSize,
                                                display::TextAlign alignment,
                                                int displayWidth,
                                                float widthRatio) {
  int characterLimit = (displayWidth - xPos) / (fontSize * widthRatio) - 1;
  if (xPos == displayWidth / 2 && alignment == display::TextAlign::TOP_CENTER) {
    characterLimit = displayWidth / (fontSize * widthRatio) - 1;
  }
  return characterLimit;
}

int HomeThingMenuTextHelpers::getTextWidth(int fontSize, int characterCount,
                                           float widthRatio) {
  return (fontSize * widthRatio * characterCount);
}

int HomeThingMenuTextHelpers::drawTextWrapped(
    int xPos, int yPos, font::Font* font, Color color,
    display::TextAlign alignment, std::string text, int maxLines,
    display::DisplayBuffer* display_buffer, float widthRatio) {
  int fontSize = font->get_baseline();
  unsigned line_begin = 0;
  const unsigned per_line = getCharacterLimit(
      xPos, fontSize, alignment, display_buffer->get_width(), widthRatio);
  ESP_LOGD(TAG, "drawTextWrapped: per_line1 %d text %s", per_line,
           text.c_str());
  while (line_begin < text.size()) {
    const unsigned ideal_end = line_begin + per_line;
    unsigned line_end = ideal_end <= text.size() ? ideal_end : text.size();
    std::string line = text.substr(line_begin, line_end - line_begin);
    ESP_LOGD(TAG,
             "Printing: xPos %d, yPos %d, font %p, color, alignment, "
             "text.c_str() + %u",
             xPos, yPos, font, line_begin);
    // Display the line using display_buffer->printf() with the substring.
    display_buffer->printf(xPos, yPos, font, color, alignment, line.c_str());

    yPos = yPos + fontSize;  // Increment yPos.
    line_begin = line_end;
  }
  return yPos;
}
}  // namespace homething_display_state
}  // namespace esphome
