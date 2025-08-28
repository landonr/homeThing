#ifdef USE_MEDIA_PLAYER_GROUP
#include "homeThingNowPlayingDisplay.h"
#include "esphome/components/homeThing/homeThingOptionMenu.h"
#include "esphome/components/homeThingDisplayState/homeThingDisplayState.h"
#include "esphome/components/homeThingDisplayState/homeThingMenuTextHelpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_menu_now_playing {

PositionCoordinate HomeThingMenuNowPlaying::get_coordinate(double radius,
                                                           double angle) {
  PositionCoordinate coordinate;
  coordinate.x = radius * cos(angle);
  coordinate.y = radius * sin(angle);
  return coordinate;
}

display::TextAlign HomeThingMenuNowPlaying::text_align_for_circle_position(
    CircleOptionMenuPosition position) {
  switch (position) {
    case RIGHT:
      return display::TextAlign::CENTER_LEFT;
      break;
    case BOTTOM:
      return display::TextAlign::BOTTOM_CENTER;
      break;
    case LEFT:
      return display::TextAlign::CENTER_RIGHT;
      break;
    case TOP:
      return display::TextAlign::TOP_CENTER;
      break;
    case CENTER:
      return display::TextAlign::CENTER;
      break;
  }
  return display::TextAlign::CENTER;
}

void HomeThingMenuNowPlaying::drawCircleOptionMenu(
    std::vector<CircleOptionMenuItem> supported_features) {
  int y_pos =
      (display_->get_height() - display_state_->get_header_height());
  int radius;
  if (display_->get_height() > display_->get_width()) {
    radius = display_->get_width() * 0.25;
  } else {
    radius = y_pos * 0.4;
  }
  y_pos = (y_pos * 0.5) + display_state_->get_header_height();
  display_->circle(display_->get_width() * 0.5, y_pos, radius,
                          display_state_->get_color_palette()->get_gray());

  for (auto& feature : supported_features) {
    auto element = feature.command->get_feature();
    double angle = feature.position * M_PI / 2.0;
    auto coordinate = get_coordinate(radius, angle);

    auto title = feature.command->get_title();
    if (element ==
        homeassistant_media_player::MediaPlayerSupportedFeature::POWER_SET) {
      if (media_player_group_->active_player_->playerState ==
          homeassistant_media_player::RemotePlayerState::
              PowerOffRemotePlayerState) {
        title = "Turn On";
      } else {
        title = "Turn Off";
      }
    }
    display::TextAlign text_alignment =
        text_align_for_circle_position(feature.position);
    if (feature.position == CENTER) {
      coordinate.x = 0;
      coordinate.y = 0;
    }

    ESP_LOGD(TAG, "drawCircleOptionMenu: %s: %f %f", title.c_str(),
             coordinate.x, coordinate.y);
    display_->printf(
        (display_->get_width() / 2) + coordinate.x, y_pos + coordinate.y,
        display_state_->get_font_small(), display_state_->primaryTextColor(),
        text_alignment, title.c_str());
  }
}

