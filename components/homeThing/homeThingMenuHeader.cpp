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
  display_buffer_->printf(
      xPos, yPos, display_state_->get_font_small(),
      text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
      title.c_str());
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
    case nowPlayingMenu: {
#ifdef USE_MEDIA_PLAYER_GROUP
      if (media_player_group_ && media_player_group_->active_player_) {
        auto headerMenuTitle =
            headerMediaPlayerTitle(media_player_group_->active_player_);
        xPos = drawPlayPauseIcon(xPos, headerMenuTitle);
        drawHeaderTitleWithString((headerMenuTitle).get_name(), xPos);
      } else {
        drawHeaderTitleWithString("Remote", xPos);
      }
#endif
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
    case bootMenu:
      drawHeaderTitleWithString(display_state_->get_boot_device_name(), xPos,
                                yPosOffset);
      break;
  }
}

#ifdef USE_MEDIA_PLAYER_GROUP
int HomeThingMenuHeader::drawPlayPauseIcon(int oldXPos,
                                           MenuTitlePlayer menuTitle) {
  int yPos = getHeaderTextYPos(0);
  int xPos = oldXPos;
  switch (menuTitle.media_player_->playerState) {
    case homeassistant_media_player::RemotePlayerState::
        PlayingRemotePlayerState: {
      display_buffer_->printf(
          xPos, yPos, display_state_->get_font_material_small(),
          menuTitle.mediaSourceIconColor(
              display_state_->get_color_palette()->get_accent_primary()),
          menuTitle.mediaSourceIcon().c_str());
      break;
    }
    case homeassistant_media_player::RemotePlayerState::PausedRemotePlayerState:
      display_buffer_->printf(
          xPos, yPos, display_state_->get_font_material_small(),
          display_state_->get_color_palette()->get_accent_primary(), "󰏤");
      break;
    case homeassistant_media_player::RemotePlayerState::
        StoppedRemotePlayerState:
      display_buffer_->printf(
          xPos, yPos, display_state_->get_font_material_small(),
          display_state_->get_color_palette()->get_accent_primary(), "󰓛");
      break;
    case homeassistant_media_player::RemotePlayerState::
        PowerOffRemotePlayerState:
      display_buffer_->printf(
          xPos, yPos, display_state_->get_font_material_small(),
          display_state_->get_color_palette()->get_accent_primary(), "󰽥");
      break;
    default:
      return oldXPos;
  }
  return xPos + display_state_->get_icon_size() +
         display_state_->get_margin_size() / 2;
}

int HomeThingMenuHeader::drawShuffle(int oldXPos, int yPosOffset) {
  if (!media_player_group_ || media_player_group_->active_player_ == NULL ||
      display_state_->get_draw_shuffle() == DisplayIconEnabledState::OFF) {
    return oldXPos;
  }
  auto active_player = media_player_group_->active_player_;
  if (active_player->get_player_type() ==
          homeassistant_media_player::RemotePlayerType::TVRemotePlayerType ||
      !active_player->supports(homeassistant_media_player::
                                   MediaPlayerSupportedFeature::SHUFFLE_SET)) {
    return oldXPos;
  }
  if (active_player->playerState !=
      homeassistant_media_player::RemotePlayerState::StoppedRemotePlayerState) {
    int xPos = oldXPos - display_state_->get_icon_size() +
               display_state_->get_margin_size() / 2;
    int yPos = getHeaderTextYPos(yPosOffset);
    if (media_player_group_->mediaShuffling()) {
      display_buffer_->printf(
          xPos, yPos, display_state_->get_font_material_small(),
          display_state_->get_color_palette()->get_accent_primary(), "󰒝");
    } else if (display_state_->get_draw_shuffle() ==
               DisplayIconEnabledState::ALWAYS) {
      display_buffer_->printf(
          xPos, yPos, display_state_->get_font_material_small(),
          display_state_->get_color_palette()->get_accent_primary(), "󰒞");
    } else {
      return oldXPos;
    }
    return xPos - display_state_->get_margin_size() / 2;
  }
  return oldXPos;
}

int HomeThingMenuHeader::drawRepeat(int oldXPos, int yPosOffset) {
  if (display_state_->get_draw_repeat() == DisplayIconEnabledState::OFF ||
      !media_player_group_ || media_player_group_->active_player_ == NULL) {
    return oldXPos;
  }
  auto active_player = media_player_group_->active_player_;
  if (active_player->get_player_type() ==
          homeassistant_media_player::RemotePlayerType::TVRemotePlayerType ||
      !active_player->supports(homeassistant_media_player::
                                   MediaPlayerSupportedFeature::REPEAT_SET) ||
      active_player->playerState ==
          homeassistant_media_player::RemotePlayerState::
              StoppedRemotePlayerState) {
    return oldXPos;
  }
  int xPos = oldXPos - display_state_->get_icon_size() +
             display_state_->get_margin_size() / 2;
  int yPos = getHeaderTextYPos(yPosOffset);
  switch (media_player_group_->get_repeat_mode()) {
    case homeassistant_media_player::MediaPlayerRepeatMode::ALL:
      display_buffer_->printf(
          xPos, yPos, display_state_->get_font_material_small(),
          display_state_->get_color_palette()->get_accent_primary(), "󰑖");
      break;
    case homeassistant_media_player::MediaPlayerRepeatMode::ONE:
      display_buffer_->printf(
          xPos, yPos, display_state_->get_font_material_small(),
          display_state_->get_color_palette()->get_accent_primary(), "󰑘");
      break;
    case homeassistant_media_player::MediaPlayerRepeatMode::OFF:
      if (display_state_->get_draw_repeat() !=
          DisplayIconEnabledState::ALWAYS) {
        return oldXPos;
      }
      display_buffer_->printf(
          xPos, yPos, display_state_->get_font_material_small(),
          display_state_->get_color_palette()->get_accent_primary(), "󰑗");
      break;
    default:
      return oldXPos;
  }
  return xPos - display_state_->get_margin_size() / 2;
}

int HomeThingMenuHeader::drawHeaderVolumeLevel(int oldXPos, int yPosOffset) {
  if (media_player_group_ == nullptr ||
      media_player_group_->active_player_ == nullptr) {
    return oldXPos;
  }
  if (!display_state_->get_draw_volume_level()) {
    return oldXPos;
  }
  int xPos = oldXPos - display_state_->get_margin_size() / 2;
  int yPos = getHeaderTextYPos(yPosOffset);
  display_buffer_->printf(
      xPos, yPos, display_state_->get_font_small(),
      text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
      display::TextAlign::TOP_RIGHT, "%.0f%%",
      media_player_group_->getVolumeLevel());
  return xPos;
}
#endif

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
                           display_state_->get_font_small()->get_baseline(),
                           timeString.length());
  display_buffer_->printf(
      xPos, yPos, display_state_->get_font_small(),
      text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
      timeString.c_str());
  return xPos - display_state_->get_margin_size() / 2;
}

int HomeThingMenuHeader::drawBattery(int oldXPos, int yPosOffset) {
  if (!display_state_->get_draw_battery_level()) {
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
#ifdef USE_MEDIA_PLAYER_GROUP
  xPos = drawRepeat(xPos, yPosOffset);
  xPos = drawShuffle(xPos, yPosOffset);
#endif
  xPos = drawHeaderTime(xPos, yPosOffset);
#ifdef USE_MEDIA_PLAYER_GROUP
  xPos = drawHeaderVolumeLevel(xPos, yPosOffset);
#endif
}
}  // namespace homething_menu_base
}  // namespace esphome
