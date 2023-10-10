#include "HomeThingAppBreakout.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_app_breakout {

void HomeThingAppBreakout::reset() {
  ESP_LOGI(TAG, "Resetting Breakout");

  // Initialize game variables
  score = 0;
  lives = 3;  // Number of lives the player starts with
  game_over = false;

  // Set up the paddle
  paddle_width = 30;  // Adjust the paddle width as needed
  paddle_position_x = (display_buffer_->get_width() - paddle_width) / 2;
  paddle_position_y =
      display_buffer_->get_height() - 10;  // Adjust paddle position as needed

  // Set up the ball
  ball_radius = 4;  // Adjust the ball size as needed
  ball_position_x = display_buffer_->get_width() / 2;
  ball_position_y = display_buffer_->get_height() / 2;
  ball_direction_x = 1;   // Initial direction (1 or -1)
  ball_direction_y = -1;  // Initial direction (-1 means moving upward)

  // Set up the bricks
  brick_rows = 4;  // Number of rows of bricks
  brick_cols = 8;  // Number of columns of bricks
  brick_width = (display_buffer_->get_width() - margin * 2) / (brick_rows + margin);  // Adjust brick width as needed
  brick_height = 3;          // Adjust brick height as needed

  // Initialize the bricks array (you may use a data structure like a 2D array)
  for (int row = 0; row < brick_rows; ++row) {
    for (int col = 0; col < brick_cols; ++col) {
      bricks[row][col] = true;  // Initialize all bricks as active
    }
  }
}

Coordinate HomeThingAppBreakout::calculate_brick_position(int row, int col) {
  // Calculate the X and Y position of the brick based on row, column, and margins
  int topMargin = display_state_->get_header_height() + margin * 2;
  int brick_x = (col * (brick_width + margin)) + margin;
  int brick_y = (row * (brick_height + margin)) + topMargin;
  return Coordinate(brick_x, brick_y);
}

void HomeThingAppBreakout::draw_paddle() {
  // Determine the paddle's dimensions and color
  int paddle_height = 5;  // Adjust the paddle height as needed
  Color paddle_color = display_state_->get_color_palette()->get_blue();

  // Draw the paddle rectangle on the display
  display_buffer_->filled_rectangle(
      paddle_position_x, paddle_position_y, paddle_width,
      paddle_position_y + paddle_height, paddle_color);
}

void HomeThingAppBreakout::draw_ball() {
  // Determine the ball's color
  Color ball_color = display_state_->get_color_palette()->get_yellow();

  // Draw the ball as a filled circle on the display
  display_buffer_->filled_circle(ball_position_x, ball_position_y, ball_radius,
                                 ball_color);
}

void HomeThingAppBreakout::draw_bricks() {
  // Determine brick color and size
  Color brick_color = display_state_->get_color_palette()->get_red();
  int topMargin = display_state_->get_header_height() + margin * 2;
  // Iterate over the bricks and draw active ones
  for (int row = 0; row < brick_rows; ++row) {
    for (int col = 0; col < brick_cols; ++col) {
      if (bricks[row][col]) {
        Coordinate brick_position = calculate_brick_position(row, col);
        // Draw a rectangle for each active brick
        display_buffer_->filled_rectangle(brick_position.x, brick_position.y, brick_width,
                                          brick_height, brick_color);
      }
    }
  }
}

void HomeThingAppBreakout::draw_score() {
  // Determine font, color, and position for the score display
  auto font = display_state_->get_font_medium();
  Color text_color = display_state_->get_color_palette()->get_white();
  int text_x = 10;  // Adjust the X position as needed
  int text_y = 10;  // Adjust the Y position as needed

  // Convert the score integer to a string
  std::string score_text = "Score: " + std::to_string(score);

  // Draw the score on the display
  display_state_->drawTextWrapped(text_x, text_y, font, text_color,
                                  display::TextAlign::TOP_LEFT, score_text, 1,
                                  display_buffer_);
}

