#include "homeThingMenuHeader.h"
#ifdef USE_LIGHT
#include "esphome/components/homeassistant_component/LightExtensions.h"
#endif

namespace esphome {
namespace homething_menu_base {
int HomeThingMenuHeader::getHeaderTextYPos(int yPosOffset) {
  return ((display_state_->get_header_height() -
           display_state_->get_font_small()->get_baseline() * 1.2) /
          2) -
         yPosOffset;
}

void HomeThingMenuHeader::drawHeaderTitleWithString(std::string title, int xPos,
                                                    int yPosOffset) {
  int yPos = getHeaderTextYPos(yPosOffset);
  display_buffer_->printf(xPos, yPos, display_state_->get_font_small(),
                          display_state_->primaryTextColor(), title.c_str());
}

int HomeThingMenuHeader::drawHeaderIcon(std::string title, int xPos,
                                        Color iconColor) {
  int yPos = getHeaderTextYPos(0);
  display_buffer_->printf(xPos, yPos, display_state_->get_font_material_small(),
                          iconColor, title.c_str());
  return xPos + display_state_->get_icon_size() +
         display_state_->get_margin_size() / 2;
}

void HomeThingMenuHeader::drawHeaderTitle(int yPosOffset,
                                          const MenuStates activeMenuState) {
  int xPos = 2;
  switch (activeMenuState) {
    case rootMenu:
    case settingsMenu:
      drawHeaderTitleWithString("Remote", xPos);
      break;
    case appMenu:
      break;
    case lightsDetailMenu: {
#ifdef USE_LIGHT
      auto selectedEntity = (*active_menu_screen_)->get_selected_entity();
      if (selectedEntity != NULL &&
          std::get<0>(*selectedEntity) == MenuItemTypeLight) {
        auto activeLight =
            static_cast<light::LightState*>(std::get<1>(*selectedEntity));
        auto headerMenuTitle = activeLight->get_name();
        int newXPos =
            drawHeaderIcon(icon(activeLight), xPos, rgbLightColor(activeLight));
        drawHeaderTitleWithString(headerMenuTitle, newXPos + 1);
      } else {
        drawHeaderTitleWithString("LightDetail", xPos);
      }
#endif
      break;
    }
    case entityMenu:
      drawHeaderTitleWithString("Entity", xPos);
      break;
    case bootMenu:
      drawHeaderTitleWithString(display_state_->get_boot_device_name(), xPos,
                                yPosOffset);
      break;
  }
}

void HomeThingMenuHeader::draw_menu_header(
    HomeThingMenuHeaderSource* header_source) {
  int yPosOffset = 0;
  display_buffer_->rectangle(
      0, display_state_->get_header_height() - yPosOffset,
      display_buffer_->get_width(), 1,
      display_state_->get_color_palette()->get_accent_primary());
  int xPos = 2;
  if (header_source != nullptr) {
    drawHeaderTitleWithString(header_source->get_header_title(), xPos);
  } else {
    drawHeaderTitleWithString("AppSource!", xPos);
  }
  xPos = display_buffer_->get_width() - display_state_->get_margin_size() / 2;
  xPos = drawBattery(xPos, yPosOffset);

#ifdef USE_TIME
  xPos = drawHeaderTime(xPos, yPosOffset);
#endif

  if (header_source != nullptr) {
    xPos = header_source->draw_header_details(xPos, getHeaderTextYPos(0),
                                              display_buffer_, display_state_);
  }
}

#ifdef USE_TIME
int HomeThingMenuHeader::drawHeaderTime(int oldXPos, int yPosOffset) {
  if (esp_time_ == nullptr) {
    return oldXPos;
  }
  if (!display_state_->get_draw_header_time() || !esp_time_->now().is_valid()) {
    return oldXPos;
  }

  int yPos = getHeaderTextYPos(yPosOffset);
  std::string timeString = esp_time_->now().strftime("%I:%M%P");
  if (timeString.length() > 0 && timeString[0] == '0') {
    timeString.erase(0, 1);
  }
  int xPos = oldXPos - display_state_->getTextWidth(
                           display_state_->get_font_small()->get_baseline(),
                           timeString.length());
  display_buffer_->printf(xPos, yPos, display_state_->get_font_small(),
                          display_state_->primaryTextColor(),
                          timeString.c_str());
  return xPos - display_state_->get_margin_size() / 2;
}
#endif

int HomeThingMenuHeader::drawBattery(int oldXPos, int yPosOffset) {
  if (!display_state_->get_draw_battery_level() ||
      battery_percent_ == nullptr) {
    return oldXPos;
  }
  float batteryWidth = 24;
  int batteryHeight = display_state_->get_header_height() - 5;
  int yPos = 2 - yPosOffset;
  int capHeight = 6;
  int capWidth = 3;
  float xPos = oldXPos - batteryWidth;
  display_buffer_->rectangle(
      xPos, yPos, batteryWidth, batteryHeight,
      display_state_->get_color_palette()->get_gray_dark());
  display_buffer_->rectangle(
      xPos + batteryWidth - 1, yPos + (batteryHeight / 2) - (capHeight / 2),
      capWidth, capHeight,
      display_state_->get_color_palette()->get_gray_dark());
  if (get_battery_percent() <= 100 && !get_charging()) {
    display_buffer_->filled_rectangle(
        xPos + 1, yPos + 1, (batteryWidth * get_battery_percent() * 0.01) - 2,
        batteryHeight - 2, display_state_->get_color_palette()->get_green());
  } else if (get_charging()) {
    display_buffer_->filled_rectangle(
        xPos + 1, yPos + 1, batteryWidth - 2, batteryHeight - 2,
        display_state_->get_color_palette()->get_yellow());
  }
  return xPos - display_state_->get_margin_size();
}

void HomeThingMenuHeader::drawHeader(int yPosOffset,
                                     const MenuStates activeMenuState) {
  display_buffer_->rectangle(
      0, display_state_->get_header_height() - yPosOffset,
      display_buffer_->get_width(), 1,
      display_state_->get_color_palette()->get_accent_primary());
  drawHeaderTitle(yPosOffset, activeMenuState);
  int xPos =
      display_buffer_->get_width() - display_state_->get_margin_size() / 2;
  xPos = drawBattery(xPos, yPosOffset);
#ifdef USE_TIME
  xPos = drawHeaderTime(xPos, yPosOffset);
#endif
  // #ifdef USE_MEDIA_PLAYER_GROUP
  //   xPos = drawHeaderVolumeLevel(xPos, yPosOffset);
  // #endif
}
}  // namespace homething_menu_base
}  // namespace esphome
