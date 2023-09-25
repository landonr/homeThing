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

std::string HomeThingMenuTextHelpers::textWrap(std::string text,
                                               unsigned per_line) {
  unsigned line_begin = 0;
  while (line_begin < text.size()) {
    const unsigned ideal_end = line_begin + per_line;
    unsigned line_end = ideal_end <= text.size() ? ideal_end : text.size() - 1;

    if (line_end == text.size() - 1) {
      ++line_end;
    } else if (std::isspace(text[line_end])) {
      text[line_end] = '\n';
      ++line_end;
    } else {
      // backtrack
      unsigned end = line_end;
      while (end > line_begin && !std::isspace(text[end]))
        --end;

      if (end != line_begin) {
        line_end = end;
        text[line_end++] = '\n';
      } else {
        text.insert(line_end++, 1, '\n');
      }
    }

    line_begin = line_end;
  }

  return text;
}

int HomeThingMenuTextHelpers::drawTextWrapped(
    int xPos, int yPos, font::Font* font, Color color,
    display::TextAlign alignment, std::string text, int maxLines,
    display::DisplayBuffer* display_buffer, float widthRatio) {
  int fontSize = font->get_baseline();
  auto wrappedTitles = new std::vector<std::string>();
  auto wrappedString = textWrap(
      text, getCharacterLimit(xPos, fontSize, alignment,
                              display_buffer->get_width(), widthRatio));
  tokenize(wrappedString, "\n", wrappedTitles);
  int characterLimit = getCharacterLimit(
      xPos, fontSize, alignment, display_buffer->get_width(), widthRatio);
  int max = maxLines != 0 && maxLines < wrappedTitles->size()
                ? maxLines
                : wrappedTitles->size();
  for (int i = 0; i < max; i++) {
    if (maxLines != 0 && maxLines - 1 == i && i == max - 1) {
      std::string title = (*wrappedTitles)[i];
      if (wrappedTitles[i].size() > characterLimit - 2) {
        title.erase(title.length() - 3);
      }
      title = title + "...";
      display_buffer->printf(xPos, yPos + (i * fontSize), font, color,
                             alignment, title.c_str());
      break;
    } else {
      display_buffer->printf(xPos, yPos + (i * fontSize), font, color,
                             alignment, (*wrappedTitles)[i].c_str());
    }
  }
  return yPos + (max * fontSize);
}
}  // namespace homething_display_state
}  // namespace esphome
