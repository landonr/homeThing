#pragma once

#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeThingApp/homeThingApp.h"

namespace esphome {
namespace homething_app_snake {

class HomeThingAppSnakeHeader
    : public homething_menu_base::HomeThingMenuHeaderSource {
 public:
  HomeThingAppSnakeHeader(int* score) : score(score) {}
  // header
  std::string get_header_title() { return to_string(*score) + "pts"; }

  int draw_header_details(
      int xPos, int yPos, display::DisplayBuffer* display_buffer,
      homething_display_state::HomeThingDisplayState* display_state,
      homething_display_state::HomeThingMenuTextHelpers* text_helpers) {
    return 0;
  }

 protected:
 private:
  const char* const TAG = "homething.app.snake.header";
  int* score;
};

struct Coordinate {
  int x, y;
  Coordinate(int newX, int newY) : x(newX), y(newY) {}
};

enum GameState {
  GAME_STATE_PLAYING,
  GAME_STATE_GAME_OVER,
  GAME_STATE_STARTING
};

class HomeThingAppSnake : public homething_menu_app::HomeThingApp {
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
  homething_menu_app::NavigationCoordination changeDirection(bool clockwise);
  // buttons
  homething_menu_app::NavigationCoordination rotaryScrollClockwise(int rotary);
  homething_menu_app::NavigationCoordination rotaryScrollCounterClockwise(
      int rotary);
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
  HomeThingMenuHeaderSource* header_source_ =
      new HomeThingAppSnakeHeader(&score);

  void set_display_buffer(display::DisplayBuffer* display_buffer) {
    display_buffer_ = display_buffer;
  }

  void set_display_state(
      homething_display_state::HomeThingDisplayState* display_state) {
    display_state_ = display_state;
  }

  bool is_animating() { return true; }

  // state callback
  bool has_state_callback() { return false; }
  void add_on_state_callback(std::function<void()>&& callback){};

 protected:
 private:
  const char* const TAG = "homething.app.snake";

  // display
  display::DisplayBuffer* display_buffer_{nullptr};
  homething_display_state::HomeThingDisplayState* display_state_{nullptr};

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
  int margin = 4;
  int score = 0;
  GameState game_state_ = GAME_STATE_STARTING;

  void active_tick_starting();
  void active_tick_game_over();
  void active_tick_playing();

  void draw_app_starting();
  void draw_app_game_over();
  void draw_app_playing();
};
}  // namespace homething_app_snake
}  // namespace esphome