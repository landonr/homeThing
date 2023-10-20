#include "homeThingAppSnake.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_app_snake {

void HomeThingAppSnake::reset() {
  ESP_LOGI(TAG, "Resetting snake");
  const auto newFruitPosition = get_random_coordinate();
  const auto newSnakePosition = Coordinate(2, 2);
  fruit_position_.x = newFruitPosition.x;
  fruit_position_.y = newFruitPosition.y;
  snake.clear();
  snake.push_back(newSnakePosition);
  score = 0;
  snake_direction_ = Coordinate(1, 0);
}

Coordinate HomeThingAppSnake::get_random_coordinate() {
  const auto bounds = get_display_bounds();
  return Coordinate((rand() % (bounds.x)), (rand() % (bounds.y)));
}

Coordinate HomeThingAppSnake::get_display_bounds() {
  int widthBounds =
      (display_buffer_->get_width() - (margin * 2)) / displayScale;
  int heightBounds = (display_buffer_->get_height() -
                      ((margin * 2) + display_state_->get_header_height())) /
                     displayScale;
  return Coordinate(widthBounds, heightBounds);
}

void HomeThingAppSnake::rootMenuTitles(
    std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {
  menu_titles->push_back(new homething_menu_base::MenuTitleBase(
      "Snake", "", homething_menu_base::ArrowMenuTitleRightIcon));
}

void HomeThingAppSnake::app_menu_titles(
    std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {}

// menu screens
homething_menu_app::NavigationCoordination HomeThingAppSnake::app_menu_select(
    int index) {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

bool HomeThingAppSnake::should_draw_app() {
  return true;
}

void HomeThingAppSnake::draw_resized_pixel(int coordinateX, int coordinateY,
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

void HomeThingAppSnake::draw_app_playing() {
  auto snakeColor = display_state_->get_color_palette()->get_green();
  for (auto segment : snake) {
    draw_resized_pixel(segment.x, segment.y, snakeColor);
  }
  auto fruitColor = display_state_->get_color_palette()->get_red();
  draw_resized_pixel(fruit_position_.x, fruit_position_.y, fruitColor);

  auto borderColor = display_state_->get_color_palette()->get_accent_primary();
  display_buffer_->rectangle(
      margin, margin + display_state_->get_header_height(),
      get_display_bounds().x * displayScale,
      get_display_bounds().y * displayScale, borderColor);
}

void HomeThingAppSnake::draw_app_starting() {
  auto largeFont = display_state_->get_font_large();
  auto mediumFont = display_state_->get_font_medium();
  auto primaryTextColor =
      display_state_->get_color_palette()->get_accent_primary();

  int yPos = display_buffer_->get_height() * 0.1;
  std::string text = "Snake!";
  display_state_->drawTextWrapped(
      display_buffer_->get_width() * 0.5, yPos, largeFont, primaryTextColor,
      display::TextAlign::TOP_CENTER, text, 3, display_buffer_);
  int topScore = 10;
  text = "Top Score: " + std::to_string(topScore);
  yPos = display_buffer_->get_height() * 0.4;
  display_state_->drawTextWrapped(
      display_buffer_->get_width() * 0.5, yPos, mediumFont, primaryTextColor,
      display::TextAlign::TOP_CENTER, text, 3, display_buffer_);

  text = "Center to start. Up to escape.";
  yPos = display_buffer_->get_height() * 0.6;
  display_state_->drawTextWrapped(
      display_buffer_->get_width() * 0.5, yPos, mediumFont, primaryTextColor,
      display::TextAlign::TOP_CENTER, text, 3, display_buffer_);
}

void HomeThingAppSnake::draw_app_game_over() {
  auto largeFont = display_state_->get_font_large();
  auto mediumFont = display_state_->get_font_medium();
  auto primaryTextColor =
      display_state_->get_color_palette()->get_accent_primary();

  int yPos = display_buffer_->get_height() * 0.1;
  std::string text = "Game Over!";
  display_state_->drawTextWrapped(
      display_buffer_->get_width() * 0.5, yPos, largeFont, primaryTextColor,
      display::TextAlign::TOP_CENTER, text, 3, display_buffer_);

  yPos = display_buffer_->get_height() * 0.3;
  text = "Score: " + std::to_string(score);
  display_state_->drawTextWrapped(
      display_buffer_->get_width() * 0.5, yPos, mediumFont, primaryTextColor,
      display::TextAlign::TOP_CENTER, text, 3, display_buffer_);
  int topScore = 10;
  text = "Top Score: " + std::to_string(topScore);
  yPos = display_buffer_->get_height() * 0.4;
  display_state_->drawTextWrapped(
      display_buffer_->get_width() * 0.5, yPos, mediumFont, primaryTextColor,
      display::TextAlign::TOP_CENTER, text, 3, display_buffer_);

  text = "Center to continue";
  yPos = display_buffer_->get_height() * 0.6;
  display_state_->drawTextWrapped(
      display_buffer_->get_width() * 0.5, yPos, mediumFont, primaryTextColor,
      display::TextAlign::TOP_CENTER, text, 3, display_buffer_);
}

void HomeThingAppSnake::draw_app(
    int menuIndex,
    const std::vector<homething_menu_base::MenuTitleBase*>* active_menu) {
  active_tick();
  switch (game_state_) {
    case GameState::GAME_STATE_STARTING:
      draw_app_starting();
      break;
    case GameState::GAME_STATE_PLAYING:
      draw_app_playing();
      break;
    case GameState::GAME_STATE_GAME_OVER:
      draw_app_game_over();
      break;
  }
}

void HomeThingAppSnake::idleTick(int idleTime, int display_timeout) {}

void HomeThingAppSnake::active_tick_playing() {
  if (fruit_position_.x == -1) {
    reset();
  }
  int newHeadY = snake[0].y + snake_direction_.x;
  int newHeadX = snake[0].x + snake_direction_.y;
  const auto bounds = get_display_bounds();
  int widthBounds = bounds.x;
  int heightBounds = bounds.y;
  if (newHeadX == fruit_position_.x && newHeadY == fruit_position_.y) {
    // Snake ate the fruit
    snake.push_back(Coordinate(fruit_position_.x, fruit_position_.y));
    auto newFruitPosition = get_random_coordinate();
    fruit_position_.x = newFruitPosition.x;
    fruit_position_.y = newFruitPosition.y;
    score = score + 1;
    ESP_LOGW(TAG, "Snake ate the fruit score: %d - %d %d, %d %d", score,
             newHeadX, newHeadY, widthBounds, heightBounds);
    return;
  } else {
    // Move the snake
    for (int i = snake.size() - 1; i > 0; --i) {
      snake[i].x = snake[i - 1].x;
      snake[i].y = snake[i - 1].y;
    }
    snake[0].x = newHeadX;
    snake[0].y = newHeadY;
  }

  // Check for collisions with the snake's body
  for (int i = 1; i < snake.size(); ++i) {
    if (snake[i].x == newHeadX && snake[i].y == newHeadY) {
      ESP_LOGW(TAG, "Snake hit itself %d %d, %d %d", newHeadX, newHeadY,
               widthBounds, heightBounds);
      game_state_ = GameState::GAME_STATE_GAME_OVER;
    }
  }
  // Check for collisions with the window boundaries
  if (newHeadX < 0 || newHeadX >= bounds.x || newHeadY < 0 ||
      newHeadY >= bounds.y) {
    ESP_LOGW(TAG, "Snake hit the wall %d %d, %d %d", newHeadX, newHeadY,
             bounds.x, bounds.y);
    game_state_ = GameState::GAME_STATE_GAME_OVER;
  }
}

void HomeThingAppSnake::active_tick_game_over() {}

void HomeThingAppSnake::active_tick_starting() {}

void HomeThingAppSnake::active_tick() {
  switch (game_state_) {
    case GameState::GAME_STATE_STARTING:
      active_tick_starting();
      break;
    case GameState::GAME_STATE_PLAYING:
      active_tick_playing();
      break;
    case GameState::GAME_STATE_GAME_OVER:
      active_tick_game_over();
      break;
  }
}

int HomeThingAppSnake::root_menu_size() {
  return 1;
}
void HomeThingAppSnake::reset_menu() {
  reset();
  game_state_ = GameState::GAME_STATE_STARTING;
}
void HomeThingAppSnake::set_app_menu_index(int app_menu_index) {}

homething_menu_app::NavigationCoordination HomeThingAppSnake::changeDirection(
    bool clockwise) {
  switch (game_state_) {
    case GameState::GAME_STATE_STARTING:
    case GameState::GAME_STATE_GAME_OVER:
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationReturn;
    case GameState::GAME_STATE_PLAYING:
      break;
  }
  if (clockwise) {
    // Rotate the current direction 90 degrees to the right.
    int new_x = snake_direction_.y;
    int new_y = -snake_direction_.x;
    snake_direction_ = Coordinate(new_x, new_y);
  } else {
    // Rotate the current direction 90 degrees to the left.
    int new_x = -snake_direction_.y;
    int new_y = snake_direction_.x;
    snake_direction_ = Coordinate(new_x, new_y);
  }
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

// buttons
homething_menu_app::NavigationCoordination
HomeThingAppSnake::rotaryScrollClockwise(int rotary) {
  return changeDirection(true);
}
homething_menu_app::NavigationCoordination
HomeThingAppSnake::rotaryScrollCounterClockwise(int rotary) {
  return changeDirection(false);
}
homething_menu_app::NavigationCoordination HomeThingAppSnake::buttonPressUp() {
  switch (game_state_) {
    case GameState::GAME_STATE_STARTING:
      reset();
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationPop;
    case GameState::GAME_STATE_GAME_OVER:
    case GameState::GAME_STATE_PLAYING:
      break;
  }
  snake_direction_ = Coordinate(-1, 0);
  return homething_menu_app::NavigationCoordination::
      NavigationCoordinationReturn;
}

homething_menu_app::NavigationCoordination
HomeThingAppSnake::buttonPressDown() {
  snake_direction_ = Coordinate(1, 0);
  return homething_menu_app::NavigationCoordination::
      NavigationCoordinationReturn;
}

homething_menu_app::NavigationCoordination
HomeThingAppSnake::buttonPressLeft() {
  snake_direction_ = Coordinate(0, -1);
  return homething_menu_app::NavigationCoordination::
      NavigationCoordinationReturn;
}

homething_menu_app::NavigationCoordination
HomeThingAppSnake::buttonPressRight() {
  snake_direction_ = Coordinate(0, 1);
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination HomeThingAppSnake::buttonPressSelect(
    int menuIndex) {
  switch (game_state_) {
    case GameState::GAME_STATE_STARTING:
      game_state_ = GameState::GAME_STATE_PLAYING;
      break;
    case GameState::GAME_STATE_PLAYING:
      reset();
      if (displayScale < 20) {
        displayScale = displayScale + 2;
      } else {
        displayScale = 2;
      }
      return homething_menu_app::NavigationCoordination::
          NavigationCoordinationNone;
      break;
    case GameState::GAME_STATE_GAME_OVER:
      reset();
      game_state_ = GameState::GAME_STATE_STARTING;
      break;
  }
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingAppSnake::buttonPressSelectHold() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingAppSnake::buttonPressScreenLeft() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingAppSnake::buttonReleaseScreenLeft() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingAppSnake::buttonPressScreenRight() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

}  // namespace homething_app_snake
}  // namespace esphome