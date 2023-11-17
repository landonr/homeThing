#pragma once

#include <string>
#include <vector>
#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeThingApp/homeThingApp.h"
#include "esphome/components/remote_base/pronto_protocol.h"
#include "esphome/components/remote_transmitter/remote_transmitter.h"
namespace esphome {
namespace homething_cattoy_app {

class HomeThingCatToyAppHeader
    : public homething_menu_base::HomeThingMenuHeaderSource {
 public:
  // header
  std::string get_header_title() { return "Cat Toy"; }

  int draw_header_details(
      int xPos, int yPos, display::DisplayBuffer* display_buffer,
      homething_display_state::HomeThingDisplayState* display_state,
      homething_display_state::HomeThingMenuTextHelpers* text_helpers) {
    return 0;
  }

 private:
  const char* const TAG = "homething.cattoy.header";
};

class HomeThingCatToyApp : public homething_menu_app::HomeThingApp {
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
  homething_menu_app::NavigationCoordination buttonPressOption();

  homething_menu_base::HomeThingMenuHeaderSource* get_header_source() {
    return header_source_;
  }
  HomeThingMenuHeaderSource* header_source_ = new HomeThingCatToyAppHeader();

  void set_remote_transmitter(
      remote_transmitter::RemoteTransmitterComponent* remote_transmitter) {
    remote_transmitter_ = remote_transmitter;
  }

  bool is_animating() { return false; }

  // state callback
  bool has_state_callback() { return false; }
  void add_on_state_callback(std::function<void()>&& callback) {}

 private:
  const char* const TAG = "homething.app.cattoy";

  remote_transmitter::RemoteTransmitterComponent* remote_transmitter_;
};
}  // namespace homething_cattoy_app
}  // namespace esphome
