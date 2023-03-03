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
  int header_height_ = 24;
  int margin_size_ = 8;
  int bottom_bar_margin_ = 8;
  int slider_margin_size_ = 8;
  int icon_size_ = 24;
  int scroll_bar_width_ = 4;
  int boot_logo_size_ = 48;
  int now_playing_max_lines_ = 6;
  float font_size_width_ratio_ = 0.6;

  bool draw_shuffle_disabled_ = false;
  bool draw_header_time_ = true;
  bool draw_battery_level_ = true;
  bool dark_mode_ = true;
  bool draw_now_playing_menu_ = true;
  bool draw_volume_level_ = true;
  std::string boot_device_name_ = "";

  int getBottomBarYPosition(bool spaceForMenu, int display_height) {
    int barHeight = small_font_->get_height();
    int menuSpace =
        spaceForMenu && draw_now_playing_menu_ ? large_font_->get_height() : 0;
    int yPos = display_height - margin_size_ - menuSpace - bottom_bar_margin_ -
               barHeight;
    return yPos;
  }

 private:
  display::Font* small_font_{nullptr};
  display::Font* medium_font_{nullptr};
  display::Font* large_font_{nullptr};
  display::Font* large_heavy_font_{nullptr};
  display::Font* material_font_large_{nullptr};
  display::Font* material_font_small_{nullptr};
  display::Font* menu_font_logo_{nullptr};
};
}  // namespace homething_menu_base
}  // namespace esphome
