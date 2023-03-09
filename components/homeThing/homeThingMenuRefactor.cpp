#include "homeThingMenuRefactor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_menu_base {
void HomeThingMenuRefactor::drawGroupedBar(int yPos, bool extend) {
  int xPos = display_state_->get_margin_size() * 2;
  int width = 8;
  int lineHeight = extend ? display_state_->get_medium_font()->get_baseline() +
                                display_state_->get_margin_size()
                          : (display_state_->get_medium_font()->get_baseline() +
                             display_state_->get_margin_size()) /
                                2;
  display_buffer_->line(
      xPos, yPos, xPos, yPos + lineHeight,
      text_helpers_->primaryTextColor(display_state_->get_dark_mode()));
  display_buffer_->line(
      xPos,
      yPos + (display_state_->get_medium_font()->get_baseline() +
              display_state_->get_margin_size()) /
                 2,
      xPos + width,
      yPos + (display_state_->get_medium_font()->get_baseline() +
              display_state_->get_margin_size()) /
                 2,
      text_helpers_->primaryTextColor(display_state_->get_dark_mode()));
}

void HomeThingMenuRefactor::drawLightSliderRGBBar(int xPos, int yPos,
                                                  int sliderHeight) {
  double rgbBarWidth = display_buffer_->get_width() -
                       2 * display_state_->get_slider_margin_size();
  display_buffer_->filled_rectangle(xPos - 1, yPos - 1, rgbBarWidth + 2,
                                    sliderHeight + 2,
                                    display_state_->color_gray_dark_2_);
  for (double i = 0; i < rgbBarWidth; i++) {
    double hue = (360.0f / rgbBarWidth) * i;
    Color dotColor = TextHelpers::hsvToRGB(hue, 1, 1);
    display_buffer_->line(i + display_state_->get_slider_margin_size(), yPos,
                          i + display_state_->get_slider_margin_size(),
                          yPos + sliderHeight, dotColor);
  }
}

void HomeThingMenuRefactor::drawLightSliderBar(
    int xPos, int yPos, int sliderHeight, SliderSelectionState sliderState,
    std::shared_ptr<MenuTitleSlider> slider, bool drawRGB) {
  if (drawRGB) {
    drawLightSliderRGBBar(xPos, yPos, sliderHeight);
    return;
  }
  display_buffer_->filled_rectangle(
      xPos, yPos,
      display_buffer_->get_width() -
          2 * display_state_->get_slider_margin_size(),
      sliderHeight, display_state_->color_gray_dark_2_);
  switch (sliderState) {
    case SliderSelectionStateActive:
      display_buffer_->filled_rectangle(xPos, yPos, slider->sliderValue,
                                        sliderHeight,
                                        display_state_->color_accent_primary_);
      break;
    case SliderSelectionStateHover:
      display_buffer_->filled_rectangle(xPos, yPos, slider->sliderValue,
                                        sliderHeight,
                                        display_state_->color_white_);
      break;
    case SliderSelectionStateNone:
      break;
  }
}

void HomeThingMenuRefactor::drawLightSliderCircle(
    int xPos, int yPos, int sliderHeight, SliderSelectionState sliderState,
    std::shared_ptr<MenuTitleSlider> slider, bool drawRGB) {
  int circleSize = 5;
  int dotPositionX =
      slider->sliderValue + display_state_->get_slider_margin_size();
  int dotPositionY = yPos + (sliderHeight / 2);
  switch (sliderState) {
    case SliderSelectionStateActive:
      if (drawRGB) {
        auto rgbColor = TextHelpers::hsvToRGB(slider->displayValue, 1, 1);
        display_buffer_->filled_circle(dotPositionX, dotPositionY, circleSize,
                                       rgbColor);
        display_buffer_->circle(dotPositionX, dotPositionY, circleSize + 1,
                                display_state_->color_gray_dark_2_);
      } else {
        display_buffer_->filled_circle(dotPositionX, dotPositionY,
                                       circleSize - 1,
                                       display_state_->color_accent_primary_);
        display_buffer_->circle(dotPositionX, dotPositionY, circleSize,
                                display_state_->color_white_);
      }
      break;
    case SliderSelectionStateHover:
      display_buffer_->filled_circle(dotPositionX, dotPositionY, circleSize,
                                     display_state_->color_white_);
      break;
    case SliderSelectionStateNone:
      display_buffer_->filled_circle(dotPositionX, dotPositionY, circleSize,
                                     display_state_->color_white_);
      break;
  }
}