void HomeThingAppBreakout::draw_lives() {
  // Determine font, color, and position for the lives display
  auto font = display_state_->get_font_medium();
  Color text_color = display_state_->get_color_palette()->get_white();
  int text_x =
      display_buffer_->get_width() - 10;  // Adjust the X position as needed
  int text_y = 10;                        // Adjust the Y position as needed

  // Convert the lives integer to a string
  std::string lives_text = "Lives: " + std::to_string(lives);

  // Draw the lives on the display
  display_state_->drawTextWrapped(text_x, text_y, font, text_color,
                                  display::TextAlign::TOP_RIGHT, lives_text, 1,
                                  display_buffer_);
}

void HomeThingAppBreakout::draw_game_over() {
  // Determine font, color, and position for the "Game Over" message
  auto font = display_state_->get_font_large();
  Color text_color = display_state_->get_color_palette()->get_red();
  int text_x = display_buffer_->get_width() / 2;
  int text_y = display_buffer_->get_height() / 2;

  // Display the "Game Over" message
  display_state_->drawTextWrapped(text_x, text_y, font, text_color,
                                  display::TextAlign::TOP_CENTER, "Game Over",
                                  2, display_buffer_);
}

void HomeThingAppBreakout::draw_win() {
  // Determine font, color, and position for the "You Win!" message
  auto font = display_state_->get_font_large();
  Color text_color = display_state_->get_color_palette()->get_green();
  int text_x = display_buffer_->get_width() / 2;
  int text_y = display_buffer_->get_height() / 2;

  // Display the "You Win!" message
  display_state_->drawTextWrapped(text_x, text_y, font, text_color,
                                  display::TextAlign::TOP_CENTER, "You Win!", 2,
                                  display_buffer_);
}

Coordinate HomeThingAppBreakout::get_display_bounds() {
  int widthBounds =
      (display_buffer_->get_width() - (margin * 2)) / displayScale;
  int heightBounds = (display_buffer_->get_height() -
                      ((margin * 2) + display_state_->get_header_height())) /
                     displayScale;
  return Coordinate(widthBounds, heightBounds);
}

