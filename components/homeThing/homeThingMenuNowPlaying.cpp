#include "homeThingMenuNowPlaying.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_menu_base {

std::vector<NowPlayingMenuState>
HomeThingMenuNowPlaying::getNowPlayingMenuStates() {
  if (speaker_group_->activePlayer->get_player_type() ==
      homeassistant_media_player::RemotePlayerType::TVRemotePlayerType) {
    return TVNowPlayingMenuStates();
  }
  return speakerNowPlayingMenuStates();
}

void HomeThingMenuNowPlaying::drawNowPlayingSelectMenu(int menuIndex) {
  auto menuTitles = getNowPlayingMenuStates();
  int activeMenuTitleCount = menuTitles.size();
  int yPos = display_buffer_->get_height() - display_state_->margin_size_ -
             display_state_->get_large_font()->get_baseline();
  if (activeMenuTitleCount < 1) {
    return;
  }
  display_buffer_->printf(
      display_buffer_->get_width() * 0.5, yPos,
      display_state_->get_large_font(),
      text_helpers_->primaryTextColor(display_state_->dark_mode_),
      display::TextAlign::TOP_CENTER,
      stringForNowPlayingMenuState(menuTitles[menuIndex]).c_str());
  if (menuIndex + 1 < activeMenuTitleCount) {
    display_buffer_->printf(
        display_buffer_->get_width() * 0.85, yPos,
        display_state_->get_small_font(),
        text_helpers_->primaryTextColor(display_state_->dark_mode_),
        display::TextAlign::TOP_CENTER,
        stringForNowPlayingMenuState(menuTitles[menuIndex + 1]).c_str());
  } else {
    display_buffer_->printf(
        display_buffer_->get_width() * 0.85, yPos,
        display_state_->get_small_font(),
        text_helpers_->primaryTextColor(display_state_->dark_mode_),
        display::TextAlign::TOP_CENTER,
        stringForNowPlayingMenuState(menuTitles[0]).c_str());
  }
  if (menuIndex - 1 >= 0) {
    display_buffer_->printf(
        display_buffer_->get_width() * 0.15, yPos,
        display_state_->get_small_font(),
        text_helpers_->primaryTextColor(display_state_->dark_mode_),
        display::TextAlign::TOP_CENTER,
        stringForNowPlayingMenuState(menuTitles[menuIndex - 1]).c_str());
  } else {
    display_buffer_->printf(
        display_buffer_->get_width() * 0.15, yPos,
        display_state_->get_small_font(),
        text_helpers_->primaryTextColor(display_state_->dark_mode_),
        display::TextAlign::TOP_CENTER,
        stringForNowPlayingMenuState(menuTitles[activeMenuTitleCount - 1])
            .c_str());
  }
}

