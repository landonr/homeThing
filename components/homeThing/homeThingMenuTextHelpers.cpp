#include "homeThingMenuTextHelpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_menu_base {
Color HomeThingMenuTextHelpers::primaryTextColor(bool dark_mode) {
  if (dark_mode) {
    return display_state_->get_color_palette()->get_white();
  } else {
    return display_state_->get_color_palette()->get_black();
  }
}

Color HomeThingMenuTextHelpers::secondaryTextColor(bool dark_mode) {
  if (dark_mode) {
    return display_state_->get_color_palette()->get_white();
  } else {
    return display_state_->get_color_palette()->get_white();
  }
}

int HomeThingMenuTextHelpers::getCharacterLimit(int xPos, int fontSize,
                                                display::TextAlign alignment) {
  int characterLimit =
      (display_buffer_->get_width() - xPos) /
          (fontSize * display_state_->get_font_size_width_ratio()) -
      1;
  if (xPos == display_buffer_->get_width() / 2 &&
      alignment == display::TextAlign::TOP_CENTER) {
    characterLimit =
        display_buffer_->get_width() /
            (fontSize * display_state_->get_font_size_width_ratio()) -
        1;
  }
  return characterLimit;
}

int HomeThingMenuTextHelpers::getTextWidth(int fontSize, int characterCount) {
  return (fontSize * display_state_->get_font_size_width_ratio() *
          characterCount);
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

int HomeThingMenuTextHelpers::drawTextWrapped(int xPos, int yPos,
                                              font::Font* font, Color color,
                                              display::TextAlign alignment,
                                              std::string text, int maxLines) {
  int fontSize = font->get_baseline();
  auto wrappedTitles = new std::vector<std::string>();
  auto wrappedString =
      textWrap(text, getCharacterLimit(xPos, fontSize, alignment));
  tokenize(wrappedString, "\n", wrappedTitles);
  int characterLimit = getCharacterLimit(xPos, fontSize, alignment);
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
      display_buffer_->printf(xPos, yPos + (i * fontSize), font, color,
                              alignment, title.c_str());
      break;
    } else {
      display_buffer_->printf(xPos, yPos + (i * fontSize), font, color,
                              alignment, (*wrappedTitles)[i].c_str());
    }
  }
  return yPos + (max * fontSize);
}
}  // namespace homething_menu_base
}  // namespace esphome
