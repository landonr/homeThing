#include "homeThingMenuNowPlaying.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_menu_base {

void HomeThingMenuNowPlaying::drawNowPlayingSelectMenu(
    std::vector<std::shared_ptr<MenuTitleBase>> menu_titles, int menu_index) {
  int yPos = display_buffer_->get_height() - display_state_->get_margin_size() -
             display_state_->get_font_large()->get_baseline();
  if (menu_titles.size() < 1 || menu_index >= menu_titles.size()) {
    return;
  }
  display_buffer_->printf(
      display_buffer_->get_width() * 0.5, yPos,
      display_state_->get_font_large(),
      text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
      display::TextAlign::TOP_CENTER,
      menu_titles[menu_index]->get_name().c_str());
  if (menu_index + 1 < menu_titles.size()) {
    display_buffer_->printf(
        display_buffer_->get_width() * 0.85, yPos,
        display_state_->get_font_small(),
        text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
        display::TextAlign::TOP_CENTER,
        menu_titles[menu_index + 1]->get_name().c_str());
  } else {
    display_buffer_->printf(
        display_buffer_->get_width() * 0.85, yPos,
        display_state_->get_font_small(),
        text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
        display::TextAlign::TOP_CENTER, menu_titles[0]->get_name().c_str());
  }
  if (menu_index - 1 >= 0) {
    display_buffer_->printf(
        display_buffer_->get_width() * 0.15, yPos,
        display_state_->get_font_small(),
        text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
        display::TextAlign::TOP_CENTER,
        menu_titles[menu_index - 1]->get_name().c_str());
  } else {
    display_buffer_->printf(
        display_buffer_->get_width() * 0.15, yPos,
        display_state_->get_font_small(),
        text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
        display::TextAlign::TOP_CENTER,
        menu_titles[menu_titles.size() - 1]->get_name().c_str());
  }
}

