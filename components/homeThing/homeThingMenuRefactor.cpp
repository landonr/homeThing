#include "homeThingMenuRefactor.h"
#include "esphome/core/log.h"
namespace esphome {
namespace homething_menu_base {
void HomeThingMenuRefactor::drawGroupedBar(int yPos, bool extend) {
  int xPos = display_state_->get_margin_size() * 2;
  int width = 8;
  int lineHeight = extend ? display_state_->get_font_medium()->get_baseline() +
                                display_state_->get_margin_size()
                          : (display_state_->get_font_medium()->get_baseline() +
                             display_state_->get_margin_size()) /
                                2;
  display_->line(xPos, yPos, xPos, yPos + lineHeight,
                        display_state_->primaryTextColor());
  display_->line(
      xPos,
      yPos + (display_state_->get_font_medium()->get_baseline() +
              display_state_->get_margin_size()) /
                 2,
      xPos + width,
      yPos + (display_state_->get_font_medium()->get_baseline() +
              display_state_->get_margin_size()) /
                 2,
      display_state_->primaryTextColor());
}

void HomeThingMenuRefactor::drawLightSliderRGBBar(int xPos, int yPos,
                                                  int sliderHeight,
                                                  int slider_width) {
  display_->filled_rectangle(
      xPos - 1, yPos - 1, slider_width + 2, sliderHeight + 2,
      display_state_->get_color_palette()->get_gray_dark_2());
  for (double i = 0; i < slider_width; i++) {
    double hue = (360.0f / slider_width) * i;
    float red, green, blue = 0;
    hsv_to_rgb(hue, 1, 1, red, green, blue);
    auto rgbColor = Color(red * 255.0, green * 255.0, blue * 255.0);
    display_->line(i + display_state_->get_slider_margin_size(), yPos,
                          i + display_state_->get_slider_margin_size(),
                          yPos + sliderHeight, rgbColor);
  }
}

void HomeThingMenuRefactor::drawLightSliderBar(int xPos, int yPos,
                                               int sliderHeight,
                                               SliderSelectionState sliderState,
                                               const MenuTitleSlider* slider,
                                               bool drawRGB) {
  int slider_min = xPos + display_state_->get_slider_margin_size();
  int slider_width = display_->get_width() -
                     2 * display_state_->get_slider_margin_size();
  if (drawRGB) {
    drawLightSliderRGBBar(xPos, yPos, sliderHeight, slider_width);
    return;
  }
  display_->filled_rectangle(
      slider_min, yPos, slider_width, sliderHeight,
      display_state_->get_color_palette()->get_gray_dark_2());
  int slider_value_width = slider->percent_value() * slider_width;
  switch (sliderState) {
    case SliderSelectionStateActive:
      display_->filled_rectangle(
          slider_min, yPos, slider_value_width, sliderHeight,
          display_state_->get_color_palette()->get_accent_primary());
      break;
    case SliderSelectionStateHover:
      display_->filled_rectangle(
          slider_min, yPos, slider_value_width, sliderHeight,
          display_state_->get_color_palette()->get_white());
      break;
    case SliderSelectionStateNone:
      break;
  }
}

void HomeThingMenuRefactor::drawLightSliderCircle(
    int xPos, int yPos, int sliderHeight, SliderSelectionState sliderState,
    const MenuTitleSlider* slider, bool drawRGB) {
  int circleSize = 5;
  int slider_width = display_->get_width() -
                     2 * display_state_->get_slider_margin_size();
  int dotPositionX = display_state_->get_slider_margin_size() +
                     slider->percent_value() * slider_width;
  int dotPositionY = yPos + (sliderHeight / 2);
  switch (sliderState) {
    case SliderSelectionStateActive:
      if (drawRGB) {
        ESP_LOGI(TAG, "dotPositionX %d, percent_value %f", dotPositionX,
                 slider->percent_value());
        float red, green, blue = 0;
        hsv_to_rgb(slider->displayValue, 1, 1, red, green, blue);
        auto rgbColor = Color(red * 255.0, green * 255.0, blue * 255.0);
        display_->filled_circle(dotPositionX, dotPositionY, circleSize,
                                       rgbColor);
        display_->circle(
            dotPositionX, dotPositionY, circleSize + 1,
            display_state_->get_color_palette()->get_gray_dark_2());
      } else {
        display_->filled_circle(
            dotPositionX, dotPositionY, circleSize - 1,
            display_state_->get_color_palette()->get_accent_primary());
        display_->circle(
            dotPositionX, dotPositionY, circleSize,
            display_state_->get_color_palette()->get_white());
      }
      break;
    case SliderSelectionStateHover:
      display_->filled_circle(
          dotPositionX, dotPositionY, circleSize,
          display_state_->get_color_palette()->get_white());
      break;
    case SliderSelectionStateNone:
      display_->filled_circle(
          dotPositionX, dotPositionY, circleSize,
          display_state_->get_color_palette()->get_white());
      break;
  }
}

void HomeThingMenuRefactor::drawLightSliderTitle(
    int xPos, int yPos, int sliderHeight, SliderSelectionState sliderState,
    const MenuTitleSlider* slider, bool drawRGB) {
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
        float red, green, blue = 0;
        hsv_to_rgb(slider->displayValue, 1, 1, red, green, blue);
        auto rgbColor = Color(red * 255.0, green * 255.0, blue * 255.0);
        display_->printf(xPos, yPos + 1,
                                display_state_->get_font_medium(), rgbColor,
                                sliderTitle.c_str());
      } else {
        display_->printf(
            xPos, yPos + 1, display_state_->get_font_medium(),
            display_state_->get_color_palette()->get_accent_primary(),
            sliderTitle.c_str());
      }
      break;
    case SliderSelectionStateHover:
    case SliderSelectionStateNone:
      display_->printf(xPos, yPos + 1, display_state_->get_font_medium(),
                              display_state_->get_color_palette()->get_white(),
                              sliderTitle.c_str());
      break;
  }
}