void HomeThingMenuNowPlaying::drawNowPlaying(int menuIndex) {
  if (drawOptionMenuAndStop()) {
    return;
  }
  if (display_state_->draw_now_playing_menu_) {
    drawNowPlayingSelectMenu(menuIndex);
  }
  int yPos = display_state_->header_height_ + display_state_->margin_size_ / 4;
  if (speaker_group_->activePlayer->playerState ==
      homeassistant_media_player::RemotePlayerState::
          PowerOffRemotePlayerState) {
    display_buffer_->printf(
        display_buffer_->get_width() / 2, yPos,
        display_state_->get_large_font(),
        text_helpers_->primaryTextColor(display_state_->dark_mode_),
        display::TextAlign::TOP_CENTER, "Power Off");
    return;
  }
  std::string nowPlayingText = "Now Playing,";

  if (speaker_group_->activePlayer->get_player_type() !=
      homeassistant_media_player::RemotePlayerType::TVRemotePlayerType) {
    homeassistant_media_player::HomeAssistantSonosMediaPlayer* activeSpeaker =
        static_cast<homeassistant_media_player::HomeAssistantSonosMediaPlayer*>(
            speaker_group_->activePlayer);
    if (activeSpeaker->mediaPlaylist != activeSpeaker->mediaTitle) {
      nowPlayingText += " " + activeSpeaker->mediaPlaylist;
    } else if (activeSpeaker->mediaAlbumName != activeSpeaker->mediaTitle) {
      nowPlayingText += " " + activeSpeaker->mediaAlbumName;
    }
  }
  int xPos = display_buffer_->get_width() / 2;
  auto nowPlayingWrappedText =
      getWrappedTitles(display_state_->margin_size_,
                       display_state_->get_medium_font()->get_baseline(),
                       display::TextAlign::TOP_LEFT, nowPlayingText);
  auto mediaArtistWrappedText = getWrappedTitles(
      xPos, display_state_->get_large_font()->get_baseline(),
      display::TextAlign::TOP_CENTER, speaker_group_->mediaTitleString());
  auto mediaTitleWrappedText = getWrappedTitles(
      xPos, display_state_->get_medium_font()->get_baseline(),
      display::TextAlign::TOP_CENTER, speaker_group_->mediaSubtitleString());
  int lineCount = nowPlayingWrappedText->size() +
                  mediaArtistWrappedText->size() +
                  mediaTitleWrappedText->size();
  int maxLines = 0;
  if (lineCount > display_state_->now_playing_max_lines_) {
    maxLines = 1;
    if (nowPlayingWrappedText->size() > 1) {
      lineCount =
          1 + mediaArtistWrappedText->size() + mediaTitleWrappedText->size();
    }
  }
  yPos = drawTextWrapped(
      display_state_->margin_size_, yPos,
      display_state_->get_medium_font()->get_baseline(),
      display_state_->get_medium_font(),
      text_helpers_->primaryTextColor(display_state_->dark_mode_),
      display::TextAlign::TOP_LEFT, *nowPlayingWrappedText, maxLines);
  delete nowPlayingWrappedText;
  if (mediaArtistWrappedText->size() == 0 &&
      mediaTitleWrappedText->size() == 0) {
    display_buffer_->printf(
        display_buffer_->get_width() / 2, yPos,
        display_state_->get_large_font(),
        text_helpers_->primaryTextColor(display_state_->dark_mode_),
        display::TextAlign::TOP_CENTER, "Nothing!");
    delete mediaArtistWrappedText;
    delete mediaTitleWrappedText;
    return;
  }
  if (lineCount > display_state_->now_playing_max_lines_) {
    maxLines = 2;
  } else {
    maxLines = 0;
  }
  yPos = yPos + display_state_->margin_size_ / 2;
  if (mediaArtistWrappedText->size() > 0) {
    yPos = drawTextWrapped(
        xPos, yPos, display_state_->get_large_font()->get_baseline(),
        display_state_->get_large_font(),
        text_helpers_->primaryTextColor(display_state_->dark_mode_),
        display::TextAlign::TOP_CENTER, *mediaArtistWrappedText, maxLines);
  }
  delete mediaArtistWrappedText;
  if (mediaTitleWrappedText->size() > 0) {
    yPos = yPos + display_state_->margin_size_;
    drawTextWrapped(display_buffer_->get_width() / 2, yPos,
                    display_state_->get_medium_font()->get_baseline(),
                    display_state_->get_medium_font(),
                    text_helpers_->primaryTextColor(display_state_->dark_mode_),
                    display::TextAlign::TOP_CENTER, *mediaTitleWrappedText,
                    maxLines);
  }
  delete mediaTitleWrappedText;
  if (optionMenu == volumeOptionMenu) {
    drawVolumeOptionMenu();
  } else {
    drawMediaDuration();
  }
}

