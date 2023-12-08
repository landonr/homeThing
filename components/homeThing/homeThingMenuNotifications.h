#pragma once

#include <string>
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/homeThingDisplayState/homeThingDisplayState.h"
#include "esphome/components/homeThingDisplayState/homeThingMenuTextHelpers.h"

namespace esphome {
namespace homething_menu_base {

struct Notification {
  std::string title;
  std::string subtitle;
  std::string text;
  bool autoClear;
  int time_added_seconds;
};

class HomeThingMenuNotifications {
 public:
  HomeThingMenuNotifications(
      display::DisplayBuffer* new_display_buffer,
      homething_display_state::HomeThingDisplayState* new_display_state)
      : display_buffer_(new_display_buffer),
        display_state_(new_display_state) {}
  void drawNotifications();
  int drawNotification(const Notification notification, int yPos);
  void addNotification(const std::string& title, const std::string& subtitle,
                       const std::string& text, bool autoClear);
  bool clearNotifications();
  // Add any other necessary functions or members here

 private:
  display::DisplayBuffer* display_buffer_{nullptr};
  homething_display_state::HomeThingDisplayState* display_state_{nullptr};
  std::vector<Notification> notifications_;
  int notification_time_ = 0;
  // Add any necessary private members or helper functions here
};

}  // namespace homething_menu_base
}  // namespace esphome
