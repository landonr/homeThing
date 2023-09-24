#pragma once

#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"

#include "esphome/components/homeThing/homeThingMenuScreen.h"
#include "esphome/components/homeThing/homeThingMenuTextHelpers.h"
#include "esphome/components/homeThingDisplayState/homeThingDisplayState.h"

namespace esphome {
namespace homething_menu_app {

enum NavigationCoordination {
  NavigationCoordinationNone,
  NavigationCoordinationPop,
  NavigationCoordinationRoot,
  NavigationCoordinationUpdate,
  NavigationCoordinationReturn
};

class HomeThingMenuApp : public homething_menu_base::HomeThingMenuHeaderSource {
 public:
  // menu titles
  virtual void rootMenuTitles(
      std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {}
  virtual void app_menu_titles(
      std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {}

  // menu screens
  virtual NavigationCoordination app_menu_select(int index) {
    return NavigationCoordination::NavigationCoordinationNone;
  }
  virtual bool should_draw_app() { return false; }
  virtual void draw_app(
      int menuIndex,
      const std::vector<homething_menu_base::MenuTitleBase*>* active_menu);
  virtual void idleTick(int idleTime, int display_timeout) {}
  virtual int root_menu_size() { return 0; }
  virtual void reset_menu() {}
  virtual void set_app_menu_index(int app_menu_index) {}

  // buttons
  virtual void rotaryScrollClockwise(int rotary) {}
  virtual void rotaryScrollCounterClockwise(int rotary) {}
  virtual NavigationCoordination buttonPressUp() {
    return NavigationCoordination::NavigationCoordinationNone;
  }
  virtual NavigationCoordination buttonPressDown() {
    return NavigationCoordination::NavigationCoordinationNone;
  }
  virtual NavigationCoordination buttonPressLeft() {
    return NavigationCoordination::NavigationCoordinationNone;
  }
  virtual NavigationCoordination buttonPressRight() {
    return NavigationCoordination::NavigationCoordinationNone;
  }
  virtual NavigationCoordination buttonPressSelect(int menuIndex) {
    return NavigationCoordination::NavigationCoordinationNone;
  }
  virtual NavigationCoordination buttonPressSelectHold() {
    return NavigationCoordination::NavigationCoordinationNone;
  }
  virtual NavigationCoordination buttonPressScreenLeft() {
    return NavigationCoordination::NavigationCoordinationNone;
  }
  virtual NavigationCoordination buttonReleaseScreenLeft() {
    return NavigationCoordination::NavigationCoordinationNone;
  }
  virtual NavigationCoordination buttonPressScreenRight() {
    return NavigationCoordination::NavigationCoordinationNone;
  }

  // header
  virtual homething_menu_base::HomeThingMenuHeaderSource* get_header_source() {
    return header_source_;
  }

  // display

  virtual bool is_animating() { return false; }

 protected:
  homething_menu_base::HomeThingMenuHeaderSource* header_source_{nullptr};

 private:
  const char* const TAG = "homething.menu.app";
};
}  // namespace homething_menu_app
}  // namespace esphome