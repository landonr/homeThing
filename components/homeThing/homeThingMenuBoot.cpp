#include "homeThingMenuBoot.h"
#include <algorithm>
#include "esphome/components/homeThing/homeThingMenuGlobals.h"
#include "esphome/components/network/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_menu_base {
bool HomeThingMenuBoot::bootSequenceCanSleep(const MenuStates activeMenuState) {
  return activeMenuState == bootMenu &&
         get_boot_menu_state() < BOOT_MENU_STATE_API;
}

int HomeThingMenuBoot::drawBootSequenceTitleRainbow(
    int xPos, int yPos, const MenuStates activeMenuState) {
  std::string bootTitle = "homeThing";
  int loopLimit = bootTitle.length();
  int delayTime = 6;
  int animationStartTime = delayTime;
  int animationLength = animationStartTime + loopLimit + delayTime * 2;
  int showSleepLength = animationLength + delayTime * 2;
  const int animationTick = animation_->animationTick->state;
  if (animationTick > animationStartTime && animationTick < animationLength) {
    int currentAnimationTick = animationTick - animationStartTime;
    int activeCharacter = currentAnimationTick;
    int maxCharacters = loopLimit;
    std::vector<Color> colors = {
        display_state_->get_color_black(), display_state_->get_color_yellow(),
        display_state_->get_color_red(),   display_state_->get_color_pink(),
        display_state_->get_color_green(), display_state_->get_color_blue()};
    int textWidth = maxCharacters *
                    display_state_->get_font_large()->get_baseline() *
                    display_state_->get_font_size_width_ratio();
    for (int i = 0; i < maxCharacters; i++) {
      int colorIndex = i <= currentAnimationTick ? currentAnimationTick - i : 0;
      if (colorIndex > 0) {
        auto color = colors.size() > colorIndex
                         ? colors[colorIndex]
                         : display_state_->get_color_accent_primary();
        int characterXPos =
            xPos - textWidth / 2 +
            (i * display_state_->get_font_large()->get_baseline() *
             display_state_->get_font_size_width_ratio());
        display_buffer_->printf(
            characterXPos, yPos, display_state_->get_font_large_heavy(), color,
            display::TextAlign::TOP_LEFT, "%c", bootTitle[i]);
      }
    }
  } else if (animationTick >= animationLength) {
    if (animationTick >= showSleepLength &&
        bootSequenceCanSleep(activeMenuState)) {
      yPos = display_state_->getBottomBarYPosition(
                 false, display_buffer_->get_height()) -
             display_state_->get_margin_size() / 2 -
             display_state_->get_font_small()->get_baseline();
      display_buffer_->printf(xPos, yPos, display_state_->get_font_small(),
                              display_state_->get_color_accent_primary(),
                              display::TextAlign::TOP_CENTER, "sleep >");
    }
  }
  return showSleepLength;
}

