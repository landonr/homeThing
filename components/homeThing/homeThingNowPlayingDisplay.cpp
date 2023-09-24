#ifdef USE_MEDIA_PLAYER_GROUP
#include "homeThingNowPlayingDisplay.h"
#include "esphome/components/homeThing/homeThingMenuTextHelpers.h"
#include "esphome/components/homeThing/homeThingOptionMenu.h"
#include "esphome/components/homeThingDisplayState/homeThingDisplayState.h"
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
      (display_buffer_->get_height() - display_state_->get_header_height());
  int radius;
  if (display_buffer_->get_height() > display_buffer_->get_width()) {
    radius = display_buffer_->get_width() * 0.25;
  } else {
    radius = y_pos * 0.4;
  }
  y_pos = (y_pos * 0.5) + display_state_->get_header_height();
  display_buffer_->circle(display_buffer_->get_width() * 0.5, y_pos, radius,
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
    display_buffer_->printf(
        (display_buffer_->get_width() / 2) + coordinate.x, y_pos + coordinate.y,
        display_state_->get_font_small(),
        text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
        text_alignment, title.c_str());
  }
}

void HomeThingMenuNowPlaying::drawNowPlayingSelectMenu(
    const std::vector<homething_menu_base::MenuTitleBase*>* menu_titles,
    int menu_index) {
  int yPos = display_buffer_->get_height() - display_state_->get_margin_size() -
             display_state_->get_font_large()->get_baseline();
  auto menuTitlesSize = menu_titles->size();
  if (menuTitlesSize < 1 || menu_index >= menuTitlesSize) {
    return;
  }
  display_buffer_->printf(
      display_buffer_->get_width() * 0.5, yPos,
      display_state_->get_font_large(),
      text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
      display::TextAlign::TOP_CENTER,
      (*menu_titles)[menu_index]->get_name().c_str());
  if (menu_index + 1 < menuTitlesSize) {
    display_buffer_->printf(
        display_buffer_->get_width() * 0.85, yPos,
        display_state_->get_font_small(),
        text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
        display::TextAlign::TOP_CENTER,
        (*menu_titles)[menu_index + 1]->get_name().c_str());
  } else {
    display_buffer_->printf(
        display_buffer_->get_width() * 0.85, yPos,
        display_state_->get_font_small(),
        text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
        display::TextAlign::TOP_CENTER, (*menu_titles)[0]->get_name().c_str());
  }
  if (menu_index - 1 >= 0) {
    display_buffer_->printf(
        display_buffer_->get_width() * 0.15, yPos,
        display_state_->get_font_small(),
        text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
        display::TextAlign::TOP_CENTER,
        (*menu_titles)[menu_index - 1]->get_name().c_str());
  } else {
    display_buffer_->printf(
        display_buffer_->get_width() * 0.15, yPos,
        display_state_->get_font_small(),
        text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
        display::TextAlign::TOP_CENTER,
        (*menu_titles)[menuTitlesSize - 1]->get_name().c_str());
  }
}

