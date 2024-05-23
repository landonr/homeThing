#include "HomeThingAppImage.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_app_image {
void HomeThingAppImage::rootMenuTitles(
    std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {
  menu_titles->push_back(new homething_menu_base::MenuTitleBase(
      "Camera", "", homething_menu_base::ArrowMenuTitleRightIcon));
}

void HomeThingAppImage::app_menu_titles(
    std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {}

// menu screens
homething_menu_app::NavigationCoordination HomeThingAppImage::app_menu_select(
    int index) {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

bool HomeThingAppImage::should_draw_app() {
  return true;
}

void HomeThingAppImage::draw_app(
    int menuIndex,
    const std::vector<homething_menu_base::MenuTitleBase*>* active_menu) {
  int centerX = display_buffer_->get_width() / 2;
  int imageXPos = centerX;
  int imageYPos = display_state_->get_header_height() + 2;
  display_buffer_->image(imageXPos, imageYPos, camera_image_, display::ImageAlign::TOP_CENTER, Color(255, 255, 255));
}

bool HomeThingAppImage::idleTick(int idleTime, int display_timeout) {
  return false;
}

int HomeThingAppImage::root_menu_size() {
  return 1;
}
void HomeThingAppImage::reset_menu() {}
void HomeThingAppImage::set_app_menu_index(int app_menu_index) {}

// buttons
homething_menu_app::NavigationCoordination
HomeThingAppImage::rotaryScrollClockwise(int rotary) {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}
homething_menu_app::NavigationCoordination
HomeThingAppImage::rotaryScrollCounterClockwise(int rotary) {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}
homething_menu_app::NavigationCoordination
HomeThingAppImage::buttonPressUp() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationPop;
}

homething_menu_app::NavigationCoordination
HomeThingAppImage::buttonPressDown() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingAppImage::buttonPressLeft() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingAppImage::buttonPressRight() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingAppImage::buttonPressSelect(int menuIndex) {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingAppImage::buttonPressOption() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

}  // namespace homething_app_weather
}  // namespace esphome