void HomeThingMenuNowPlaying::drawMediaDuration() {
  if (speaker_group_->activePlayer->get_player_type() !=
      homeassistant_media_player::RemotePlayerType::TVRemotePlayerType) {
    homeassistant_media_player::HomeAssistantSonosMediaPlayer* activeSpeaker =
        static_cast<homeassistant_media_player::HomeAssistantSonosMediaPlayer*>(
            speaker_group_->activePlayer);
    int mediaDuration = activeSpeaker->mediaDuration;
    int mediaPosition = activeSpeaker->mediaPosition;
    if (mediaDuration <= 0 && mediaPosition <= 0) {
      return;
    }
    int barMargin = 1;
    int barHeight = display_state_->get_small_font()->get_baseline();
    int textWidth = (display_state_->get_small_font()->get_baseline() *
                     display_state_->font_size_width_ratio_ * 5) +
                    display_state_->margin_size_ / 2;
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
        text_helpers_->primaryTextColor(display_state_->dark_mode_));
    display_buffer_->filled_rectangle(
        textWidth + barMargin * 2, yPos + barMargin * 2, barWidth,
        barHeight - 2 - barMargin * 2,
        text_helpers_->primaryTextColor(display_state_->dark_mode_));

    int textYPos =
        yPos - display_state_->get_small_font()->get_baseline() * 0.1;
    std::string mediaDurationSeconds = secondsToString(mediaDuration);
    std::string mediaPositionSeconds = secondsToString(mediaPosition);
    display_buffer_->printf(
        display_state_->margin_size_, textYPos,
        display_state_->get_small_font(),
        text_helpers_->primaryTextColor(display_state_->dark_mode_),
        display::TextAlign::TOP_LEFT, "%d:%s", mediaPosition / 60,
        mediaPositionSeconds.c_str());
    display_buffer_->printf(
        display_buffer_->get_width() - display_state_->margin_size_, textYPos,
        display_state_->get_small_font(),
        text_helpers_->primaryTextColor(display_state_->dark_mode_),
        display::TextAlign::TOP_RIGHT, "%d:%s", mediaDuration / 60,
        mediaDurationSeconds.c_str());
  }
}

std::string HomeThingMenuNowPlaying::stringForNowPlayingMenuState(
    NowPlayingMenuState state) {
  switch (state) {
    case pauseNowPlayingMenuState:
      return speaker_group_->playTitleString();
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
      if (speaker_group_->mediaShuffling()) {
        return "Shfl on";
      } else {
        return "Shfl off";
      }
  }
  return "";
}

void HomeThingMenuNowPlaying::drawSpeakerOptionMenu() {
  display_buffer_->circle(display_buffer_->get_width() * 0.5,
                          (display_buffer_->get_height() - 16) * 0.45 + 24, 48,
                          display_state_->color_gray_);
  display_buffer_->printf(
      display_buffer_->get_width() * 0.5,
      (display_buffer_->get_height() - 16) * 0.15 + 16,
      display_state_->get_small_font(),
      text_helpers_->primaryTextColor(display_state_->dark_mode_),
      display::TextAlign::TOP_CENTER, speaker_group_->shuffleString().c_str());
  display_buffer_->printf(
      display_buffer_->get_width() * 0.5,
      (display_buffer_->get_height() - 16) * 0.75 + 16,
      display_state_->get_small_font(),
      text_helpers_->primaryTextColor(display_state_->dark_mode_),
      display::TextAlign::TOP_CENTER, "Group");
  display_buffer_->printf(
      display_buffer_->get_width() * 0.8,
      (display_buffer_->get_height() - 16) * 0.45 + 16,
      display_state_->get_small_font(),
      text_helpers_->primaryTextColor(display_state_->dark_mode_),
      display::TextAlign::TOP_CENTER, speaker_group_->muteString().c_str());
}

