#include "esphome/components/homeThing/HomeThingNowPlayingHeader.h"

namespace esphome {
namespace homething_menu_now_playing {

std::string HomeThingMenuNowPlayingHeader::get_header_title() {
  switch (*app_menu_index_) {
    case 0: {
      if (media_player_group_ == nullptr) {
        return "Now Playing";
      }
      if (media_player_group_->active_player_ == nullptr) {
        return "Loading...";
      }
      return media_player_group_->active_player_->get_name();
      break;
    }
    case 1:
      return "Sources";
      break;
    case 2:
      return "Media Players";
      break;
    default:
      break;
  }
  return "xx2";
}

int HomeThingMenuNowPlayingHeader::draw_header_details(
    int xPos, int yPos, display::DisplayBuffer* display_buffer,
    homething_display_state::HomeThingDisplayState* display_state,
    homething_menu_base::HomeThingMenuTextHelpers* text_helpers) {
  switch (*app_menu_index_) {
    case 0:
      break;
    default:
      return 0;
  }
  xPos = xPos - drawPlayPauseIcon(xPos, yPos, display_buffer, display_state,
                                  text_helpers);
  xPos = xPos -
         drawRepeat(xPos, yPos, display_buffer, display_state, text_helpers);
  xPos = xPos -
         drawShuffle(xPos, yPos, display_buffer, display_state, text_helpers);
  return xPos;
}

int HomeThingMenuNowPlayingHeader::drawPlayPauseIcon(
    int oldXPos, int yPos, display::DisplayBuffer* display_buffer,
    homething_display_state::HomeThingDisplayState* display_state,
    homething_menu_base::HomeThingMenuTextHelpers* text_helpers) {
  int iconWidth =
      display_state->get_icon_size() + (display_state->get_margin_size() / 2);
  int xPos = oldXPos - iconWidth + (display_state->get_margin_size() / 2);
  auto active_player = media_player_group_->active_player_;
  if (active_player == NULL) {
    return 0;
  }
  auto menuTitle = homething_menu_base::headerMediaPlayerTitle(active_player);
  switch (active_player->playerState) {
    case homeassistant_media_player::RemotePlayerState::
        PlayingRemotePlayerState: {
      display_buffer->printf(
          xPos, yPos, display_state->get_font_material_small(),
          menuTitle.mediaSourceIconColor(
              display_state->get_color_palette()->get_accent_primary()),
          menuTitle.mediaSourceIcon().c_str());
      break;
    }
    case homeassistant_media_player::RemotePlayerState::PausedRemotePlayerState:
      display_buffer->printf(
          xPos, yPos, display_state->get_font_material_small(),
          display_state->get_color_palette()->get_accent_primary(), "󰏤");
      break;
    case homeassistant_media_player::RemotePlayerState::
        StoppedRemotePlayerState:
      display_buffer->printf(
          xPos, yPos, display_state->get_font_material_small(),
          display_state->get_color_palette()->get_accent_primary(), "󰓛");
      break;
    case homeassistant_media_player::RemotePlayerState::
        PowerOffRemotePlayerState:
      display_buffer->printf(
          xPos, yPos, display_state->get_font_material_small(),
          display_state->get_color_palette()->get_accent_primary(), "󰽥");
      break;
    default:
      return 0;
  }
  return iconWidth;
}

int HomeThingMenuNowPlayingHeader::drawShuffle(
    int oldXPos, int yPos, display::DisplayBuffer* display_buffer,
    homething_display_state::HomeThingDisplayState* display_state,
    homething_menu_base::HomeThingMenuTextHelpers* text_helpers) {
  if (!media_player_group_ || media_player_group_->active_player_ == NULL ||
      display_state->get_draw_shuffle() ==
          homething_display_state::DisplayIconEnabledState::OFF) {
    ESP_LOGI(TAG, "drawShuffle return 0");
    return 0;
  }
  auto active_player = media_player_group_->active_player_;
  if (active_player->get_player_type() ==
          homeassistant_media_player::RemotePlayerType::TVRemotePlayerType ||
      !active_player->supports(homeassistant_media_player::
                                   MediaPlayerSupportedFeature::SHUFFLE_SET)) {
    ESP_LOGI(TAG, "drawShuffle return 0 not set");
    return 0;
  }
  if (active_player->playerState !=
      homeassistant_media_player::RemotePlayerState::StoppedRemotePlayerState) {
    int iconWidth =
        display_state->get_icon_size() + (display_state->get_margin_size() / 2);
    int xPos = oldXPos - iconWidth + (display_state->get_margin_size() / 2);
    if (media_player_group_->mediaShuffling()) {
      display_buffer->printf(
          xPos, yPos, display_state->get_font_material_small(),
          display_state->get_color_palette()->get_accent_primary(), "󰒝");
    } else if (display_state->get_draw_shuffle() ==
               homething_display_state::DisplayIconEnabledState::ALWAYS) {
      display_buffer->printf(
          xPos, yPos, display_state->get_font_material_small(),
          display_state->get_color_palette()->get_accent_primary(), "󰒞");
    } else {
      ESP_LOGI(TAG, "drawShuffle uhh %d", display_state->get_draw_shuffle());
      return 0;
    }
    return iconWidth;
  }
  ESP_LOGI(TAG, "drawShuffle fail");
  return 0;
}

int HomeThingMenuNowPlayingHeader::drawRepeat(
    int oldXPos, int yPos, display::DisplayBuffer* display_buffer,
    homething_display_state::HomeThingDisplayState* display_state,
    homething_menu_base::HomeThingMenuTextHelpers* text_helpers) {
  if (display_state->get_draw_repeat() ==
          homething_display_state::DisplayIconEnabledState::OFF ||
      !media_player_group_ || media_player_group_->active_player_ == NULL) {
    return 0;
  }
  auto active_player = media_player_group_->active_player_;
  if (active_player->get_player_type() ==
          homeassistant_media_player::RemotePlayerType::TVRemotePlayerType ||
      !active_player->supports(homeassistant_media_player::
                                   MediaPlayerSupportedFeature::REPEAT_SET) ||
      active_player->playerState ==
          homeassistant_media_player::RemotePlayerState::
              StoppedRemotePlayerState) {
    return 0;
  }
  int iconWidth =
      display_state->get_icon_size() + (display_state->get_margin_size() / 2);
  int xPos = oldXPos - iconWidth + (display_state->get_margin_size() / 2);
  switch (media_player_group_->get_repeat_mode()) {
    case homeassistant_media_player::MediaPlayerRepeatMode::ALL:
      display_buffer->printf(
          xPos, yPos, display_state->get_font_material_small(),
          display_state->get_color_palette()->get_accent_primary(), "󰑖");
      break;
    case homeassistant_media_player::MediaPlayerRepeatMode::ONE:
      display_buffer->printf(
          xPos, yPos, display_state->get_font_material_small(),
          display_state->get_color_palette()->get_accent_primary(), "󰑘");
      break;
    case homeassistant_media_player::MediaPlayerRepeatMode::OFF:
      if (display_state->get_draw_repeat() !=
          homething_display_state::DisplayIconEnabledState::ALWAYS) {
        return 0;
      }
      display_buffer->printf(
          xPos, yPos, display_state->get_font_material_small(),
          display_state->get_color_palette()->get_accent_primary(), "󰑗");
      break;
    default:
      return 0;
  }
  return iconWidth;
}

int HomeThingMenuNowPlayingHeader::drawHeaderVolumeLevel(
    int oldXPos, int yPos, display::DisplayBuffer* display_buffer,
    homething_display_state::HomeThingDisplayState* display_state,
    homething_menu_base::HomeThingMenuTextHelpers* text_helpers) {
  if (media_player_group_ == nullptr ||
      media_player_group_->active_player_ == nullptr) {
    return 0;
  }
  if (!display_state->get_draw_volume_level()) {
    return 0;
  }
  int xPos = oldXPos - display_state->get_margin_size() / 2;
  display_buffer->printf(
      xPos, yPos, display_state->get_font_small(),
      text_helpers->primaryTextColor(display_state->get_dark_mode()),
      display::TextAlign::TOP_RIGHT, "%.0f%%",
      media_player_group_->getVolumeLevel());
  return 24;
}
}  // namespace homething_menu_now_playing
}  // namespace esphome