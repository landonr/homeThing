#pragma once

#include <string>
#include <vector>
#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeThingApp/homeThingApp.h"
#include "esphome/components/image/image.h"
namespace esphome {
namespace homething_app_image {

class HomeThingAppImageHeader
    : public homething_menu_base::HomeThingMenuHeaderSource {
 public:
  std::string get_header_title() { return "Camera"; }

  int draw_header_details(
      int xPos, int yPos, display::DisplayBuffer* display_buffer,
      homething_display_state::HomeThingDisplayState* display_state,
      homething_display_state::HomeThingMenuTextHelpers* text_helpers) {
    return 0;
  }

 private:
  const char* const TAG = "homething.app.image.header";
};

class HomeThingAppImage : public homething_menu_app::HomeThingApp {
 public:
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

  bool idleTick(int idleTime, int display_timeout);
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
  homething_menu_app::NavigationCoordination buttonPressOption();

  homething_menu_base::HomeThingMenuHeaderSource* get_header_source() {
    return header_source_;
  }
  HomeThingMenuHeaderSource* header_source_ = new HomeThingAppImageHeader();

  void set_display_buffer(display::DisplayBuffer* display_buffer) {
    display_buffer_ = display_buffer;
  }

  void set_display_state(
      homething_display_state::HomeThingDisplayState* display_state) {
    display_state_ = display_state;
  }

  bool is_animating() { return false; }

  // state callback
  bool has_state_callback() { return false; }
  void add_on_state_callback(std::function<void()>&& callback) {}

  // setup
  void set_camera_image(image::Image* camera_image) {
    camera_image_ = camera_image;
  }

 private:
  const char* const TAG = "homething.app.image";

  // display
  display::DisplayBuffer* display_buffer_{nullptr};
  homething_display_state::HomeThingDisplayState* display_state_{nullptr};

  // images

  image::Image* camera_image_{nullptr};
};
}  // namespace homething_app_image
}  // namespace esphome
