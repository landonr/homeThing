#include "homeThingMenuBoot.h"
#include <algorithm>
#include <string>
#include "esphome/components/network/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_menu_base {

#ifdef USE_MEDIA_PLAYER_GROUP
void set_media_players_loaded(
    binary_sensor::BinarySensor* media_player_group_sensor) {
  media_player_group_sensor_ = media_player_group_sensor;
  if (media_player_group_sensor_) {
    media_player_group_sensor_->add_on_state_callback(
        [this](bool state) { this->callback_(); });
  }
}
#endif

int HomeThingMenuBoot::drawBootSequenceTitleRainbow(
    int xPos, int yPos, const MenuStates activeMenuState) {
  std::string bootTitle = "homeThing";
  int loopLimit = bootTitle.length();
  int delayTime = 6;
  int animationStartTime = delayTime;
  int animationLength = animationStartTime + loopLimit + delayTime * 2;
  const int animationTick = animation_->animationTick->state;

  ESP_LOGD(TAG, "tick %d total %d", animationTick, animationLength);
  if (animationTick > animationStartTime && animationTick < animationLength) {
    int currentAnimationTick = animationTick - animationStartTime;
    int activeCharacter = currentAnimationTick;
    int maxCharacters = loopLimit;
    std::vector<Color> colors = {
        display_state_->get_color_palette()->get_black(),
        display_state_->get_color_palette()->get_yellow(),
        display_state_->get_color_palette()->get_red(),
        display_state_->get_color_palette()->get_pink(),
        display_state_->get_color_palette()->get_green(),
        display_state_->get_color_palette()->get_blue()};
    int textWidth = maxCharacters *
                    display_state_->get_font_large()->get_baseline() *
                    display_state_->get_font_size_width_ratio();
    for (int i = 0; i < maxCharacters; i++) {
      int colorIndex = i <= currentAnimationTick ? currentAnimationTick - i : 0;
      if (colorIndex > 0) {
        auto color =
            colors.size() > colorIndex
                ? colors[colorIndex]
                : display_state_->get_color_palette()->get_accent_primary();
        int characterXPos =
            xPos - textWidth / 2 +
            (i * display_state_->get_font_large()->get_baseline() *
             display_state_->get_font_size_width_ratio());
        display_buffer_->printf(
            characterXPos, yPos, display_state_->get_font_large_heavy(), color,
            display::TextAlign::TOP_LEFT, "%c", bootTitle[i]);
      }
    }
  }
  return animationLength;
}

int HomeThingMenuBoot::drawBootSequenceLogo(int xPos, int imageYPos) {
#ifdef USE_IMAGE
  if (display_state_->get_launch_image() == NULL) {
    return 0;
  }
  float animationLength = 6;
  int delayTime = 2;
  int totalDuration = delayTime + animationLength;
  const int animationTick = animation_->animationTick->state;
  if (animationTick > delayTime && animationTick < totalDuration) {
    int colorValue =
        (static_cast<float>(animationTick - delayTime) / animationLength) * 255;
    if (!display_state_->get_dark_mode()) {
      colorValue = 255 - colorValue;
    }
    auto color = Color(colorValue, colorValue, colorValue);
    display_buffer_->image(xPos, imageYPos, display_state_->get_launch_image(),
                           display::ImageAlign::TOP_CENTER, color);
  } else if (animationTick >= totalDuration) {
    int color = !display_state_->get_dark_mode() ? 0 : 255;
    display_buffer_->image(xPos, imageYPos, display_state_->get_launch_image(),
                           display::ImageAlign::TOP_CENTER,
                           Color(color, color, color));
  }
  return totalDuration;
#endif
  return 0;
}

int HomeThingMenuBoot::drawBootSequenceHeader(
    const MenuStates activeMenuState) {
  float animationLength = 8;
  int delayTime = 20;
  int totalDuration = delayTime + animationLength;
  int maxValue = display_state_->get_header_height();
  const int animationTick = animation_->animationTick->state;
  if (animationTick > delayTime && animationTick < totalDuration) {
    int yPosOffset = maxValue - static_cast<float>((animationTick - delayTime) /
                                                   animationLength) *
                                    maxValue;
    header_->drawHeader(yPosOffset, activeMenuState);
  } else if (animationTick >= totalDuration) {
    header_->drawHeader(0, activeMenuState);
  }
  return totalDuration;
}

