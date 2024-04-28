#include "homeThingMenuNotifications.h"

namespace esphome {
namespace homething_menu_base {

void HomeThingMenuNotifications::drawNotifications() {
  int yPos = 10;          // Starting Y position for the first notification
  const int padding = 8;  // Vertical padding between notifications
  const int MAX_NOTIFICATION_TIME = 5;  // Maximum time in seconds

  for (auto it = notifications_.begin(); it != notifications_.end();) {
    yPos = drawNotification(*it, yPos);

    it->time_added_seconds++;  // Increment time_added for each drawn frame

    if (!it->persistent && it->time_added_seconds > MAX_NOTIFICATION_TIME) {
      it = notifications_.erase(
          it);  // Remove notification if time exceeds limit
    } else {
      yPos += padding;  // Add padding for the next notification
      ++it;
    }
  }
}

int HomeThingMenuNotifications::drawNotification(
    const Notification notification, int yPos) {
  int borderSize = 2;
  int padding = display_state_->get_margin_size();
  int maxLines = 4;  // Maximum lines allowed
  int rectWidth = display_buffer_->get_width() - (2 * borderSize);
  int font_small_height = display_state_->get_font_small()->get_height();
  int rectHeight = (maxLines - 1) * font_small_height + padding;
  int headerHeight = display_state_->get_font_medium()->get_height() + padding;
  rectHeight += headerHeight + padding;
  int rectXPos = (display_buffer_->get_width() - rectWidth) / 2;

  // Draw outer rectangle
  display_buffer_->filled_rectangle(
      rectXPos - borderSize, yPos - borderSize, rectWidth + (2 * borderSize),
      rectHeight + (2 * borderSize),
      display_state_->get_color_palette()->get_gray_dark_2());

  // Draw inner filled rectangle
  display_buffer_->filled_rectangle(
      rectXPos, yPos, rectWidth, rectHeight,
      display_state_->get_color_palette()->get_accent_primary());

  Color mixedColor = display_state_->get_color_palette()->mixColors(
      display_state_->get_color_palette()->get_white(),
      display_state_->get_color_palette()->get_accent_primary(), 0.85f);

  // Draw header filled rectangle
  display_buffer_->filled_rectangle(rectXPos, yPos, rectWidth, headerHeight,
                                    mixedColor);

  // Wrap and draw text
  int textXPos = rectXPos + borderSize;
  int textYPos = yPos + borderSize;

  display_state_->drawTextWrapped(
      textXPos, textYPos, display_state_->get_font_medium(),
      display_state_->primaryTextColor(), display::TextAlign::LEFT,
      notification.title, 1, display_buffer_);

  textYPos += headerHeight;

  if (!notification.subtitle.empty()) {
    // textYPos += display_state_->get_font_small()->get_height() + padding;
    // display_buffer_->printf(textXPos, textYPos,
    //                         display_state_->get_font_small(),
    //                         display_state_->primaryTextColor(),
    //                         display::TextAlign::LEFT, notification.subtitle.c_str());
    textYPos = display_state_->drawTextWrapped(
        textXPos, textYPos, display_state_->get_font_small(),
        display_state_->primaryTextColor(), display::TextAlign::LEFT,
        notification.subtitle, maxLines, display_buffer_);
  }

  if (!notification.message.empty()) {
    display_state_->drawTextWrapped(
        textXPos, textYPos, display_state_->get_font_small(),
        display_state_->primaryTextColor(), display::TextAlign::LEFT,
        notification.message, maxLines, display_buffer_);
  }

  return yPos + rectHeight;
}

void HomeThingMenuNotifications::addNotification(const std::string& title,
                                                 const std::string& subtitle,
                                                 const std::string& message,
                                                 bool persistent) {

  Notification newNotification = {title, subtitle, message, persistent, 0};
  notifications_.erase(
      std::remove_if(notifications_.begin(), notifications_.end(),
                     [](const Notification& notification) {
                       return !notification.persistent;
                     }),
      notifications_.end());

  notifications_.push_back(newNotification);
}

void HomeThingMenuNotifications::clearNotifications() {
  if (notifications_.empty()) {
    return;
  }
  notifications_.clear();
}

}  // namespace homething_menu_base
}  // namespace esphome