void HomeThingMenuNowPlaying::drawTVOptionMenu() {
  display_buffer_->circle(display_buffer_->get_width() * 0.5,
                          (display_buffer_->get_height() - 16) * 0.45 + 24, 48,
                          display_state_->color_gray_);
  display_buffer_->printf(
      display_buffer_->get_width() * 0.5,
      (display_buffer_->get_height() - 16) * 0.15 + 16,
      display_state_->get_small_font(),
      text_helpers_->primaryTextColor(display_state_->dark_mode_),
      display::TextAlign::TOP_CENTER, "Remote Menu");
  display_buffer_->printf(
      display_buffer_->get_width() * 0.5,
      (display_buffer_->get_height() - 16) * 0.75 + 16,
      display_state_->get_small_font(),
      text_helpers_->primaryTextColor(display_state_->dark_mode_),
      display::TextAlign::TOP_CENTER, "Pause");
  display_buffer_->printf(
      display_buffer_->get_width() * 0.2,
      (display_buffer_->get_height() - 16) * 0.45 + 16,
      display_state_->get_small_font(),
      text_helpers_->primaryTextColor(display_state_->dark_mode_),
      display::TextAlign::TOP_CENTER, "Back");
  display_buffer_->printf(
      display_buffer_->get_width() * 0.8,
      (display_buffer_->get_height() - 16) * 0.45 + 16,
      display_state_->get_small_font(),
      text_helpers_->primaryTextColor(display_state_->dark_mode_),
      display::TextAlign::TOP_CENTER, "Home");
  display_buffer_->printf(
      display_buffer_->get_width() * 0.5,
      (display_buffer_->get_height() - 16) * 0.45 + 16,
      display_state_->get_small_font(),
      text_helpers_->primaryTextColor(display_state_->dark_mode_),
      display::TextAlign::TOP_CENTER, "TV Power");
}

void HomeThingMenuNowPlaying::drawVolumeOptionMenu() {
  int barMargin = 1;
  int barHeight = display_state_->get_small_font()->get_baseline();
  int iconMargin = display_state_->get_small_font()->get_baseline() *
                   display_state_->font_size_width_ratio_ * 3;
  int totalBarWidth = display_buffer_->get_width() - iconMargin * 2;
  int barWidth = (totalBarWidth - 4) * (speaker_group_->getVolumeLevel() / 100);
  int yPos = display_state_->getBottomBarYPosition(
      true, display_buffer_->get_height());
  display_buffer_->printf(iconMargin / 2 - display_state_->icon_size_ / 2,
                          yPos + 1, display_state_->get_material_font_small(),
                          display_state_->color_accent_primary_, "󰕿");
  display_buffer_->printf(display_buffer_->get_width() - iconMargin / 2 -
                              display_state_->icon_size_ / 2,
                          yPos + 1, display_state_->get_material_font_small(),
                          display_state_->color_accent_primary_, "󰕾");

  display_buffer_->rectangle(iconMargin, yPos, totalBarWidth, barHeight,
                             display_state_->color_accent_primary_);
  display_buffer_->filled_rectangle(
      iconMargin + barMargin * 2, yPos + barMargin * 2, barWidth,
      barHeight - 2 - barMargin * 2, display_state_->color_accent_primary_);
}

bool HomeThingMenuNowPlaying::drawOptionMenuAndStop() {
  switch (optionMenu) {
    case tvOptionMenu:
      drawTVOptionMenu();
      return true;
    case speakerOptionMenu:
      drawSpeakerOptionMenu();
      return true;
    case volumeOptionMenu:
      // called later so it's over text
      return false;
    case noOptionMenu:
      return false;
    case playingNewSourceMenu:
      display_buffer_->printf(
          display_buffer_->get_width() / 2,
          display_state_->header_height_ + display_state_->margin_size_,
          display_state_->get_medium_font(),
          text_helpers_->primaryTextColor(display_state_->dark_mode_),
          display::TextAlign::TOP_CENTER, "Playing...");
      auto playingNewSourceWrappedText = getWrappedTitles(
          display_buffer_->get_width() / 2,
          display_state_->get_large_font()->get_baseline(),
          display::TextAlign::TOP_CENTER, speaker_group_->playingNewSourceText);
      drawTextWrapped(
          display_buffer_->get_width() / 2,
          display_state_->header_height_ + display_state_->margin_size_ * 2 +
              display_state_->get_medium_font()->get_baseline(),
          24, display_state_->get_large_font(),
          text_helpers_->primaryTextColor(display_state_->dark_mode_),
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
  TextHelpers::tokenize(wrappedTitles, "\n", output);
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