void HomeThingMenuRefactor::drawLightSlider(int xPos, int yPos,
                                            SliderSelectionState sliderState,
                                            const MenuTitleSlider* slider,
                                            bool drawRGB) {
  int sliderHeight = 3;
  if (sliderState == SliderSelectionStateHover) {
    display_->filled_rectangle(
        0, yPos, display_->get_width(),
        (display_state_->get_margin_size() +
         display_state_->get_font_medium()->get_baseline()) *
            2,
        display_state_->get_color_palette()->get_accent_primary());
  }
  int sliderOffset =
      static_cast<int>((display_state_->get_font_medium()->get_baseline() +
                        display_state_->get_margin_size()) /
                       2);
  int oneRow = display_state_->get_font_medium()->get_baseline() +
               display_state_->get_margin_size();
  drawLightSliderBar(xPos, yPos + sliderOffset + oneRow, sliderHeight,
                     sliderState, slider, drawRGB);
  drawLightSliderCircle(xPos, yPos + sliderOffset + oneRow, sliderHeight,
                        sliderState, slider, drawRGB);
  drawLightSliderTitle(xPos, yPos, sliderHeight, sliderState, slider, drawRGB);
}

void HomeThingMenuRefactor::drawSwitch(bool switchState, int yPos) {
  int circleSize = display_state_->get_font_small()->get_baseline() / 2;
  int xPos = display_state_->get_margin_size() + circleSize;
  int centerYPos = yPos + (display_state_->get_font_medium()->get_baseline() +
                           display_state_->get_margin_size()) /
                              2;
  display_->circle(xPos, centerYPos, circleSize,
                          display_state_->primaryTextColor());
  if (switchState) {
    display_->filled_circle(xPos, centerYPos, circleSize - 2,
                                   display_state_->primaryTextColor());
  }
}

void HomeThingMenuRefactor::drawLightSwitch(bool switchState, int yPos,
                                            Color lightColor,
                                            bool rowSelected) {
  int xPos = display_state_->get_margin_size() - 2;
  if (rowSelected) {
    display_->filled_rectangle(
        xPos - 1, yPos, display_state_->get_icon_size() + 1,
        display_state_->get_font_medium()->get_baseline() +
            display_state_->get_margin_size(),
        display_state_->get_color_palette()->get_accent_primary());
  }
  yPos += (display_state_->get_margin_size() / 4);
  if (switchState) {
    display_->printf(xPos, yPos,
                            display_state_->get_font_material_small(),
                            lightColor, "󰌵");
  } else {
    display_->printf(xPos, yPos,
                            display_state_->get_font_material_small(),
                            lightColor, "󰌶");
  }
  bool drawLightOutline = !display_state_->get_dark_mode() && !rowSelected &&
                          lightColor == Color(255, 255, 255);
  if (drawLightOutline) {
    display_->printf(
        xPos, yPos, display_state_->get_font_material_small(),
        display_state_->get_color_palette()->get_black(), "󰌶");
  }
}

void HomeThingMenuRefactor::drawArrow(int yPos, int menuTitlesCount,
                                      int maxItems) {
  int xPos = display_->get_width() - 8;
  if (menuTitlesCount > maxItems + 1) {
    xPos = xPos - display_state_->get_scroll_bar_width();
  }
  display_->line(
      xPos, yPos + 4, xPos + 3,
      yPos + (display_state_->get_font_medium()->get_baseline() +
              display_state_->get_margin_size()) /
                 2,
      display_state_->secondaryTextColor());
  display_->line(
      xPos,
      yPos +
          (display_state_->get_font_medium()->get_baseline() +
           display_state_->get_margin_size()) -
          4,
      xPos + 3,
      yPos + (display_state_->get_font_medium()->get_baseline() +
              display_state_->get_margin_size()) /
                 2,
      display_state_->secondaryTextColor());
}
}  // namespace homething_menu_base
}  // namespace esphome
