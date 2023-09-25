#include "homeThingAppSnake.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_app_snake {

void HomeThingAppSnake::reset() {
  ESP_LOGI(TAG, "Resetting snake");
  const auto bounds = get_display_bounds();
  const auto newFruitPosition = get_random_coordinate();
  const auto newSnakePosition = get_random_coordinate();
  fruit_position_.x = newFruitPosition.x;
  fruit_position_.y = newFruitPosition.y;
  snake.clear();
  snake.push_back(newSnakePosition);
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

void HomeThingAppSnake::draw_app(
    int menuIndex,
    const std::vector<homething_menu_base::MenuTitleBase*>* active_menu) {
  active_tick();
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

void HomeThingAppSnake::idleTick(int idleTime, int display_timeout) {}

void HomeThingAppSnake::active_tick() {
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
    ESP_LOGW(TAG, "Snake ate the fruit %d %d, %d %d", newHeadX, newHeadY,
             widthBounds, heightBounds);
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
      reset();
    }
  }
  // Check for collisions with the window boundaries
  if (newHeadX < 0 || newHeadX >= bounds.x || newHeadY < 0 ||
      newHeadY >= bounds.y) {
    ESP_LOGW(TAG, "Snake hit the wall %d %d, %d %d", newHeadX, newHeadY,
             bounds.x, bounds.y);
    reset();
  }
}

int HomeThingAppSnake::root_menu_size() {
  return 1;
}
void HomeThingAppSnake::reset_menu() {}
void HomeThingAppSnake::set_app_menu_index(int app_menu_index) {}

// buttons
void HomeThingAppSnake::rotaryScrollClockwise(int rotary) {}
void HomeThingAppSnake::rotaryScrollCounterClockwise(int rotary) {}
homething_menu_app::NavigationCoordination HomeThingAppSnake::buttonPressUp() {
  snake_direction_ = Coordinate(-1, 0);
  return homething_menu_app::NavigationCoordination::
      NavigationCoordinationReturn;
}
homething_menu_app::NavigationCoordination
HomeThingAppSnake::buttonPressDown() {
  snake_direction_ = Coordinate(1, 0);
  return homething_menu_app::NavigationCoordination::
      NavigationCoordinationReturn;
}  // namespace homething_app_snake
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
  reset();
  if (displayScale < 20) {
    displayScale = displayScale + 2;
  } else {
    displayScale = 2;
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