#pragma once

#include <string>
#include "esphome/components/homeThing/homeThingMenuDisplayState.h"
#include "esphome/components/homeThing/homeThingMenuScreen.h"
#include "esphome/components/homeThing/homeThingMenuTextHelpers.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"

#ifdef USE_MEDIA_PLAYER_GROUP
#include "esphome/components/homeThing/homeThingOptionMenu.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"
#endif

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/time/real_time_clock.h"
#include "esphome/core/time.h"

namespace esphome {
namespace homething_menu_base {

class HomeThingMenuHeaderSource {
 public:
  virtual std::string get_header_title() { return "xx"; }
  virtual int draw_header_details(
      int xPos, int yPosOffset, display::DisplayBuffer* display_buffer,
      homething_menu_base::HomeThingMenuDisplayState* display_state,
      homething_menu_base::HomeThingMenuTextHelpers* text_helpers) {
    return 0;
  }
};

class HomeThingMenuHeader {
 public:
  HomeThingMenuHeader(display::DisplayBuffer* new_display_buffer,
                      HomeThingMenuDisplayState* new_display_state,
                      HomeThingMenuTextHelpers* new_text_helpers)
      : display_buffer_(new_display_buffer),
        display_state_(new_display_state),
        text_helpers_(new_text_helpers) {}
  void drawHeader(int yPosOffset, const MenuStates activeMenuState);
  void draw_menu_header(HomeThingMenuHeaderSource* header_source);
  void set_battery_percent(sensor::Sensor* battery_percent) {
    battery_percent_ = battery_percent;
  }
  void set_charging(binary_sensor::BinarySensor* charging) {
    charging_ = charging;
  }
  void set_active_menu_screen(HomeThingMenuScreen** active_menu_screen) {
    active_menu_screen_ = active_menu_screen;
  }
  void set_time_id(time::RealTimeClock* time_id) { this->esp_time_ = time_id; }

 private:
  float get_battery_percent() {
    if (battery_percent_ != nullptr && battery_percent_->has_state()) {
      return battery_percent_->state;
    } else {
      return 0;
    }
  }
  bool get_charging() {
    if (charging_ != nullptr && charging_->has_state()) {
      return charging_->state;
    } else {
      return false;
    }
  }
  void drawHeaderTitleWithString(std::string title, int xPos,
                                 int yPosOffset = 0);
  void drawHeaderTitle(int yPosOffset, const MenuStates activeMenuState);
  int getHeaderTextYPos(int yPosOffset);
  int drawBattery(int oldXPos, int yPosOffset);
  int drawHeaderIcon(std::string title, int xPos, Color iconColor);
  int drawHeaderTime(int oldXPos, int yPosOffset);

  display::DisplayBuffer* display_buffer_{nullptr};
  HomeThingMenuDisplayState* display_state_{nullptr};
  HomeThingMenuTextHelpers* text_helpers_{nullptr};
  sensor::Sensor* battery_percent_{nullptr};
  binary_sensor::BinarySensor* charging_{nullptr};
  time::RealTimeClock* esp_time_{nullptr};
  const char* const TAG = "homething.menu.header";
  HomeThingMenuScreen** active_menu_screen_{nullptr};
};

}  // namespace homething_menu_base
}  // namespace esphome
