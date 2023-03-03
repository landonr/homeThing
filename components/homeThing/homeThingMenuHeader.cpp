#include "homeThingMenuHeader.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_menu_base {
int HomeThingMenuHeader::getHeaderTextYPos(int yPosOffset) {
  return ((display_state_->header_height_ -
           display_state_->get_small_font()->get_height() * 1.2) /
          2) -
         yPosOffset;
}

void HomeThingMenuHeader::drawHeaderTitleWithString(std::string title, int xPos,
                                                    int yPosOffset) {
  int yPos = getHeaderTextYPos(yPosOffset);
  display_buffer_->printf(
      xPos, yPos, display_state_->get_small_font(),
      text_helpers_->primaryTextColor(display_state_->dark_mode_),
      title.c_str());
}

int HomeThingMenuHeader::drawHeaderIcon(std::string title, int xPos,
                                        Color iconColor) {
  int yPos = getHeaderTextYPos(0);
  display_buffer_->printf(xPos, yPos, display_state_->get_material_font_small(),
                          iconColor, title.c_str());
  return xPos + display_state_->icon_size_ + display_state_->margin_size_ / 2;
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
  return xPos + display_state_->icon_size_ + display_state_->margin_size_ / 2;
}

void HomeThingMenuHeader::drawHeaderTitle(int yPosOffset,
                                          const MenuStates activeMenuState) {
  int xPos = 2;
  switch (activeMenuState) {
    case rootMenu:
    case backlightMenu:
    case sleepMenu:
    case nowPlayingMenu: {
      if (speakerGroup != NULL) {
        auto headerMenuTitle =
            headerMediaPlayerTitle(speakerGroup->activePlayer);
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
      if (lightGroup->getActiveLight() != NULL) {
        auto activeLight = lightGroup->getActiveLight();
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
      drawHeaderTitleWithString(display_state_->boot_device_name_, xPos,
                                yPosOffset);
      break;
  }
}

int HomeThingMenuHeader::drawShuffle(int oldXPos, int yPosOffset) {
  if (speakerGroup == NULL || speakerGroup->activePlayer == NULL) {
    return oldXPos;
  }
  if (speakerGroup->activePlayer->get_player_type() ==
      homeassistant_media_player::RemotePlayerType::TVRemotePlayerType) {
    return oldXPos;
  }
  if (speakerGroup->activePlayer->playerState !=
      homeassistant_media_player::RemotePlayerState::StoppedRemotePlayerState) {
    int xPos =
        oldXPos - display_state_->icon_size_ + display_state_->margin_size_ / 2;
    int yPos = getHeaderTextYPos(yPosOffset);
    if (speakerGroup->mediaShuffling()) {
      display_buffer_->printf(xPos, yPos,
                              display_state_->get_material_font_small(),
                              display_state_->color_accent_primary_, "󰒝");
    } else if (display_state_->draw_shuffle_disabled_) {
      display_buffer_->printf(xPos, yPos,
                              display_state_->get_material_font_small(),
                              display_state_->color_accent_primary_, "󰒞");
    } else {
      return oldXPos;
    }
    return xPos - display_state_->margin_size_ / 2;
  }
  return oldXPos;
}

int HomeThingMenuHeader::drawHeaderTime(int oldXPos, int yPosOffset) {
  if (!display_state_->draw_header_time_ || !esp_time_->now().is_valid()) {
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
                           display_state_->get_small_font()->get_height(),
                           timeString.length());
  display_buffer_->printf(
      xPos, yPos, display_state_->get_small_font(),
      text_helpers_->primaryTextColor(display_state_->dark_mode_),
      timeString.c_str());
  return xPos - display_state_->margin_size_ / 2;
}

int HomeThingMenuHeader::drawBattery(int oldXPos, int yPosOffset) {
  if (!display_state_->draw_battery_level_) {
    return oldXPos;
  }
  int batteryWidth = 24;
  int batteryHeight = display_state_->header_height_ - 5;
  int yPos = 2 - yPosOffset;
  int capHeight = 6;
  int capWidth = 3;
  int xPos = oldXPos - batteryWidth;
  display_buffer_->rectangle(xPos, yPos, batteryWidth, batteryHeight,
                             display_state_->color_gray_dark_);
  display_buffer_->rectangle(
      xPos + batteryWidth - 1, yPos + (batteryHeight / 2) - (capHeight / 2),
      capWidth, capHeight, display_state_->color_gray_dark_);
  if (battery_percent_->state <= 100 && !charging) {
    display_buffer_->filled_rectangle(
        xPos + 1, yPos + 1, (batteryWidth * battery_percent_->state * 0.01) - 2,
        batteryHeight - 2, display_state_->color_green_);
  } else {
    display_buffer_->filled_rectangle(xPos + 1, yPos + 1, batteryWidth - 2,
                                      batteryHeight - 2,
                                      display_state_->color_yellow_);
  }
  return xPos - display_state_->margin_size_;
}

int HomeThingMenuHeader::drawHeaderVolumeLevel(int oldXPos, int yPosOffset) {
  if (speakerGroup == nullptr || speakerGroup->activePlayer == nullptr) {
    return oldXPos;
  }
  if (!display_state_->draw_volume_level_) {
    return oldXPos;
  }
  int xPos = oldXPos - display_state_->margin_size_ / 2;
  int yPos = getHeaderTextYPos(yPosOffset);
  display_buffer_->printf(
      xPos, yPos, display_state_->get_small_font(),
      text_helpers_->primaryTextColor(display_state_->dark_mode_),
      display::TextAlign::TOP_RIGHT, "%.0f%%", speakerGroup->getVolumeLevel());
  return xPos;
}

void HomeThingMenuHeader::drawHeader(int yPosOffset,
                                     const MenuStates activeMenuState) {
  display_buffer_->rectangle(0, display_state_->header_height_ - yPosOffset,
                             display_buffer_->get_width(), 1,
                             display_state_->color_accent_primary_);
  drawHeaderTitle(yPosOffset, activeMenuState);
  int xPos = display_buffer_->get_width() - display_state_->margin_size_ / 2;
  drawHeaderVolumeLevel(
      drawHeaderTime(drawShuffle(drawBattery(xPos, yPosOffset), yPosOffset),
                     yPosOffset),
      yPosOffset);
}
}  // namespace homething_menu_base
}  // namespace esphome