void HomeThingMenuNowPlaying::drawNowPlayingSelectMenu(
    const std::vector<homething_menu_base::MenuTitleBase*>* menu_titles,
    int menu_index) {
  int yPos = display_->get_height() - display_state_->get_margin_size() -
             display_state_->get_font_large()->get_baseline();
  auto menuTitlesSize = menu_titles->size();
  if (menuTitlesSize < 1 || menu_index >= menuTitlesSize) {
    return;
  }
  display_->printf(display_->get_width() * 0.5, yPos,
                          display_state_->get_font_large(),
                          display_state_->primaryTextColor(),
                          display::TextAlign::TOP_CENTER,
                          (*menu_titles)[menu_index]->get_name().c_str());
  if (menu_index + 1 < menuTitlesSize) {
    display_->printf(display_->get_width() * 0.85, yPos,
                            display_state_->get_font_small(),
                            display_state_->primaryTextColor(),
                            display::TextAlign::TOP_CENTER,
                            (*menu_titles)[menu_index + 1]->get_name().c_str());
  } else {
    display_->printf(
        display_->get_width() * 0.85, yPos,
        display_state_->get_font_small(), display_state_->primaryTextColor(),
        display::TextAlign::TOP_CENTER, (*menu_titles)[0]->get_name().c_str());
  }
  if (menu_index - 1 >= 0) {
    display_->printf(display_->get_width() * 0.15, yPos,
                            display_state_->get_font_small(),
                            display_state_->primaryTextColor(),
                            display::TextAlign::TOP_CENTER,
                            (*menu_titles)[menu_index - 1]->get_name().c_str());
  } else {
    display_->printf(
        display_->get_width() * 0.15, yPos,
        display_state_->get_font_small(), display_state_->primaryTextColor(),
        display::TextAlign::TOP_CENTER,
        (*menu_titles)[menuTitlesSize - 1]->get_name().c_str());
  }
}

void HomeThingMenuNowPlaying::drawNowPlaying(
    int menuIndex, HomeThingOptionMenu* option_menu,
    const std::vector<homething_menu_base::MenuTitleBase*>* active_menu) {
  if (drawOptionMenuAndStop(option_menu)) {
    ESP_LOGI(TAG, "drawNowPlaying: drawOptionMenuAndStop");
    return;
  }
  if (display_state_ == nullptr) {
    ESP_LOGI(TAG, "drawNowPlaying: display state null");
    return;
  }
  if (active_menu && active_menu->size() > 0 && draw_bottom_menu_) {
    drawNowPlayingSelectMenu(active_menu, menuIndex);
  }
  int yPos =
      display_state_->get_header_height() + display_state_->get_margin_size();
  if (drawMediaTextAndStop(yPos)) {
    return;
  }

  // image
#ifdef USE_IMAGE
  if (media_player_group_->active_player_->mediaSource !=
      homeassistant_media_player::RemotePlayerMediaSource::
          TVRemotePlayerMediaSource) {
    drawImage();
  }
#endif

  // bottom bar
  drawBottomBar(option_menu);
}

bool HomeThingMenuNowPlaying::drawMediaTextAndStop(int startYPos) {
  if (media_player_group_->active_player_ == nullptr) {
    return true;
  }
  int yPos = startYPos;
  int xPos = display_->get_width() / 2;
  if (media_player_group_->mediaTitleString().size() == 0) {
    display_->printf(display_->get_width() / 2, yPos,
                            display_state_->get_font_large(),
                            display_state_->primaryTextColor(),
                            display::TextAlign::TOP_CENTER, "Nothing!");
    return true;
  } else if (media_player_group_->active_player_->playerState ==
             homeassistant_media_player::RemotePlayerState::
                 PowerOffRemotePlayerState) {
    display_->printf(display_->get_width() / 2, yPos,
                            display_state_->get_font_large(),
                            display_state_->primaryTextColor(),
                            display::TextAlign::TOP_CENTER, "Power Off");
    return true;
  } else {
    display_state_->drawTextMarquee(
        xPos, yPos, display_state_->get_font_small(),
        display_state_->primaryTextColor(), display::TextAlign::TOP_CENTER,
        media_player_group_->mediaTitleString(), tick, display_);

    yPos += display_state_->get_font_small()->get_height() +
            display_state_->get_margin_size();

    display_state_->drawTextMarquee(
        xPos, yPos, display_state_->get_font_medium(),
        display_state_->primaryTextColor(), display::TextAlign::TOP_CENTER,
        media_player_group_->mediaSubtitleString(), tick, display_);

    yPos += display_state_->get_font_small()->get_height() +
            display_state_->get_margin_size();

    display_state_->drawTextMarquee(
        xPos, yPos, display_state_->get_font_small(),
        display_state_->primaryTextColor(), display::TextAlign::TOP_CENTER,
        media_player_group_->mediaAlbumString(), tick, display_);
  }
  return false;
}

