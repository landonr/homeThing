#include "homeThingCatToyApp.h"

namespace esphome {
namespace homething_cattoy_app {

void HomeThingCatToyApp::rootMenuTitles(
    std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {
  menu_titles->push_back(new homething_menu_base::MenuTitleBase(
      "Cat toy", "", homething_menu_base::ArrowMenuTitleRightIcon));
}
void HomeThingCatToyApp::app_menu_titles(
    std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {}

// menu screens
homething_menu_app::NavigationCoordination HomeThingCatToyApp::app_menu_select(
    int index) {

  auto call = remote_transmitter_->transmit();
  esphome::remote_base::ProntoData data = {
      "0000 006D 0022 0000 0143 00A0 0015 0014 0015 0014 0015 0014 0015 0014 "
      "0015 0014 0015 0014 0015 0014 0015 0014 0015 003A 0015 003A 0015 003A "
      "0015 003A 0015 003A 0015 003A 0015 003A 0015 003A 0015 0014 0015 0014 "
      "0015 0014 0015 0014 0015 0014 0015 0014 0015 0014 0015 0014 0015 003A "
      "0015 003A 0015 003A 0015 003A 0015 003A 0015 003A 0015 003A 0015 003A "
      "0015 06C3"};
  esphome::remote_base::ProntoProtocol().encode(call.get_data(), data);
  call.set_send_times(1);
  call.perform();
  return homething_menu_app::NavigationCoordination::
      NavigationCoordinationReturn;
}
bool HomeThingCatToyApp::should_draw_app() {
  return true;
}
void HomeThingCatToyApp::draw_app(
    int menuIndex,
    const std::vector<homething_menu_base::MenuTitleBase*>* active_menu) {
  if (display_ == nullptr) {
    return;
  }
  auto largeFont = display_state_->get_font_large();
  auto primaryTextColor =
      display_state_->get_color_palette()->get_accent_primary();

  auto color = display_state_->get_color_palette()->get_pink();
  int yPos = display_->get_height() * 0.1;
  display_->printf(display_->get_width() * 0.5, yPos, largeFont, color,
                   display::TextAlign::TOP_CENTER, "Cat Toy!");

  color = display_state_->get_color_palette()->get_red();
  display_->printf((display_->get_width() * 0.5) + 1, yPos + 1, largeFont,
                   color, display::TextAlign::TOP_CENTER, "Cat Toy!");

  yPos = display_->get_height() * 0.3;
  display_->printf(display_->get_width() * 0.5, yPos, largeFont,
                   primaryTextColor, display::TextAlign::TOP_CENTER,
                   "Scroll Right");

  yPos = display_->get_height() * 0.4;
  display_->printf(display_->get_width() * 0.5, yPos, largeFont,
                   primaryTextColor, display::TextAlign::TOP_CENTER, "Drive");

  yPos = display_->get_height() * 0.6;
  display_->printf(display_->get_width() * 0.5, yPos, largeFont,
                   primaryTextColor, display::TextAlign::TOP_CENTER,
                   "Scroll Left");

  yPos = display_->get_height() * 0.7;
  display_->printf(display_->get_width() * 0.5, yPos, largeFont,
                   primaryTextColor, display::TextAlign::TOP_CENTER, "Reverse");
}

bool HomeThingCatToyApp::idleTick(int idleTime, int display_timeout) {
  return false;
}
int HomeThingCatToyApp::root_menu_size() {
  return 1;
}
void HomeThingCatToyApp::reset_menu() {}
void HomeThingCatToyApp::set_app_menu_index(int app_menu_index) {}

// buttons
homething_menu_app::NavigationCoordination
HomeThingCatToyApp::rotaryScrollClockwise(int rotary) {
  auto call = remote_transmitter_->transmit();
  esphome::remote_base::ProntoData data = {
      "0000 006D 0022 0000 0143 00A0 0015 0014 0015 0014 0015 0014 0015 0014 "
      "0015 0014 0015 0014 0015 0014 0015 0014 0015 003A 0015 003A 0015 003A "
      "0015 003A 0015 003A 0015 003A 0015 003A 0015 003A 0015 0014 0015 0014 "
      "0015 0014 0015 0014 0015 0014 0015 0014 0015 0014 0015 0014 0015 003A "
      "0015 003A 0015 003A 0015 003A 0015 003A 0015 003A 0015 003A 0015 003A "
      "0015 06C3"};
  esphome::remote_base::ProntoProtocol().encode(call.get_data(), data);
  call.set_send_times(1);
  call.perform();
  return homething_menu_app::NavigationCoordination::
      NavigationCoordinationReturn;
}

homething_menu_app::NavigationCoordination
HomeThingCatToyApp::rotaryScrollCounterClockwise(int rotary) {
  auto call = remote_transmitter_->transmit();
  esphome::remote_base::ProntoData data = {
      "0000 006D 0022 0000 0145 009D 0017 0011 0018 0011 0018 0011 0018 0011 "
      "0017 0011 0018 0011 0018 0011 0018 0011 0017 0037 0018 0037 0017 0037 "
      "0017 0037 0017 0037 0017 0037 0017 0037 0017 0037 0017 0011 0018 0011 "
      "0017 0011 0017 0011 0017 0037 0017 0037 0017 0011 0017 0011 0017 0037 "
      "0017 0037 0017 0037 0017 0037 0017 0011 0018 0011 0017 0037 0018 0036 "
      "0019 06C3"};
  esphome::remote_base::ProntoProtocol().encode(call.get_data(), data);
  call.set_send_times(1);
  call.perform();
  return homething_menu_app::NavigationCoordination::
      NavigationCoordinationReturn;
}

homething_menu_app::NavigationCoordination HomeThingCatToyApp::buttonPressUp() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationPop;
}

homething_menu_app::NavigationCoordination
HomeThingCatToyApp::buttonPressDown() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingCatToyApp::buttonPressLeft() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingCatToyApp::buttonPressRight() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingCatToyApp::buttonPressSelect(int menuIndex) {
  auto call = remote_transmitter_->transmit();
  esphome::remote_base::ProntoData data = {
      "0000 006D 0022 0000 0143 00A0 0015 0014 0015 0014 0015 0014 0015 0014 "
      "0015 0014 0015 0014 0015 0014 0015 0014 0015 003A 0015 003A 0015 003A "
      "0015 003A 0015 003A 0015 003A 0015 003A 0015 003A 0015 0014 0015 0014 "
      "0015 0014 0015 0014 0015 0014 0015 0014 0015 0014 0015 0014 0015 003A "
      "0015 003A 0015 003A 0015 003A 0015 003A 0015 003A 0015 003A 0015 003A "
      "0015 06C3"};
  esphome::remote_base::ProntoProtocol().encode(call.get_data(), data);
  call.set_send_times(1);
  call.perform();
  return homething_menu_app::NavigationCoordination::
      NavigationCoordinationReturn;
}

homething_menu_app::NavigationCoordination
HomeThingCatToyApp::buttonPressOption() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

}  // namespace homething_cattoy_app
}  // namespace esphome