void HomeThingMenuNowPlaying::drawNowPlaying(
    int menuIndex, const option_menuType option_menu,
    std::vector<std::shared_ptr<MenuTitleBase>> active_menu) {
  if (drawOptionMenuAndStop(option_menu)) {
    return;
  }
  if (active_menu.size() > 0 &&
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

  if (media_player_group_->active_player_->get_player_type() !=
      homeassistant_media_player::RemotePlayerType::TVRemotePlayerType) {
    homeassistant_media_player::HomeAssistantSpeakerMediaPlayer* activeSpeaker =
        static_cast<
            homeassistant_media_player::HomeAssistantSpeakerMediaPlayer*>(
            media_player_group_->active_player_);
    if (activeSpeaker->playlist_title != activeSpeaker->mediaTitle) {
      nowPlayingText += " " + activeSpeaker->playlist_title;
    } else if (activeSpeaker->media_album_name != activeSpeaker->mediaTitle) {
      nowPlayingText += " " + activeSpeaker->media_album_name;
    }
  }
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
  if (mediaTitleWrappedText->size() > 0) {
    yPos = yPos + display_state_->get_margin_size();
    drawTextWrapped(
        display_buffer_->get_width() / 2, yPos,
        display_state_->get_font_medium()->get_baseline(),
        display_state_->get_font_medium(),
        text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
        display::TextAlign::TOP_CENTER, *mediaTitleWrappedText, maxLines);
  }
  delete mediaTitleWrappedText;
  if (option_menu == volumeOptionMenu) {
    drawVolumeOptionMenu();
  } else {
    drawMediaDuration();
  }
}

void HomeThingMenuNowPlaying::drawMediaDuration() {
  if (media_player_group_->active_player_->get_player_type() !=
      homeassistant_media_player::RemotePlayerType::TVRemotePlayerType) {
    homeassistant_media_player::HomeAssistantSpeakerMediaPlayer* activeSpeaker =
        static_cast<
            homeassistant_media_player::HomeAssistantSpeakerMediaPlayer*>(
            media_player_group_->active_player_);
    int mediaDuration = activeSpeaker->mediaDuration;
    int mediaPosition = activeSpeaker->mediaPosition;
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

    int textYPos =
        yPos - display_state_->get_font_small()->get_baseline() * 0.1;
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
}

std::string HomeThingMenuNowPlaying::stringForNowPlayingMenuState(
    NowPlayingMenuState state) {
  switch (state) {
    case pauseNowPlayingMenuState:
      return media_player_group_->playTitleString();
    case volumeUpNowPlayingMenuState:
      return "Vol Up";
    case volumeDownNowPlayingMenuState:
      return "Vol Down";
    case nextNowPlayingMenuState:
      return "Next";
    case menuNowPlayingMenuState:
      return "Menu";
    case backNowPlayingMenuState:
      return "Back";
    case TVPowerNowPlayingMenuState:
      return "Power";
    case homeNowPlayingMenuState:
      return "TV Home";
    case groupNowPlayingMenuState:
      return "Group";
    case shuffleNowPlayingMenuState:
      return media_player_group_->shuffle_string();
  }
  return "";
}

void HomeThingMenuNowPlaying::drawCircleOptionMenu() {
  int y_pos =
      (display_buffer_->get_height() - display_state_->get_header_height());
  int radius = y_pos * 0.4;
  y_pos = (y_pos * 0.5) + display_state_->get_header_height();
  display_buffer_->circle(display_buffer_->get_width() * 0.5, y_pos, radius,
                          display_state_->get_color_palette()->get_gray());

  auto supported_features =
      media_player_group_->active_player_->get_option_menu_features();
  auto max_index = min(static_cast<int>(supported_features.size()), 5);

  for (int i = 0; i < max_index; i++) {
    auto element = *(supported_features[i].get());
    double angle = i * M_PI / 2.0;
    auto coordinate = get_coordinate(radius, angle);

    auto title = homeassistant_media_player::supported_feature_string(element);

    display::TextAlign text_alignment;
    switch (i) {
      case 0:
        text_alignment = display::TextAlign::CENTER_LEFT;
        break;
      case 1:
        text_alignment = display::TextAlign::BOTTOM_CENTER;
        break;
      case 2:
        text_alignment = display::TextAlign::CENTER_RIGHT;
        break;
      case 3:
        text_alignment = display::TextAlign::TOP_CENTER;
        break;
      case 4:
        text_alignment = display::TextAlign::CENTER;
        coordinate.x = 0;
        coordinate.y = 0;
        break;
      default:
        break;
    }

    ESP_LOGD(TAG, "drawCircleOptionMenu: %d - %s: %f %f", i, title.c_str(),
             coordinate.x, coordinate.y);
    display_buffer_->printf(
        (display_buffer_->get_width() / 2) + coordinate.x, y_pos + coordinate.y,
        display_state_->get_font_small(),
        text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
        text_alignment, title.c_str());
  }
  // display_buffer_->printf(
  //     display_buffer_->get_width() * 0.5,
  //     (display_buffer_->get_height() - 16) * 0.15 + 16,
  //     display_state_->get_font_small(),
  //     text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
  //     display::TextAlign::TOP_CENTER,
  //     media_player_group_->shuffle_string().c_str());
  // display_buffer_->printf(
  //     display_buffer_->get_width() * 0.5,
  //     (display_buffer_->get_height() - 16) * 0.75 + 16,
  //     display_state_->get_font_small(),
  //     text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
  //     display::TextAlign::TOP_CENTER, "Group");
  // display_buffer_->printf(
  //     display_buffer_->get_width() * 0.8,
  //     (display_buffer_->get_height() - 16) * 0.45 + 16,
  //     display_state_->get_font_small(),
  //     text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
  //     display::TextAlign::TOP_CENTER,
  //     media_player_group_->muteString().c_str());
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
    const option_menuType option_menu) {
  switch (option_menu) {
    case tvOptionMenu:
    case speakerOptionMenu:
      drawCircleOptionMenu();
      return true;
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
                           media_player_group_->playingNewSourceText);
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
    int xPos, int yPos, int fontSize, display::Font* font, Color color,
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

}  // namespace homething_menu_base
}  // namespace esphome