#ifdef USE_IMAGE
void HomeThingMenuNowPlaying::drawImage() {
  if (now_playing_image_ == nullptr) {
    return;
  }
  int imageHeight = now_playing_image_->get_height();
  int imageXPos = display_->get_width() / 2;
  int imageYPos = getBottomBarYPosition() - imageHeight - 1;
  display_->image(imageXPos, imageYPos, now_playing_image_,
                         display::ImageAlign::TOP_CENTER);

  display_->line(0, imageYPos - 1, display_->get_width(),
                        imageYPos - 1, display_state_->primaryTextColor());
}
#endif

void HomeThingMenuNowPlaying::drawBottomText() {
  int textYPos = getBottomBarYPosition() +
                 display_state_->get_font_small()->get_height() +
                 display_state_->get_margin_size() * 3;
  int xPos = display_->get_width() / 2;
  std::string text = media_player_group_->mediaPlaylistString().length() > 0
                         ? media_player_group_->mediaPlaylistString()
                         : media_player_group_->queuePositionString();
  display_state_->drawTextWrapped(
      xPos, textYPos, display_state_->get_font_small(),
      display_state_->primaryTextColor(), display::TextAlign::TOP_CENTER, text,
      1, display_);
}

void HomeThingMenuNowPlaying::drawBottomBar(HomeThingOptionMenu* option_menu) {
  if (media_player_group_->active_player_->mediaSource !=
      homeassistant_media_player::RemotePlayerMediaSource::
          TVRemotePlayerMediaSource) {
    int yPos = getBottomBarYPosition();
    display_->line(0, yPos - 1, display_->get_width(), yPos - 1,
                          display_state_->primaryTextColor());
    if (option_menu && option_menu->type == volumeOptionMenu) {
      drawVolumeOptionMenu();
    } else {
      drawMediaDuration();
    }
    drawBottomText();
  }
}

void HomeThingMenuNowPlaying::drawMediaDuration() {
  // if (media_player_group_->active_player_->get_player_type() !=
  //     homeassistant_media_player::RemotePlayerType::TVRemotePlayerType) {
  //   homeassistant_media_player::HomeAssistantSpeakerMediaPlayer* activeSpeaker =
  //       static_cast<
  //           homeassistant_media_player::HomeAssistantSpeakerMediaPlayer*>(
  //           media_player_group_->active_player_);
  int mediaDuration = media_player_group_->active_player_->mediaDuration;
  int mediaPosition = media_player_group_->active_player_->mediaPosition;
  if (mediaDuration <= 0 && mediaPosition <= 0) {
    return;
  }
  int barMargin = 1;
  int barHeight = display_state_->get_font_small()->get_baseline();
  int textWidth = (display_state_->get_font_small()->get_baseline() *
                   display_state_->get_font_size_width_ratio() * 5) +
                  display_state_->get_margin_size() / 2;
  int totalBarWidth = display_->get_width() - textWidth * 2;
  int barWidth = 0;
  if (mediaDuration > 0 && mediaPosition > 0) {
    barWidth = (totalBarWidth - 4) * (static_cast<double>(mediaPosition) /
                                      static_cast<double>(mediaDuration));
  }

  int yPos = getBottomBarYPosition() + display_state_->get_margin_size() * 2;
  display_->rectangle(textWidth, yPos, totalBarWidth, barHeight,
                             display_state_->primaryTextColor());
  display_->filled_rectangle(
      textWidth + barMargin * 2, yPos + barMargin * 2, barWidth,
      barHeight - 2 - barMargin * 2, display_state_->primaryTextColor());

  int textYPos = yPos - display_state_->get_font_small()->get_baseline() * 0.1;
  std::string mediaDurationSeconds = secondsToString(mediaDuration);
  std::string mediaPositionSeconds = secondsToString(mediaPosition);
  display_->printf(display_state_->get_margin_size(), textYPos,
                          display_state_->get_font_small(),
                          display_state_->primaryTextColor(),
                          display::TextAlign::TOP_LEFT, "%d:%s",
                          mediaPosition / 60, mediaPositionSeconds.c_str());
  display_->printf(
      display_->get_width() - display_state_->get_margin_size(),
      textYPos, display_state_->get_font_small(),
      display_state_->primaryTextColor(), display::TextAlign::TOP_RIGHT,
      "%d:%s", mediaDuration / 60, mediaDurationSeconds.c_str());
}

