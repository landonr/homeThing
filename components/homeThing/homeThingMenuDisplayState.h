#pragma once

#include <string>
#include "esphome/components/display/display_buffer.h"
#include "esphome/core/color.h"

namespace esphome {
namespace homething_menu_base {

class HomeThingMenuDisplayState {
 public:
  display::Font* get_small_font() { return small_font_; }
  void set_small_font(display::Font* small_font) { small_font_ = small_font; }

  display::Font* get_medium_font() { return medium_font_; }
  void set_medium_font(display::Font* medium_font) {
    medium_font_ = medium_font;
  }

  display::Font* get_large_font() { return large_font_; }
  void set_large_font(display::Font* large_font) { large_font_ = large_font; }

  display::Font* get_large_heavy_font() { return large_heavy_font_; }
  void set_large_heavy_font(display::Font* large_heavy_font) {
    large_heavy_font_ = large_heavy_font;
  }

  display::Font* get_material_font_large() { return material_font_large_; }
  void set_material_font_large(display::Font* material_font_large) {
    material_font_large_ = material_font_large;
  }

  display::Font* get_material_font_small() { return material_font_small_; }
  void set_material_font_small(display::Font* material_font_small) {
    material_font_small_ = material_font_small;
  }

  display::Font* get_menu_font_logo() { return menu_font_logo_; }
  void set_menu_font_logo(display::Font* menu_font_logo) {
    menu_font_logo_ = menu_font_logo;
  }

  bool get_draw_now_playing_bottom_menu() { return draw_now_playing_menu_; }
  void set_draw_now_playing_bottom_menu(bool draw_now_playing_menu) {
    draw_now_playing_menu_ = draw_now_playing_menu;
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
  int get_boot_logo_size() { return boot_logo_size_; }
  void set_boot_logo_size(int boot_logo_size) {
    boot_logo_size_ = boot_logo_size;
  }
  int get_now_playing_max_lines() { return now_playing_max_lines_; }
  void set_now_playing_max_lines(int now_playing_max_lines) {
    now_playing_max_lines_ = now_playing_max_lines;
  }
  float get_font_size_width_ratio() { return font_size_width_ratio_; }
  void set_font_size_width_ratio(float font_size_width_ratio) {
    font_size_width_ratio_ = font_size_width_ratio;
  }
  bool get_draw_shuffle_disabled() { return draw_shuffle_disabled_; }
  void set_draw_shuffle_disabled(bool draw_shuffle_disabled) {
    draw_shuffle_disabled_ = draw_shuffle_disabled;
  }
  bool get_draw_header_time() { return draw_header_time_; }
  void set_draw_header_time(bool draw_header_time) {
    draw_header_time_ = draw_header_time;
  }
  bool get_draw_battery_level() { return draw_battery_level_; }
  void set_draw_battery_level(bool draw_battery_level) {
    draw_battery_level_ = draw_battery_level;
  }
  bool get_dark_mode() { return dark_mode_; }
  void set_dark_mode(bool dark_mode) { dark_mode_ = dark_mode; }
  bool get_draw_volume_level() { return draw_volume_level_; }
  void set_draw_volume_level(bool draw_volume_level) {
    draw_volume_level_ = draw_volume_level;
  }
  std::string get_boot_device_name() { return boot_device_name_; }
  void set_boot_device_name(std::string boot_device_name) {
    boot_device_name_ = boot_device_name;
  }

// color
  Color get_color_gray_dark() { return color_gray_dark_; }
  void set_color_gray_dark(Color color_gray_dark) {
    color_gray_dark_ = color_gray_dark;
  }
  Color get_color_gray_dark_2() { return color_gray_dark_2_; }
  void set_color_gray_dark_2(Color color_gray_dark_2) {
    color_gray_dark_2_ = color_gray_dark_2;
  }
  Color get_color_gray() { return color_gray_; }
  void set_color_gray(Color color_gray) { color_gray_ = color_gray; }
  Color get_color_accent_primary() { return color_accent_primary_; }
  void set_color_accent_primary(Color color_accent_primary) {
    color_accent_primary_ = color_accent_primary;
  }
  Color get_color_blue() { return color_blue_; }
  void set_color_blue(Color color_blue) { color_blue_ = color_blue; }
  Color get_color_green() { return color_green_; }
  void set_color_green(Color color_green) { color_green_ = color_green; }
  Color get_color_black() { return color_black_; }
  void set_color_black(Color color_black) { color_black_ = color_black; }
  Color get_color_white() { return color_white_; }
  void set_color_white(Color color_white) { color_white_ = color_white; }
  Color get_color_pink() { return color_pink_; }
  void set_color_pink(Color color_pink) { color_pink_ = color_pink; }
  Color get_color_red() { return color_red_; }
  void set_color_red(Color color_red) { color_red_ = color_red; }
  Color get_color_yellow() { return color_yellow_; }
  void set_color_yellow(Color color_yellow) { color_yellow_ = color_yellow; }

  int getBottomBarYPosition(bool spaceForMenu, int display_height) {
    int barHeight = small_font_->get_height();
    int menuSpace = spaceForMenu && get_draw_now_playing_bottom_menu()
                        ? large_font_->get_height()
                        : 0;
    int yPos = display_height - margin_size_ - menuSpace - bottom_bar_margin_ -
               barHeight;
    return yPos;
  }

 private:
  Color color_gray_dark_ = Color(102, 102, 102);
  Color color_gray_dark_2_ = Color(20, 20, 20);
  Color color_gray_ = Color(25, 25, 25);
  Color color_accent_primary_ = Color(0, 114, 165);
  Color color_blue_ = Color(0, 0, 255);
  Color color_green_ = Color(0, 255, 0);
  Color color_black_ = Color(0, 0, 0);
  Color color_white_ = Color(255, 255, 255);
  Color color_pink_ = Color(255, 40, 255);
  Color color_red_ = Color(255, 0, 0);
  Color color_yellow_ = Color(255, 191, 25);

  display::Font* small_font_{nullptr};
  display::Font* medium_font_{nullptr};
  display::Font* large_font_{nullptr};
  display::Font* large_heavy_font_{nullptr};
  display::Font* material_font_large_{nullptr};
  display::Font* material_font_small_{nullptr};
  display::Font* menu_font_logo_{nullptr};
  int header_height_;
  int margin_size_;
  int bottom_bar_margin_;
  int slider_margin_size_;
  int icon_size_;
  int scroll_bar_width_;
  int boot_logo_size_;
  int now_playing_max_lines_;
  float font_size_width_ratio_;
  bool draw_shuffle_disabled_;
  bool draw_header_time_;
  bool draw_battery_level_;
  bool dark_mode_;
  bool draw_volume_level_;
  bool draw_now_playing_menu_;
  std::string boot_device_name_ = "homeThing";
};
}  // namespace homething_menu_base
}  // namespace esphome