float HomeThingMenuBoot::bootSequenceLoadingProgress() {
  switch (get_boot_menu_state()) {
    case BOOT_MENU_STATE_START:
      return 0;
    case BOOT_MENU_STATE_NETWORK:
      return 0.1;
    case BOOT_MENU_STATE_API:
      return 0.2;
    case BOOT_MENU_STATE_PLAYERS:
#ifdef USE_MEDIA_PLAYER_GROUP
      if (media_player_group_ != NULL) {
        float totalPlayers =
            static_cast<float>(media_player_group_->totalPlayers());
        float loadedPlayers =
            static_cast<float>(media_player_group_->loadedPlayers);
        float progress = 0.7 * (loadedPlayers / totalPlayers);
        return 0.3 + progress;
      }
#endif
      return 0.3;
    case BOOT_MENU_STATE_COMPLETE:
      return 1;
  }
  return 0;
}

void HomeThingMenuBoot::drawBootSequenceLoadingBar(int yPosOffset,
                                                   float progress) {
  int barMargin = 1;
  int barHeight = display_state_->get_font_small()->get_baseline();
  int iconMargin = display_state_->get_font_small()->get_baseline() *
                   display_state_->get_font_size_width_ratio() * 3;
  int totalBarWidth = display_buffer_->get_width() - iconMargin * 2;
  int barWidth = (totalBarWidth - 4) * progress;
  int yPos = display_state_->getBottomBarYPosition(
                 false, display_buffer_->get_height()) +
             yPosOffset;

  display_buffer_->rectangle(
      iconMargin, yPos, totalBarWidth, barHeight,
      display_state_->get_color_palette()->get_accent_primary());
  display_buffer_->filled_rectangle(
      iconMargin + barMargin * 2, yPos + barMargin * 2, barWidth,
      barHeight - 2 - barMargin * 2,
      display_state_->get_color_palette()->get_accent_primary());
}

int HomeThingMenuBoot::drawBootSequenceLoadingBarAnimation() {
  float animationLength = 8;
  int delayTime = 20;
  int totalDuration = delayTime + animationLength;
  int maxValue = display_state_->get_font_small()->get_baseline() +
                 display_state_->get_bottom_bar_margin();

  const int animationTick = animation_->animationTick->state;
  if (animationTick > delayTime && animationTick < totalDuration) {
    int yPosOffset = maxValue - (static_cast<float>(animationTick - delayTime) /
                                 animationLength) *
                                    maxValue;
    drawBootSequenceLoadingBar(yPosOffset, bootSequenceLoadingProgress());
  } else if (animationTick >= totalDuration) {
    drawBootSequenceLoadingBar(0, bootSequenceLoadingProgress());
  }
  return totalDuration;
}

BootMenuSkipState HomeThingMenuBoot::bootSequenceCanSkip(
    const MenuStates activeMenuState) {
#ifdef USE_MEDIA_PLAYER_GROUP
  bool canSkip = activeMenuState == bootMenu && media_player_group_ != NULL &&
                 media_player_group_->loadedPlayers > 0;
#else
  bool canSkip = true;
#endif
  bool finishedBootAnimation = animation_->animationTick->state >=
                               animation_config_.total_animation_length();
  if (finishedBootAnimation)
    boot_animation_complete_ = true;
  if (canSkip) {
    return BOOT_MENU_SKIP_STATE_MENU;
  } else if (get_boot_menu_state() < BOOT_MENU_STATE_API &&
             finishedBootAnimation) {
    return BOOT_MENU_SKIP_STATE_SLEEP;
  }
  return BOOT_MENU_SKIP_STATE_NONE;
}

void HomeThingMenuBoot::drawBootSequenceSkipTitle(
    int xPos, int imageYPos, const MenuStates activeMenuState) {
  auto skip_state = bootSequenceCanSkip(activeMenuState);
  int yPos = display_state_->getBottomBarYPosition(
                 false, display_buffer_->get_height()) -
             display_state_->get_margin_size() / 2 -
             display_state_->get_font_small()->get_baseline();
  switch (skip_state) {
    case BOOT_MENU_SKIP_STATE_NONE: {
      break;
    };
    case BOOT_MENU_SKIP_STATE_MENU: {
      display_buffer_->printf(
          xPos, yPos, display_state_->get_font_small(),
          display_state_->get_color_palette()->get_accent_primary(),
          display::TextAlign::TOP_CENTER, "skip >");
      break;
    };
    case BOOT_MENU_SKIP_STATE_SLEEP: {
      display_buffer_->printf(
          xPos, yPos, display_state_->get_font_small(),
          display_state_->get_color_palette()->get_accent_primary(),
          display::TextAlign::TOP_CENTER, "sleep >");
      break;
    };
  }
}

