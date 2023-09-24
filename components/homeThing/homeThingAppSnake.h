#pragma once

#include "esphome/components/homeThing/homeThingMenuApp.h"
#include "esphome/components/homeThing/homeThingMenuHeader.h"

namespace esphome {
namespace homething_app_snake {

class HomeThingAppSnakeHeader
    : public homething_menu_base::HomeThingMenuHeaderSource {
 public:
  // header
  std::string get_header_title() { return "Big Slime"; }

  int draw_header_details(
      int xPos, int yPos, display::DisplayBuffer* display_buffer,
      homething_menu_base::HomeThingMenuDisplayState* display_state,
      homething_menu_base::HomeThingMenuTextHelpers* text_helpers) {
    return 0;
  }

 protected:
 private:
  const char* const TAG = "homething.app.snake.header";
};

struct Coordinate {
  int x, y;
  Coordinate(int newX, int newY) : x(newX), y(newY) {}
};

class HomeThingAppSnake : public homething_menu_app::HomeThingMenuApp {
 public:
  void reset();
  // menu titles
  void rootMenuTitles(
      std::vector<homething_menu_base::MenuTitleBase*>* menu_titles);

  void app_menu_titles(
      std::vector<homething_menu_base::MenuTitleBase*>* menu_titles);

  // menu screens
  homething_menu_app::NavigationCoordination app_menu_select(int index);
  bool should_draw_app();
  void draw_app(
      int menuIndex,
      const std::vector<homething_menu_base::MenuTitleBase*>* active_menu);

  void idleTick(int idleTime, int display_timeout);
  void active_tick();

  int root_menu_size();
  void reset_menu();
  void set_app_menu_index(int app_menu_index);

  // buttons
  void rotaryScrollClockwise(int rotary);
  void rotaryScrollCounterClockwise(int rotary);
  homething_menu_app::NavigationCoordination buttonPressUp();
  homething_menu_app::NavigationCoordination buttonPressDown();
  homething_menu_app::NavigationCoordination buttonPressLeft();
  homething_menu_app::NavigationCoordination buttonPressRight();
  homething_menu_app::NavigationCoordination buttonPressSelect(int menuIndex);
  homething_menu_app::NavigationCoordination buttonPressSelectHold();
  homething_menu_app::NavigationCoordination buttonPressScreenLeft();
  homething_menu_app::NavigationCoordination buttonReleaseScreenLeft();
  homething_menu_app::NavigationCoordination buttonPressScreenRight();

  homething_menu_base::HomeThingMenuHeaderSource* get_header_source() {
    return header_source_;
  }
  HomeThingMenuHeaderSource* header_source_ = new HomeThingAppSnakeHeader();

  void set_display_buffer(display::DisplayBuffer* display_buffer) {
    display_buffer_ = display_buffer;
  }

  void set_display_state(
      homething_menu_base::HomeThingMenuDisplayState* display_state) {
    display_state_ = display_state;
  }

  void set_text_helpers(
      homething_menu_base::HomeThingMenuTextHelpers* text_helpers) {
    text_helpers_ = text_helpers;
  }

  bool is_animating() { return true; }

 protected:
 private:
  const char* const TAG = "homething.app.snake";

  // display
  display::DisplayBuffer* display_buffer_{nullptr};
  homething_menu_base::HomeThingMenuDisplayState* display_state_{nullptr};
  homething_menu_base::HomeThingMenuTextHelpers* text_helpers_{nullptr};

  // menu titles

  void sourceMenuTitles(
      std::vector<homething_menu_base::MenuTitleBase*>* menu_titles);
  void media_player_menu_titles(
      std::vector<homething_menu_base::MenuTitleBase*>* menu_titles);

  void draw_resized_pixel(int coordinateX, int coordinateY, Color color);
Coordinate get_display_bounds();
  void create_new_fruit();
  Coordinate get_random_coordinate();
  Coordinate fruit_position_ = Coordinate(-1, -1);
  Coordinate snake_direction_ = Coordinate(1, 0);
  std::vector<Coordinate> snake = {Coordinate(30, 30)};
  double displayScale = 16;
  int margin = 10;
};
}  // namespace homething_app_snake
}  // namespace esphome