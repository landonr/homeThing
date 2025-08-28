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

void HomeThingMenuTextHelpers::drawTextMarquee(
    int xPos, int yPos, font::Font* font, Color color,
    display::TextAlign alignment, std::string text, int animationTick,
    display::Display* display, float widthRatio) {
  // Adjust the number of characters to show in the marquee
  const int marqueeLength =
      getCharacterLimit(xPos, font->get_baseline(), alignment,
                        display->get_width(), widthRatio);
  ;
  if (text.length() < marqueeLength) {
    display->printf(xPos, yPos, font, color, alignment, "%s",
                           text.c_str());
    return;
  }
  std::string spaceTitle = "   ";
  int marqueePositionMaxed = 0;
  int textLength = text.length();
  bool animate =
      animationTick > 0 && animationTick < textLength + spaceTitle.length();

  if (!text.empty() && textLength > marqueeLength && animate) {
    const int visibleLength = textLength - marqueeLength;
    // Calculate the position for the marquee effect
    int tick = animationTick %
               textLength;  // +1 to ensure at least one character is shown
    marqueePositionMaxed = tick;
  }

  std::string title1 = "";
  std::string marqueeTitle, title2;
  if (marqueePositionMaxed < textLength && animationTick < textLength) {
    title1 = text.substr(marqueePositionMaxed, marqueeLength);
    if (title1.length() < marqueeLength) {
      title1 += spaceTitle.substr(0, marqueeLength - (title1.length()));
    }
  } else {
    title1 = spaceTitle.substr(
        0, (textLength + spaceTitle.length()) - animationTick);
  }
  if (animate) {
    // std::string title1 = text.erase(0, marqueePositionMaxed);
    marqueeTitle = title1;
    if (marqueeTitle.length() < marqueeLength) {
      title2 = text.substr(0, marqueeLength - (title1.length()));
      marqueeTitle += title2;
    }

    ESP_LOGD(TAG,
             "drawTextMarquee: xPos %d, yPos %d, marqueePositionMaxed %d, tick "
             "%d, animate %d, text1 %s, text2 %s, marqueeTitle %s",
             xPos, yPos, marqueePositionMaxed, animationTick, animate,
             title1.c_str(), title2.c_str(), marqueeTitle.c_str());
  } else {
    marqueeTitle = text.substr(0, marqueeLength);
  }

  if (!marqueeTitle.empty()) {
    display->printf(xPos, yPos, font, color, alignment, "%s",
                           marqueeTitle.c_str());
  }
}

int HomeThingMenuTextHelpers::drawTextWrapped(
    int xPos, int yPos, font::Font* font, Color color,
    display::TextAlign alignment, std::string text, int maxLines,
    display::Display* display, float widthRatio) {
  int fontSize = font->get_baseline();
  unsigned line_begin = 0;
  const unsigned per_line = getCharacterLimit(
      xPos, fontSize, alignment, display->get_width(), widthRatio);
  ESP_LOGD(TAG, "drawTextWrapped: per_line1 %d text %s", per_line,
           text.c_str());
  unsigned linesDrawn = 0;
  if (text.size() <= per_line) {
    maxLines = 1;
  }
  while (line_begin < text.size() && (maxLines <= 0 || linesDrawn < maxLines)) {
    unsigned line_end = line_begin + per_line;
    if (line_end >= text.size()) {
      line_end = text.size();
    } else {
      // Check if the current character is not a space; if so, move the end backwards
      while (line_end > line_begin && text[line_end] != ' ' &&
             text[line_end] != '\n') {
        --line_end;
      }
      // If no space found within the limit, move the end forwards until a space is found
      while (line_end < text.size() && text[line_end] != ' ' &&
             text[line_end] != '\n' && line_end < line_begin + per_line) {
        ++line_end;
      }
    }

    std::string line = text.substr(line_begin, line_end - line_begin);
    ESP_LOGD(TAG,
             "Printing: xPos %d, yPos %d, font %p, color, alignment, "
             "%s + %u",
             xPos, yPos, font, text.c_str(), linesDrawn);
    display->printf(xPos, yPos, font, color, alignment, line.c_str());

    yPos = yPos + fontSize;  // Increment yPos.
    ++linesDrawn;

    if (line_end < text.size() &&
        (text[line_end] == ' ' || text[line_end] == '\n')) {
      // Move past the space or newline character if present.
      line_begin = line_end + 1;
    } else {
      line_begin = line_end;
    }
  }
  return yPos;
}
}  // namespace homething_display_state
}  // namespace esphome
