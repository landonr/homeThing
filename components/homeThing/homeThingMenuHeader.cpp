#include "homeThingMenuHeader.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_menu_base {
int HomeThingMenuHeader::getHeaderTextYPos(int yPosOffset) {
  return ((display_state_->get_header_height() -
           display_state_->get_small_font()->get_baseline() * 1.2) /
          2) -
         yPosOffset;
}

void HomeThingMenuHeader::drawHeaderTitleWithString(std::string title, int xPos,
                                                    int yPosOffset) {
  int yPos = getHeaderTextYPos(yPosOffset);
  display_buffer_->printf(
      xPos, yPos, display_state_->get_small_font(),
      text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
      title.c_str());
}

int HomeThingMenuHeader::drawHeaderIcon(std::string title, int xPos,
                                        Color iconColor) {
  int yPos = getHeaderTextYPos(0);
  display_buffer_->printf(xPos, yPos, display_state_->get_material_font_small(),
                          iconColor, title.c_str());
  return xPos + display_state_->get_icon_size() +
         display_state_->get_margin_size() / 2;
}

int HomeThingMenuHeader::drawPlayPauseIcon(int oldXPos,
                                           MenuTitlePlayer menuTitle) {
  int yPos = getHeaderTextYPos(0);
  int xPos = oldXPos;
  switch (menuTitle.media_player_->playerState) {
    case homeassistant_media_player::RemotePlayerState::
        PlayingRemotePlayerState: {
      display_buffer_->printf(
          xPos, yPos, display_state_->get_material_font_small(),
          menuTitle.mediaSourceIconColor(display_state_->color_accent_primary_),
          menuTitle.mediaSourceIcon().c_str());
      break;
    }
    case homeassistant_media_player::RemotePlayerState::PausedRemotePlayerState:
      display_buffer_->printf(xPos, yPos,
                              display_state_->get_material_font_small(),
                              display_state_->color_accent_primary_, "󰏤");
      break;
    case homeassistant_media_player::RemotePlayerState::
        StoppedRemotePlayerState:
      display_buffer_->printf(xPos, yPos,
                              display_state_->get_material_font_small(),
                              display_state_->color_accent_primary_, "󰓛");
      break;
    case homeassistant_media_player::RemotePlayerState::
        PowerOffRemotePlayerState:
      display_buffer_->printf(xPos, yPos,
                              display_state_->get_material_font_small(),
                              display_state_->color_accent_primary_, "󰽥");
      break;
    default:
      return oldXPos;
  }
  return xPos + display_state_->get_icon_size() +
         display_state_->get_margin_size() / 2;
}

void HomeThingMenuHeader::drawHeaderTitle(int yPosOffset,
                                          const MenuStates activeMenuState) {
  int xPos = 2;
  switch (activeMenuState) {
    case rootMenu:
    case backlightMenu:
    case sleepMenu:
    case nowPlayingMenu: {
      if (speaker_group_ != NULL) {
        auto headerMenuTitle =
            headerMediaPlayerTitle(speaker_group_->activePlayer);
        xPos = drawPlayPauseIcon(xPos, headerMenuTitle);
        drawHeaderTitleWithString((headerMenuTitle).get_name(), xPos);
      } else {
        drawHeaderTitleWithString("Remote", xPos);
      }
      break;
    }
    case sourcesMenu:
      drawHeaderTitleWithString("Sources", xPos);
      break;
    case groupMenu:
      drawHeaderTitleWithString("Group Speakers", xPos);
      break;
    case mediaPlayersMenu:
      drawHeaderTitleWithString("Media Players", xPos);
      break;
    case scenesMenu:
      drawHeaderTitleWithString("Scenes/Actions", xPos);
      break;
    case lightsMenu:
      drawHeaderTitleWithString("Lights", xPos);
      break;
    case lightsDetailMenu: {
      if (light_group_->getActiveLight() != NULL) {
        auto activeLight = light_group_->getActiveLight();
        auto headerMenuTitle = activeLight->get_name();
        int newXPos = drawHeaderIcon(activeLight->icon(), xPos,
                                     activeLight->rgbLightColor());
        drawHeaderTitleWithString(headerMenuTitle, newXPos + 1);
      } else {
        drawHeaderTitleWithString("LightDetail", xPos);
      }
      break;
    }
    case switchesMenu:
      drawHeaderTitleWithString("Switches", xPos);
      break;
    case sensorsMenu:
      drawHeaderTitleWithString("Sensors", xPos);
      break;
    case bootMenu:
      drawHeaderTitleWithString(display_state_->get_boot_device_name(), xPos,
                                yPosOffset);
      break;
  }
}