void HomeThingMenuNowPlaying::drawVolumeOptionMenu() {
  int barMargin = 1;
  int barHeight = display_state_->get_font_small()->get_baseline();
  int iconMargin = display_state_->get_font_small()->get_baseline() *
                   display_state_->get_font_size_width_ratio() * 3;
  int totalBarWidth = display_->get_width() - iconMargin * 2;
  int barWidth =
      (totalBarWidth - 4) * (media_player_group_->getVolumeLevel() / 100);
  int yPos = getBottomBarYPosition() + display_state_->get_margin_size() * 3;
  display_->printf(
      iconMargin / 2 - display_state_->get_icon_size() / 2, yPos + 1,
      display_state_->get_font_material_small(),
      display_state_->get_color_palette()->get_accent_primary(), "󰕿");
  display_->printf(
      display_->get_width() - iconMargin / 2 -
          display_state_->get_icon_size() / 2,
      yPos + 1, display_state_->get_font_material_small(),
      display_state_->get_color_palette()->get_accent_primary(), "󰕾");

  display_->rectangle(
      iconMargin, yPos, totalBarWidth, barHeight,
      display_state_->get_color_palette()->get_accent_primary());
  display_->filled_rectangle(
      iconMargin + barMargin * 2, yPos + barMargin * 2, barWidth,
      barHeight - 2 - barMargin * 2,
      display_state_->get_color_palette()->get_accent_primary());
}

bool HomeThingMenuNowPlaying::drawOptionMenuAndStop(
    const HomeThingOptionMenu* option_menu) {
  if (option_menu == nullptr) {
    return false;
  }
  switch (option_menu->type) {
    case circleOptionMenu:
      if (option_menu && option_menu->circle_options.size() > 0) {
        drawCircleOptionMenu(option_menu->circle_options);
        return true;
      }
      return false;
    case volumeOptionMenu:
      // called later so it's over text
      return false;
    case noOptionMenu:
      return false;
    case playingNewSourceMenu: {
      int yPos = display_state_->get_header_height() +
                 display_state_->get_margin_size();
      display_->printf(display_->get_width() / 2, yPos,
                              display_state_->get_font_medium(),
                              display_state_->primaryTextColor(),
                              display::TextAlign::TOP_CENTER, "Playing...");
      yPos = yPos + display_state_->get_margin_size() +
             display_state_->get_font_medium()->get_baseline();
      display_state_->drawTextWrapped(
          display_->get_width() / 2, yPos,
          display_state_->get_font_medium(),
          display_state_->get_color_palette()->get_accent_primary(),
          display::TextAlign::TOP_CENTER,
          media_player_group_->get_new_source_name(), 0, display_);
      return true;
    }
  }
  return true;
}

// private
std::string HomeThingMenuNowPlaying::secondsToString(int seconds) {
  if (seconds == -1) {
    return "00:00";
  }
  return seconds % 60 < 10 ? "0" + to_string(seconds % 60)
                           : to_string(seconds % 60);
}

int HomeThingMenuNowPlaying::getBottomBarYPosition() {
  int barYPosition = display_->get_height();
  barYPosition -= (display_state_->get_font_small()->get_height() * 2) +
                  (display_state_->get_margin_size() * 2);
  if (draw_bottom_menu_) {
    barYPosition -= display_state_->get_font_large()->get_baseline() +
                    display_state_->get_margin_size();
  }
  return barYPosition;
}
}  // namespace homething_menu_now_playing
}  // namespace esphome
#endif