void HomeThingMenuRefactor::drawLightSliderTitle(
    int xPos, int yPos, int sliderHeight, SliderSelectionState sliderState,
    std::shared_ptr<MenuTitleSlider> slider, bool drawRGB) {
  std::string sliderTitle = "";
  if (drawRGB) {
    sliderTitle = slider->get_name();
  } else if (slider->displayValue > 0) {
    sliderTitle = slider->get_name() + " - " + to_string(slider->displayValue) +
                  slider->sliderUnit;
  } else {
    sliderTitle = slider->get_name();
  }
  switch (sliderState) {
    case SliderSelectionStateActive:
      if (drawRGB) {
        auto rgbColor = TextHelpers::hsvToRGB(slider->displayValue, 1, 1);
        display_buffer_->printf(xPos, yPos + 1,
                                display_state_->get_medium_font(), rgbColor,
                                sliderTitle.c_str());
      } else {
        display_buffer_->printf(
            xPos, yPos + 1, display_state_->get_medium_font(),
            display_state_->color_accent_primary_, sliderTitle.c_str());
      }
      break;
    case SliderSelectionStateHover:
    case SliderSelectionStateNone:
      display_buffer_->printf(xPos, yPos + 1, display_state_->get_medium_font(),
                              display_state_->color_white_,
                              sliderTitle.c_str());
      break;
  }
}

void HomeThingMenuRefactor::drawLightSlider(
    int xPos, int yPos, SliderSelectionState sliderState,
    std::shared_ptr<MenuTitleSlider> slider, bool drawRGB) {
  int sliderHeight = 3;
  if (sliderState == SliderSelectionStateHover) {
    display_buffer_->filled_rectangle(
        0, yPos, display_buffer_->get_width(),
        (display_state_->get_margin_size() +
         display_state_->get_medium_font()->get_baseline()) *
            2,
        display_state_->color_accent_primary_);
  }
  int sliderOffset =
      static_cast<int>((display_state_->get_medium_font()->get_baseline() +
                        display_state_->get_margin_size()) /
                       2);
  int oneRow = display_state_->get_medium_font()->get_baseline() +
               display_state_->get_margin_size();
  drawLightSliderBar(xPos, yPos + sliderOffset + oneRow, sliderHeight,
                     sliderState, slider, drawRGB);
  drawLightSliderCircle(xPos, yPos + sliderOffset + oneRow, sliderHeight,
                        sliderState, slider, drawRGB);
  drawLightSliderTitle(xPos, yPos, sliderHeight, sliderState, slider, drawRGB);
}

void HomeThingMenuRefactor::drawSwitch(bool switchState, int yPos) {
  int circleSize = display_state_->get_small_font()->get_baseline() / 2;
  int xPos = display_state_->get_margin_size() + circleSize;
  int centerYPos = yPos + (display_state_->get_medium_font()->get_baseline() +
                           display_state_->get_margin_size()) /
                              2;
  display_buffer_->circle(
      xPos, centerYPos, circleSize,
      text_helpers_->primaryTextColor(display_state_->get_dark_mode()));
  if (switchState) {
    display_buffer_->filled_circle(
        xPos, centerYPos, circleSize - 2,
        text_helpers_->primaryTextColor(display_state_->get_dark_mode()));
  }
}

void HomeThingMenuRefactor::drawLightSwitch(bool switchState, int yPos,
                                            Color lightColor,
                                            bool rowSelected) {
  int xPos = display_state_->get_margin_size() - 2;
  if (rowSelected) {
    display_buffer_->filled_rectangle(
        xPos - 1, yPos, display_state_->get_icon_size() + 1,
        display_state_->get_medium_font()->get_baseline() +
            display_state_->get_margin_size(),
        display_state_->color_black_);
  }
  yPos += (display_state_->get_margin_size() / 4);
  if (switchState) {
    display_buffer_->printf(xPos, yPos,
                            display_state_->get_material_font_small(),
                            lightColor, "󰌵");
  } else {
    display_buffer_->printf(xPos, yPos,
                            display_state_->get_material_font_small(),
                            lightColor, "󰌶");
  }
}

void HomeThingMenuRefactor::drawArrow(int yPos, int menuTitlesCount,
                                      int maxItems) {
  int xPos = display_buffer_->get_width() - 8;
  if (menuTitlesCount > maxItems + 1) {
    xPos = xPos - display_state_->get_scroll_bar_width();
  }
  display_buffer_->line(
      xPos, yPos + 4, xPos + 3,
      yPos + (display_state_->get_medium_font()->get_baseline() +
              display_state_->get_margin_size()) /
                 2,
      text_helpers_->primaryTextColor(display_state_->get_dark_mode()));
  display_buffer_->line(
      xPos,
      yPos +
          (display_state_->get_medium_font()->get_baseline() +
           display_state_->get_margin_size()) -
          4,
      xPos + 3,
      yPos + (display_state_->get_medium_font()->get_baseline() +
              display_state_->get_margin_size()) /
                 2,
      text_helpers_->primaryTextColor(display_state_->get_dark_mode()));
}
}  // namespace homething_menu_base
}  // namespace esphome
