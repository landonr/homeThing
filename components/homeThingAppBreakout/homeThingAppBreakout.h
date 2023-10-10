#pragma once

#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeThingApp/homeThingApp.h"

namespace esphome {
namespace homething_app_breakout {

class HomeThingAppBreakoutHeader
    : public homething_menu_base::HomeThingMenuHeaderSource {
 public:
  // header
  std::string get_header_title() { return "Breakout"; }

  int draw_header_details(
      int xPos, int yPos, display::DisplayBuffer* display_buffer,
      homething_display_state::HomeThingDisplayState* display_state,
      homething_display_state::HomeThingMenuTextHelpers* text_helpers) {
    return 0;
  }

 protected:
 private:
  const char* const TAG = "homething.app.breakout.header";
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

class HomeThingAppBreakout : public homething_menu_app::HomeThingApp {
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
  HomeThingMenuHeaderSource* header_source_ = new HomeThingAppBreakoutHeader();

  void set_display_buffer(display::DisplayBuffer* display_buffer) {
    display_buffer_ = display_buffer;
  }

  void set_display_state(
      homething_display_state::HomeThingDisplayState* display_state) {
    display_state_ = display_state;
  }

  bool is_animating() { return true; }

 protected:
 private:
  const char* const TAG = "homething.app.breakout";

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
  Coordinate calculate_brick_position(int row, int col);

  // Paddle properties
  int paddle_width = 0;
  int paddle_height = 5;
  int paddle_position_x = 0;
  int paddle_position_y = 0 + get_bottom_margin();

  // Ball properties
  int ball_radius = 0;
  int ball_position_x = 0;
  int ball_position_y = 0;
  int ball_direction_x = 0;
  int ball_direction_y = 0;

  // Brick properties
  int brick_rows = 0;
  int brick_cols = 0;
  int brick_width = 0;
  int brick_height = 0;
  bool bricks[24][24];

  // Game variables
  int score = -1;
  int lives = -1;
  GameState game_state_ = GAME_STATE_STARTING;

  // ui
  int margin = 8;
  int paddleSpeed = 12;
  int ball_speed = 8;
  int bottomMargin = 12;
  double displayScale = 16;
  int get_top_margin();
  int get_bottom_margin();

  // Drawing functions
  void draw_paddle();
  void draw_ball();
  void draw_bricks();
  void draw_score();
  void draw_lives();
  void draw_game_over();
  void draw_win();
  void draw_starting();
};
}  // namespace homething_app_breakout
}  // namespace esphome