void HomeThingMenuNowPlaying::drawNowPlaying(
    int menuIndex, HomeThingOptionMenu* option_menu,
    const std::vector<homething_menu_base::MenuTitleBase*>* active_menu) {
  if ((option_menu && drawOptionMenuAndStop(option_menu)) ||
      display_state_ == nullptr) {
    return;
  }
  if (active_menu && active_menu->size() > 0 &&
      display_state_->get_draw_now_playing_bottom_menu()) {
    drawNowPlayingSelectMenu(active_menu, menuIndex);
  }
  int yPos = display_state_->get_header_height() +
             display_state_->get_margin_size() / 4;
  if (media_player_group_->active_player_->playerState ==
      homeassistant_media_player::RemotePlayerState::
          PowerOffRemotePlayerState) {
    display_buffer_->printf(
        display_buffer_->get_width() / 2, yPos,
        display_state_->get_font_large(),
        text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
        display::TextAlign::TOP_CENTER, "Power Off");
    return;
  }
  std::string nowPlayingText = "Now Playing,";

  // if (media_player_group_->active_player_->get_player_type() !=
  //     homeassistant_media_player::RemotePlayerType::TVRemotePlayerType) {
  //   homeassistant_media_player::HomeAssistantSpeakerMediaPlayer* activeSpeaker =
  //       static_cast<
  //           homeassistant_media_player::HomeAssistantSpeakerMediaPlayer*>(
  //           media_player_group_->active_player_);
  if (media_player_group_->active_player_->playlist_title !=
      media_player_group_->active_player_->mediaTitle) {
    nowPlayingText += " " + media_player_group_->active_player_->playlist_title;
    // } else if (media_player_group_->active_player_->media_album_name !=
    //            media_player_group_->active_player_->mediaTitle) {
    //   nowPlayingText +=
    //       " " + media_player_group_->active_player_->media_album_name;
  }
  // }
  int xPos = display_buffer_->get_width() / 2;
  auto nowPlayingWrappedText =
      getWrappedTitles(display_state_->get_margin_size(),
                       display_state_->get_font_medium()->get_baseline(),
                       display::TextAlign::TOP_LEFT, nowPlayingText);
  auto mediaArtistWrappedText = getWrappedTitles(
      xPos, display_state_->get_font_large()->get_baseline(),
      display::TextAlign::TOP_CENTER, media_player_group_->mediaTitleString());
  auto mediaTitleWrappedText =
      getWrappedTitles(xPos, display_state_->get_font_medium()->get_baseline(),
                       display::TextAlign::TOP_CENTER,
                       media_player_group_->mediaSubtitleString());
  int lineCount = nowPlayingWrappedText->size() +
                  mediaArtistWrappedText->size() +
                  mediaTitleWrappedText->size();
  int maxLines = 0;
  if (lineCount > display_state_->get_now_playing_max_lines()) {
    maxLines = 1;
    if (nowPlayingWrappedText->size() > 1) {
      lineCount =
          1 + mediaArtistWrappedText->size() + mediaTitleWrappedText->size();
    }
  }
  yPos = drawTextWrapped(
      display_state_->get_margin_size(), yPos,
      display_state_->get_font_medium()->get_baseline(),
      display_state_->get_font_medium(),
      text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
      display::TextAlign::TOP_LEFT, *nowPlayingWrappedText, maxLines);
  delete nowPlayingWrappedText;
  if (mediaArtistWrappedText->size() == 0 &&
      mediaTitleWrappedText->size() == 0) {
    display_buffer_->printf(
        display_buffer_->get_width() / 2, yPos,
        display_state_->get_font_large(),
        text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
        display::TextAlign::TOP_CENTER, "Nothing!");
    delete mediaArtistWrappedText;
    delete mediaTitleWrappedText;
    return;
  }
  if (lineCount > display_state_->get_now_playing_max_lines()) {
    maxLines = 2;
  } else {
    maxLines = 0;
  }
  yPos = yPos + display_state_->get_margin_size() / 2;
  if (mediaArtistWrappedText->size() > 0) {
    yPos = drawTextWrapped(
        xPos, yPos, display_state_->get_font_large()->get_baseline(),
        display_state_->get_font_large(),
        text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
        display::TextAlign::TOP_CENTER, *mediaArtistWrappedText, maxLines);
  }
  delete mediaArtistWrappedText;
  if (mediaTitleWrappedText->size() > 0 &&
      media_player_group_->mediaTitleString() !=
          media_player_group_->mediaSubtitleString()) {
    yPos = yPos + display_state_->get_margin_size();
    drawTextWrapped(
        display_buffer_->get_width() / 2, yPos,
        display_state_->get_font_medium()->get_baseline(),
        display_state_->get_font_medium(),
        text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
        display::TextAlign::TOP_CENTER, *mediaTitleWrappedText, maxLines);
  }
  delete mediaTitleWrappedText;
  if (option_menu && option_menu->type == volumeOptionMenu) {
    drawVolumeOptionMenu();
  } else {
    drawMediaDuration();
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
  int totalBarWidth = display_buffer_->get_width() - textWidth * 2;
  int barWidth = 0;
  if (mediaDuration > 0 && mediaPosition > 0) {
    barWidth = (totalBarWidth - 4) * (static_cast<double>(mediaPosition) /
                                      static_cast<double>(mediaDuration));
  }

  int yPos = display_state_->getBottomBarYPosition(
      true, display_buffer_->get_height());
  display_buffer_->rectangle(
      textWidth, yPos, totalBarWidth, barHeight,
      text_helpers_->primaryTextColor(display_state_->get_dark_mode()));
  display_buffer_->filled_rectangle(
      textWidth + barMargin * 2, yPos + barMargin * 2, barWidth,
      barHeight - 2 - barMargin * 2,
      text_helpers_->primaryTextColor(display_state_->get_dark_mode()));

  int textYPos = yPos - display_state_->get_font_small()->get_baseline() * 0.1;
  std::string mediaDurationSeconds = secondsToString(mediaDuration);
  std::string mediaPositionSeconds = secondsToString(mediaPosition);
  display_buffer_->printf(
      display_state_->get_margin_size(), textYPos,
      display_state_->get_font_small(),
      text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
      display::TextAlign::TOP_LEFT, "%d:%s", mediaPosition / 60,
      mediaPositionSeconds.c_str());
  display_buffer_->printf(
      display_buffer_->get_width() - display_state_->get_margin_size(),
      textYPos, display_state_->get_font_small(),
      text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
      display::TextAlign::TOP_RIGHT, "%d:%s", mediaDuration / 60,
      mediaDurationSeconds.c_str());
}

void HomeThingMenuNowPlaying::drawVolumeOptionMenu() {
  int barMargin = 1;
  int barHeight = display_state_->get_font_small()->get_baseline();
  int iconMargin = display_state_->get_font_small()->get_baseline() *
                   display_state_->get_font_size_width_ratio() * 3;
  int totalBarWidth = display_buffer_->get_width() - iconMargin * 2;
  int barWidth =
      (totalBarWidth - 4) * (media_player_group_->getVolumeLevel() / 100);
  int yPos = display_state_->getBottomBarYPosition(
      true, display_buffer_->get_height());
  display_buffer_->printf(
      iconMargin / 2 - display_state_->get_icon_size() / 2, yPos + 1,
      display_state_->get_font_material_small(),
      display_state_->get_color_palette()->get_accent_primary(), "󰕿");
  display_buffer_->printf(
      display_buffer_->get_width() - iconMargin / 2 -
          display_state_->get_icon_size() / 2,
      yPos + 1, display_state_->get_font_material_small(),
      display_state_->get_color_palette()->get_accent_primary(), "󰕾");

  display_buffer_->rectangle(
      iconMargin, yPos, totalBarWidth, barHeight,
      display_state_->get_color_palette()->get_accent_primary());
  display_buffer_->filled_rectangle(
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
    case tvOptionMenu:
    case speakerOptionMenu:
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
    case playingNewSourceMenu:
      display_buffer_->printf(
          display_buffer_->get_width() / 2,
          display_state_->get_header_height() +
              display_state_->get_margin_size(),
          display_state_->get_font_medium(),
          text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
          display::TextAlign::TOP_CENTER, "Playing...");
      auto playingNewSourceWrappedText =
          getWrappedTitles(display_buffer_->get_width() / 2,
                           display_state_->get_font_large()->get_baseline(),
                           display::TextAlign::TOP_CENTER,
                           media_player_group_->get_new_source_name());
      drawTextWrapped(
          display_buffer_->get_width() / 2,
          display_state_->get_header_height() +
              display_state_->get_margin_size() * 2 +
              display_state_->get_font_medium()->get_baseline(),
          24, display_state_->get_font_large(),
          text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
          display::TextAlign::TOP_CENTER, *playingNewSourceWrappedText, 0);
      delete playingNewSourceWrappedText;
      return true;
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

std::vector<std::string>* HomeThingMenuNowPlaying::getWrappedTitles(
    int xPos, int fontSize, display::TextAlign alignment, std::string text) {
  std::vector<std::string>* output = new std::vector<std::string>();
  if (text.size() == 0) {
    return output;
  }
  std::string wrappedTitles = text_helpers_->textWrap(
      text, text_helpers_->getCharacterLimit(xPos, fontSize, alignment));
  tokenize(wrappedTitles, "\n", output);
  return output;
}

int HomeThingMenuNowPlaying::drawTextWrapped(
    int xPos, int yPos, int fontSize, font::Font* font, Color color,
    display::TextAlign alignment, std::vector<std::string> wrappedTitles,
    int maxLines) {
  int characterLimit =
      text_helpers_->getCharacterLimit(xPos, fontSize, alignment);
  int max = maxLines != 0 && maxLines < wrappedTitles.size()
                ? maxLines
                : wrappedTitles.size();
  for (int i = 0; i < max; i++) {
    if (maxLines != 0 && maxLines - 1 == i && i == max - 1) {
      std::string title = wrappedTitles[i];
      if (wrappedTitles[i].size() > characterLimit - 2) {
        title.erase(title.length() - 3);
      }
      title = title + "...";
      display_buffer_->printf(xPos, yPos + (i * fontSize), font, color,
                              alignment, title.c_str());
      break;
    } else {
      display_buffer_->printf(xPos, yPos + (i * fontSize), font, color,
                              alignment, wrappedTitles[i].c_str());
    }
  }
  return yPos + (max * fontSize);
}
}  // namespace homething_menu_now_playing
}  // namespace esphome
#endif
