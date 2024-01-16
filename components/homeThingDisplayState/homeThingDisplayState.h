#pragma once

#include <string>
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/font/font.h"
#include "esphome/components/homeThingDisplayState/homeThingColorPalette.h"
#ifdef USE_IMAGE
#include "esphome/components/image/image.h"
#endif
#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif
#include "homeThingMenuTextHelpers.h"

namespace esphome {
namespace homething_display_state {

enum DisplayIconEnabledState { OFF = 1, ON = 2, ALWAYS = 3 };

class HomeThingDisplayState {
 public:
  font::Font* get_font_small() { return font_small_; }
  void set_font_small(font::Font* font_small) { font_small_ = font_small; }

  font::Font* get_font_medium() { return font_medium_; }
  void set_font_medium(font::Font* font_medium) { font_medium_ = font_medium; }

  font::Font* get_font_large() { return font_large_; }
  void set_font_large(font::Font* font_large) { font_large_ = font_large; }

  font::Font* get_font_large_heavy() { return font_large_heavy_; }
  void set_font_large_heavy(font::Font* font_large_heavy) {
    font_large_heavy_ = font_large_heavy;
  }

  font::Font* get_font_material_large() { return font_material_large_; }
  void set_font_material_large(font::Font* font_material_large) {
    font_material_large_ = font_material_large;
  }

  font::Font* get_font_material_small() { return font_material_small_; }
  void set_font_material_small(font::Font* font_material_small) {
    font_material_small_ = font_material_small;
  }

  int get_header_height() { return header_height_; }
  void set_header_height(int header_height) { header_height_ = header_height; }
  int get_margin_size() { return margin_size_; }
  void set_margin_size(int margin_size) { margin_size_ = margin_size; }
  int get_bottom_bar_margin() { return bottom_bar_margin_; }
  void set_bottom_bar_margin(int bottom_bar_margin) {
    bottom_bar_margin_ = bottom_bar_margin;
  }
  int get_slider_margin_size() { return slider_margin_size_; }
  void set_slider_margin_size(int slider_margin_size) {
    slider_margin_size_ = slider_margin_size;
  }
  int get_icon_size() { return icon_size_; }
  void set_icon_size(int icon_size) { icon_size_ = icon_size; }
  int get_scroll_bar_width() { return scroll_bar_width_; }
  void set_scroll_bar_width(int scroll_bar_width) {
    scroll_bar_width_ = scroll_bar_width;
  }
  float get_font_size_width_ratio() { return font_size_width_ratio_; }
  void set_font_size_width_ratio(float font_size_width_ratio) {
    font_size_width_ratio_ = font_size_width_ratio;
  }
  DisplayIconEnabledState get_draw_shuffle() { return draw_shuffle_; }
  void set_draw_shuffle(DisplayIconEnabledState draw_shuffle) {
    draw_shuffle_ = draw_shuffle;
  }
  DisplayIconEnabledState get_draw_repeat() { return draw_repeat_; }
  void set_draw_repeat(DisplayIconEnabledState draw_repeat) {
    draw_repeat_ = draw_repeat;
  }
  bool get_draw_header_time() { return draw_header_time_; }
  void set_draw_header_time(bool draw_header_time) {
    draw_header_time_ = draw_header_time;
  }
  bool get_draw_battery_level() { return draw_battery_level_; }
  void set_draw_battery_level(bool draw_battery_level) {
    draw_battery_level_ = draw_battery_level;
  }
  bool get_dark_mode() {
#ifdef USE_SWITCH
    if (dark_mode_switch_ != nullptr) {
      return dark_mode_switch_->state;
    }
#endif
    return true;
  }
#ifdef USE_SWITCH
  void set_dark_mode_switch(switch_::Switch* dark_mode_switch) {
    dark_mode_switch_ = dark_mode_switch;
  }
#endif
  bool get_draw_volume_level() { return draw_volume_level_; }
  void set_draw_volume_level(bool draw_volume_level) {
    draw_volume_level_ = draw_volume_level;
  }
  std::string get_boot_device_name() { return boot_device_name_; }
  void set_boot_device_name(std::string boot_device_name) {
    boot_device_name_ = boot_device_name;
  }

  int getBottomLoadingBarYPosition(int display_height) {
    int barHeight = font_small_->get_height();
    int yPos = display_height - margin_size_ - bottom_bar_margin_ - barHeight;
    return yPos;
  }

  HomeThingColorPalette* get_color_palette() { return color_palette_; }
  void set_color_palette(HomeThingColorPalette* color_palette) {
    color_palette_ = color_palette;
  }

  HomeThingMenuTextHelpers* get_text_helpers() { return text_helpers_; }

  Color primaryTextColor() {
    return text_helpers_->primaryTextColor(get_dark_mode(),
                                           get_color_palette()->get_white(),
                                           get_color_palette()->get_black());
  }
  Color secondaryTextColor() {
    return text_helpers_->primaryTextColor(get_dark_mode(),
                                           get_color_palette()->get_white(),
                                           get_color_palette()->get_white());
  }

  int getCharacterLimit(int xPos, int fontSize, display::TextAlign alignment,
                        int displayWidth) {
    return text_helpers_->getCharacterLimit(
        xPos, fontSize, alignment, displayWidth, get_font_size_width_ratio());
  }
  int getTextWidth(int fontSize, int characterCount) {
    return text_helpers_->getTextWidth(fontSize, characterCount,
                                       get_font_size_width_ratio());
  }

  int drawTextWrapped(int xPos, int yPos, font::Font* font, Color color,
                      display::TextAlign alignment, std::string text,
                      int maxLines, display::DisplayBuffer* display_buffer) {
    return text_helpers_->drawTextWrapped(xPos, yPos, font, color, alignment,
                                          text, maxLines, display_buffer,
                                          get_font_size_width_ratio());
  }

  void drawTextMarquee(int xPos, int yPos, font::Font* font, Color color,
                       display::TextAlign alignment, std::string text,
                       int animationTick,
                       display::DisplayBuffer* display_buffer) {
    text_helpers_->drawTextMarquee(xPos, yPos, font, color, alignment, text,
                                   animationTick, display_buffer,
                                   get_font_size_width_ratio());
  }

 private:
  HomeThingMenuTextHelpers* text_helpers_ = new HomeThingMenuTextHelpers();
  HomeThingColorPalette* color_palette_;
  font::Font* font_small_{nullptr};
  font::Font* font_medium_{nullptr};
  font::Font* font_large_{nullptr};
  font::Font* font_large_heavy_{nullptr};
  font::Font* font_material_large_{nullptr};
  font::Font* font_material_small_{nullptr};
  int header_height_;
  int margin_size_;
  int bottom_bar_margin_;
  int slider_margin_size_;
  int icon_size_;
  int scroll_bar_width_;
  float font_size_width_ratio_;
  DisplayIconEnabledState draw_shuffle_;
  DisplayIconEnabledState draw_repeat_;
  bool draw_header_time_;
  bool draw_battery_level_;
  bool draw_volume_level_;
  std::string boot_device_name_ = "homeThing";
#ifdef USE_SWITCH
  switch_::Switch* dark_mode_switch_{nullptr};
#endif
};
}  // namespace homething_display_state
}  // namespace esphome