int HomeThingMenuHeader::drawShuffle(int oldXPos, int yPosOffset) {
  if (speaker_group_ == NULL || speaker_group_->activePlayer == NULL) {
    return oldXPos;
  }
  if (speaker_group_->activePlayer->get_player_type() ==
      homeassistant_media_player::RemotePlayerType::TVRemotePlayerType) {
    return oldXPos;
  }
  if (speaker_group_->activePlayer->playerState !=
      homeassistant_media_player::RemotePlayerState::StoppedRemotePlayerState) {
    int xPos = oldXPos - display_state_->get_icon_size() +
               display_state_->get_margin_size() / 2;
    int yPos = getHeaderTextYPos(yPosOffset);
    if (speaker_group_->mediaShuffling()) {
      display_buffer_->printf(xPos, yPos,
                              display_state_->get_material_font_small(),
                              display_state_->color_accent_primary_, "󰒝");
    } else if (display_state_->get_draw_shuffle_disabled()) {
      display_buffer_->printf(xPos, yPos,
                              display_state_->get_material_font_small(),
                              display_state_->color_accent_primary_, "󰒞");
    } else {
      return oldXPos;
    }
    return xPos - display_state_->get_margin_size() / 2;
  }
  return oldXPos;
}

int HomeThingMenuHeader::drawHeaderTime(int oldXPos, int yPosOffset) {
  if (!display_state_->get_draw_header_time() || !esp_time_->now().is_valid()) {
    return oldXPos;
  }
  // switch (activeMenuState) {
  //   case rootMenu:
  //   case nowPlayingMenu:
  //     break;
  //   default:
  //     if (display_buffer_->get_width() < 200) {
  //       return oldXPos;
  //     }
  //     break;
  // }
  int yPos = getHeaderTextYPos(yPosOffset);
  std::string timeString = esp_time_->now().strftime("%I:%M%P");
  if (timeString.length() > 0 && timeString[0] == '0') {
    timeString.erase(0, 1);
  }
  int xPos = oldXPos - text_helpers_->getTextWidth(
                           display_state_->get_small_font()->get_baseline(),
                           timeString.length());
  display_buffer_->printf(
      xPos, yPos, display_state_->get_small_font(),
      text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
      timeString.c_str());
  return xPos - display_state_->get_margin_size() / 2;
}

int HomeThingMenuHeader::drawBattery(int oldXPos, int yPosOffset) {
  if (!display_state_->get_draw_battery_level()) {
    return oldXPos;
  }
  int batteryWidth = 24;
  int batteryHeight = display_state_->get_header_height() - 5;
  int yPos = 2 - yPosOffset;
  int capHeight = 6;
  int capWidth = 3;
  int xPos = oldXPos - batteryWidth;
  display_buffer_->rectangle(xPos, yPos, batteryWidth, batteryHeight,
                             display_state_->color_gray_dark_);
  display_buffer_->rectangle(
      xPos + batteryWidth - 1, yPos + (batteryHeight / 2) - (capHeight / 2),
      capWidth, capHeight, display_state_->color_gray_dark_);
  if (get_battery_percent() <= 100 && !get_charging()) {
    display_buffer_->filled_rectangle(
        xPos + 1, yPos + 1, (batteryWidth * get_battery_percent() * 0.01) - 2,
        batteryHeight - 2, display_state_->color_green_);
  } else {
    display_buffer_->filled_rectangle(xPos + 1, yPos + 1, batteryWidth - 2,
                                      batteryHeight - 2,
                                      display_state_->color_yellow_);
  }
  return xPos - display_state_->get_margin_size();
}

int HomeThingMenuHeader::drawHeaderVolumeLevel(int oldXPos, int yPosOffset) {
  if (speaker_group_ == nullptr || speaker_group_->activePlayer == nullptr) {
    return oldXPos;
  }
  if (!display_state_->get_draw_volume_level()) {
    return oldXPos;
  }
  int xPos = oldXPos - display_state_->get_margin_size() / 2;
  int yPos = getHeaderTextYPos(yPosOffset);
  display_buffer_->printf(
      xPos, yPos, display_state_->get_small_font(),
      text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
      display::TextAlign::TOP_RIGHT, "%.0f%%",
      speaker_group_->getVolumeLevel());
  return xPos;
}

void HomeThingMenuHeader::drawHeader(int yPosOffset,
                                     const MenuStates activeMenuState) {
  display_buffer_->rectangle(
      0, display_state_->get_header_height() - yPosOffset,
      display_buffer_->get_width(), 1, display_state_->color_accent_primary_);
  drawHeaderTitle(yPosOffset, activeMenuState);
  int xPos =
      display_buffer_->get_width() - display_state_->get_margin_size() / 2;
  drawHeaderVolumeLevel(
      drawHeaderTime(drawShuffle(drawBattery(xPos, yPosOffset), yPosOffset),
                     yPosOffset),
      yPosOffset);
}
}  // namespace homething_menu_base
}  // namespace esphome