void HomeThingAppBreakout::rootMenuTitles(
    std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {
  menu_titles->push_back(new homething_menu_base::MenuTitleBase(
      "Breakout", "", homething_menu_base::ArrowMenuTitleRightIcon));
}

void HomeThingAppBreakout::app_menu_titles(
    std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {}

// menu screens
homething_menu_app::NavigationCoordination
HomeThingAppBreakout::app_menu_select(int index) {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

bool HomeThingAppBreakout::should_draw_app() {
  return true;
}

void HomeThingAppBreakout::draw_resized_pixel(int coordinateX, int coordinateY,
                                              Color color) {
  int x = coordinateX * displayScale;
  int y = coordinateY * displayScale;
  for (int i = 0; i < displayScale; ++i) {
    for (int j = 0; j < displayScale; ++j) {
      display_buffer_->draw_pixel_at(
          x + i + margin, y + j + margin + display_state_->get_header_height(),
          color);
    }
  }
}

void HomeThingAppBreakout::draw_app(
    int menuIndex,
    const std::vector<homething_menu_base::MenuTitleBase*>* active_menu) {
  active_tick();
  draw_paddle();
  draw_ball();
  draw_bricks();
  draw_score();
  // draw_lives();
}

void HomeThingAppBreakout::idleTick(int idleTime, int display_timeout) {}

void HomeThingAppBreakout::active_tick() {
  if (score == -1) {
    reset();
  }
  // Check if the game is over; if so, do nothing
  if (game_over) {
    return;
  }

  ball_position_x += ball_direction_x * ball_speed;
  ball_position_y += ball_direction_y * ball_speed;

  // Check for collisions with the paddle
  if (ball_position_x >= paddle_position_x &&
      ball_position_x <= paddle_position_x + paddle_width &&
      ball_position_y >= paddle_position_y) {
    // Ball hits the paddle, reverse its vertical direction
    ball_direction_y = -ball_direction_y;
  }

  // Check for collisions with the walls (left, right, and top)
  if (ball_position_x <= 0 || ball_position_x >= display_buffer_->get_width()) {
    ball_direction_x = -ball_direction_x;
  }
  if (ball_position_y <= display_state_->get_header_height()) {
    ball_direction_y = -ball_direction_y;
  }

  // Check for collisions with the bricks
  // Iterate over the bricks and check if the ball hits any active brick
  for (int row = 0; row < brick_rows; ++row) {
    for (int col = 0; col < brick_cols; ++col) {
      if (bricks[row][col]) {
        Coordinate brick_position = calculate_brick_position(row, col);

        // Check if the ball collides with the brick
        if (ball_position_x >= brick_position.x &&
            ball_position_x <= brick_position.x + brick_width &&
            ball_position_y >= brick_position.y &&
            ball_position_y <= brick_position.y + brick_height) {
          // Ball hits a brick, remove the brick and reverse ball's direction
          bricks[row][col] = false;
          ball_direction_y = -ball_direction_y;
          // Increase the player's score
          score += 10;  // You can adjust the score increment as needed
        }
      }
    }
  }

  // Check for game over conditions
  if (ball_position_y >= display_buffer_->get_height()) {
    // Ball went below the paddle, player loses a life
    lives -= 1;
    if (lives <= 0) {
      // No lives left, game over
      game_over = true;
    } else {
      // Reset the ball and paddle positions
      ball_position_x = display_buffer_->get_width() / 2;
      ball_position_y = display_buffer_->get_height() / 2;
      paddle_position_x = (display_buffer_->get_width() - paddle_width) / 2;
    }
  }

  // Check for victory condition (all bricks destroyed)
  bool all_bricks_destroyed = true;
  for (int row = 0; row < brick_rows; ++row) {
    for (int col = 0; col < brick_cols; ++col) {
      if (bricks[row][col]) {
        all_bricks_destroyed = false;
        break;
      }
    }
  }
  if (all_bricks_destroyed) {
    // Player wins
    game_over = true;
    // You can add a victory message or transition to the next level here
  }
}

int HomeThingAppBreakout::root_menu_size() {
  return 1;
}
void HomeThingAppBreakout::reset_menu() {}
void HomeThingAppBreakout::set_app_menu_index(int app_menu_index) {}

// buttons
void HomeThingAppBreakout::rotaryScrollClockwise(int rotary) {
  // Move the paddle to the right when the rotary encoder is scrolled clockwise
  paddle_position_x += paddleSpeed;

  // Ensure the paddle stays within the display boundaries
  if (paddle_position_x + paddle_width > display_buffer_->get_width()) {
    paddle_position_x = display_buffer_->get_width() - paddle_width;
  }
}

void HomeThingAppBreakout::rotaryScrollCounterClockwise(int rotary) {
  // Move the paddle to the left when the rotary encoder is scrolled counter-clockwise
  paddle_position_x -= paddleSpeed;

  // Ensure the paddle stays within the display boundaries
  if (paddle_position_x < 0) {
    paddle_position_x = 0;
  }
}

homething_menu_app::NavigationCoordination
HomeThingAppBreakout::buttonPressUp() {
  return homething_menu_app::NavigationCoordination::
      NavigationCoordinationReturn;
}
homething_menu_app::NavigationCoordination
HomeThingAppBreakout::buttonPressDown() {
  return homething_menu_app::NavigationCoordination::
      NavigationCoordinationReturn;
}
homething_menu_app::NavigationCoordination
HomeThingAppBreakout::buttonPressLeft() {
  return homething_menu_app::NavigationCoordination::
      NavigationCoordinationReturn;
}
homething_menu_app::NavigationCoordination
HomeThingAppBreakout::buttonPressRight() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}
homething_menu_app::NavigationCoordination
HomeThingAppBreakout::buttonPressSelect(int menuIndex) {
  reset();
  if (displayScale < 20) {
    displayScale = displayScale + 2;
  } else {
    displayScale = 2;
  }
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}
homething_menu_app::NavigationCoordination
HomeThingAppBreakout::buttonPressSelectHold() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}
homething_menu_app::NavigationCoordination
HomeThingAppBreakout::buttonPressScreenLeft() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}
homething_menu_app::NavigationCoordination
HomeThingAppBreakout::buttonReleaseScreenLeft() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}
homething_menu_app::NavigationCoordination
HomeThingAppBreakout::buttonPressScreenRight() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

}  // namespace homething_app_breakout
}  // namespace esphome