int HomeThingMenuBoot::drawBootSequenceTitle(int xPos, int imageYPos,
                                             const MenuStates activeMenuState) {
  int yPos = imageYPos + display_state_->get_boot_logo_size() +
             display_state_->get_margin_size();
  int maxAnimationDuration = 0;
  switch (get_boot_menu_state()) {
    case BOOT_MENU_STATE_API:
      display_state_->drawTextWrapped(
          xPos, yPos, display_state_->get_font_large_heavy(),
          display_state_->get_color_palette()->get_accent_primary(),
          display::TextAlign::TOP_CENTER, "api connecting...", 4,
          display_buffer_);
      break;
    case BOOT_MENU_STATE_PLAYERS:
    case BOOT_MENU_STATE_COMPLETE: {
#ifdef USE_MEDIA_PLAYER_GROUP
      if (media_player_group_ && media_player_group_->totalPlayers() > 0) {
        int totalPlayers = media_player_group_->totalPlayers();
        int loadedPlayers = media_player_group_->loadedPlayers;
        auto playersLoadedString = to_string(loadedPlayers) + "/" +
                                   to_string(totalPlayers) +
                                   " media players loaded";
        display_state_->drawTextWrapped(
            xPos, yPos, display_state_->get_font_large_heavy(),
            display_state_->get_color_palette()->get_accent_primary(),
            display::TextAlign::TOP_CENTER, playersLoadedString, 5,
            display_buffer_);
      } else {
        display_state_->drawTextWrapped(
            xPos, yPos, display_state_->get_font_large_heavy(),
            display_state_->get_color_palette()->get_accent_primary(),
            display::TextAlign::TOP_CENTER, "api connected!", 4,
            display_buffer_);
      }
#endif
      break;
    }
    case BOOT_MENU_STATE_NETWORK:
      display_state_->drawTextWrapped(
          xPos, yPos, display_state_->get_font_large_heavy(),
          display_state_->get_color_palette()->get_accent_primary(),
          display::TextAlign::TOP_CENTER, "wifi connecting...", 4,
          display_buffer_);
      break;
    case BOOT_MENU_STATE_START:
      maxAnimationDuration =
          drawBootSequenceTitleRainbow(xPos, yPos, activeMenuState);
      break;
  }
  drawBootSequenceSkipTitle(xPos, imageYPos, activeMenuState);
  return maxAnimationDuration;
}

bool HomeThingMenuBoot::drawBootSequence(const MenuStates activeMenuState) {
  int imageYPos = display_state_->get_header_height() +
                  display_state_->get_margin_size() * 2;
  int xPos = display_buffer_->get_width() / 2;
  int maxAnimationDuration = 0;
  maxAnimationDuration =
      std::max(maxAnimationDuration, drawBootSequenceHeader(activeMenuState));
  maxAnimationDuration =
      std::max(maxAnimationDuration, drawBootSequenceLogo(xPos, imageYPos));
  maxAnimationDuration =
      std::max(maxAnimationDuration, drawBootSequenceLoadingBarAnimation());
  maxAnimationDuration =
      std::max(maxAnimationDuration,
               drawBootSequenceTitle(xPos, imageYPos, activeMenuState));
  const int animationTick = animation_->animationTick->state;
  if (animationTick <= maxAnimationDuration) {
    return true;
  } else {
    return get_boot_menu_state() == BOOT_MENU_STATE_COMPLETE;
  }
  return true;
}

BootMenuState HomeThingMenuBoot::get_boot_menu_state() {
  const int animationTick = animation_->animationTick->state;
  ESP_LOGD(TAG, "tick %d total %d", animationTick,
           animation_config_.total_animation_length());
  bool api = true;
  auto media_players = BOOT_MENU_STATE_COMPLETE;
#ifdef USE_BINARY_SENSOR
  if (api_connected_) {
    api = api_connected_->has_state() && api_connected_->state &&
          network::is_connected();
  }

  if (media_player_group_sensor_) {
    media_players = media_player_group_sensor_->state ? BOOT_MENU_STATE_COMPLETE
                                                      : BOOT_MENU_STATE_PLAYERS;
  }
#endif
  bool draw_animation = animation_->animationTick->state <
                            animation_config_.total_animation_length() &&
                        !boot_animation_complete_;
  if (!api && draw_animation) {
    return BOOT_MENU_STATE_START;
  } else if (!network::is_connected()) {
    return BOOT_MENU_STATE_NETWORK;
  } else if (!api) {
    return BOOT_MENU_STATE_API;
  } else {
    ESP_LOGD(TAG, "media players %d", media_players);
    return media_players;
  }
}
}  // namespace homething_menu_base
}  // namespace esphome
