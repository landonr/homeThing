#pragma once

#include "esphome/core/color.h"

namespace esphome {
namespace homething_display_state {

class HomeThingColorPalette {
 public:
  // color
  Color get_gray_dark() { return gray_dark_; }
  void set_gray_dark(Color gray_dark) { gray_dark_ = gray_dark; }
  Color get_gray_dark_2() { return gray_dark_2_; }
  void set_gray_dark_2(Color gray_dark_2) { gray_dark_2_ = gray_dark_2; }
  Color get_gray() { return gray_; }
  void set_gray(Color gray) { gray_ = gray; }
  Color get_accent_primary() { return accent_primary_; }
  void set_accent_primary(Color accent_primary) {
    accent_primary_ = accent_primary;
  }
  Color get_blue() { return blue_; }
  void set_blue(Color blue) { blue_ = blue; }
  Color get_green() { return green_; }
  void set_green(Color green) { green_ = green; }
  Color get_black() { return black_; }
  void set_black(Color black) { black_ = black; }
  Color get_white() { return white_; }
  void set_white(Color white) { white_ = white; }
  Color get_pink() { return pink_; }
  void set_pink(Color pink) { pink_ = pink; }
  Color get_red() { return red_; }
  void set_red(Color red) { red_ = red; }
  Color get_yellow() { return yellow_; }
  void set_yellow(Color yellow) { yellow_ = yellow; }

 private:
  Color gray_dark_ = Color(102, 102, 102);
  Color gray_dark_2_ = Color(20, 20, 20);
  Color gray_ = Color(25, 25, 25);
  Color accent_primary_ = Color(0, 114, 165);
  Color blue_ = Color(0, 0, 255);
  Color green_ = Color(0, 255, 0);
  Color black_ = Color(0, 0, 0);
  Color white_ = Color(255, 255, 255);
  Color pink_ = Color(255, 40, 255);
  Color red_ = Color(255, 0, 0);
  Color yellow_ = Color(255, 191, 25);
};
}  // namespace homething_display_state
}  // namespace esphome