int HomeThingMenuBoot::drawBootSequenceLogo(int xPos, int imageYPos) {
  float animationLength = 6;
  int delayTime = 2;
  int totalDuration = delayTime + animationLength;
  const int animationTick = animation_->animationTick->state;
  if (animationTick > delayTime && animationTick < totalDuration) {
    int colorValue =
        (static_cast<float>(animationTick - delayTime) / animationLength) * 255;
    auto color = Color(colorValue, colorValue, colorValue);
    display_buffer_->printf(xPos, imageYPos, display_state_->get_font_logo(),
                            color, display::TextAlign::TOP_CENTER, "");
  } else if (animationTick >= totalDuration) {
    display_buffer_->printf(xPos, imageYPos, display_state_->get_font_logo(),
                            display_state_->get_color_white(),
                            display::TextAlign::TOP_CENTER, "");
  }
  return totalDuration;
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
      if (media_player_group_ != NULL) {
        float totalPlayers =
            static_cast<float>(media_player_group_->totalPlayers());
        float loadedPlayers =
            static_cast<float>(media_player_group_->loadedPlayers);
        float progress = 0.7 * (loadedPlayers / totalPlayers);
        return 0.3 + progress;
      }
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

  display_buffer_->rectangle(iconMargin, yPos, totalBarWidth, barHeight,
                             display_state_->get_color_accent_primary());
  display_buffer_->filled_rectangle(iconMargin + barMargin * 2,
                                    yPos + barMargin * 2, barWidth,
                                    barHeight - 2 - barMargin * 2,
                                    display_state_->get_color_accent_primary());
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

bool HomeThingMenuBoot::bootSequenceCanSkip(const MenuStates activeMenuState) {
  return activeMenuState == bootMenu && media_player_group_ != NULL &&
         media_player_group_->loadedPlayers > 0;
}

void HomeThingMenuBoot::drawBootSequenceSkipTitle(
    int xPos, int imageYPos, const MenuStates activeMenuState) {
  if (bootSequenceCanSkip(activeMenuState)) {
    int yPos = display_state_->getBottomBarYPosition(
                   false, display_buffer_->get_height()) -
               display_state_->get_margin_size() / 2 -
               display_state_->get_font_small()->get_baseline();
    display_buffer_->printf(xPos, yPos, display_state_->get_font_small(),
                            display_state_->get_color_accent_primary(),
                            display::TextAlign::TOP_CENTER, "skip >");
  }
}

void HomeThingMenuBoot::skipBootSequence(const MenuStates activeMenuState) {
  if (!bootSequenceCanSkip(activeMenuState)) {
    if (bootSequenceCanSleep(activeMenuState)) {
      // sleep_switch_->turn_on();
    }
    return;
  }
  media_player_group_->selectFirstActivePlayer();
}

int HomeThingMenuBoot::drawBootSequenceTitle(int xPos, int imageYPos,
                                             const MenuStates activeMenuState) {
  int yPos = imageYPos + display_state_->get_boot_logo_size() +
             display_state_->get_margin_size();
  int maxAnimationDuration = 0;
  switch (get_boot_menu_state()) {
    case BOOT_MENU_STATE_API:
      display_buffer_->printf(
          xPos, yPos, display_state_->get_font_large_heavy(),
          display_state_->get_color_accent_primary(),
          display::TextAlign::TOP_CENTER, "api connecting...");
      break;
    case BOOT_MENU_STATE_PLAYERS:
    case BOOT_MENU_STATE_COMPLETE: {
      if (media_player_group_ && media_player_group_->totalPlayers() > 0) {
        int totalPlayers = media_player_group_->totalPlayers();
        int loadedPlayers = media_player_group_->loadedPlayers;
        display_buffer_->printf(
            xPos, yPos, display_state_->get_font_large_heavy(),
            display_state_->get_color_accent_primary(),
            display::TextAlign::TOP_CENTER, "%d/%d players loaded",
            loadedPlayers, totalPlayers);
      } else {
        display_buffer_->printf(
            xPos, yPos, display_state_->get_font_large_heavy(),
            display_state_->get_color_accent_primary(),
            display::TextAlign::TOP_CENTER, "api connected!");
      }
      break;
    }
    case BOOT_MENU_STATE_NETWORK:
      display_buffer_->printf(
          xPos, yPos, display_state_->get_font_large_heavy(),
          display_state_->get_color_accent_primary(),
          display::TextAlign::TOP_CENTER, "wifi connecting...");
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
  if (media_player_group_) {
    media_player_group_->findActivePlayer();
  }

  int imageYPos = display_state_->get_header_height() +
                  display_state_->get_margin_size() * 2;
  int xPos = display_buffer_->get_width() / 2;
  int maxAnimationDuration = 0;
  maxAnimationDuration =
      max(maxAnimationDuration, drawBootSequenceHeader(activeMenuState));
  maxAnimationDuration =
      max(maxAnimationDuration, drawBootSequenceLogo(xPos, imageYPos));
  maxAnimationDuration =
      max(maxAnimationDuration, drawBootSequenceLoadingBarAnimation());
  maxAnimationDuration =
      max(maxAnimationDuration,
          drawBootSequenceTitle(xPos, imageYPos, activeMenuState));
  const int animationTick = animation_->animationTick->state;
  if (animationTick < maxAnimationDuration) {
    return true;
  } else {
    return get_boot_menu_state() == BOOT_MENU_STATE_COMPLETE;
  }
  return true;
}

BootMenuState HomeThingMenuBoot::get_boot_menu_state() {
  if (api_connected_->has_state() && api_connected_->state) {
    if (media_player_group_) {
      int totalPlayers = media_player_group_->totalPlayers();
      int loadedPlayers = media_player_group_->loadedPlayers;
      return totalPlayers == loadedPlayers ? BOOT_MENU_STATE_COMPLETE
                                           : BOOT_MENU_STATE_PLAYERS;
    }
    return BOOT_MENU_STATE_COMPLETE;
  } else if (network::is_connected()) {
    return BOOT_MENU_STATE_API;
  } else if (animation_->animationTick->state <
             animation_config_.total_animation_length()) {
    return BOOT_MENU_STATE_START;
  } else {
    return BOOT_MENU_STATE_NETWORK;
  }
}
}  // namespace homething_menu_base
}  // namespace esphome
