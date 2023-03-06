#include "homeThingMenuBoot.h"
#include <algorithm>
#include "esphome/components/homeThing/homeThingMenuGlobals.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_menu_base {
bool HomeThingMenuBoot::bootSequenceCanSleep(const MenuStates activeMenuState) {
  return activeMenuState == bootMenu &&
         (!wifi_connected_->state || !api_connected_->state);
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
        display_state_->color_black_, display_state_->color_yellow_,
        display_state_->color_red_,   display_state_->color_pink_,
        display_state_->color_green_, display_state_->color_blue_};
    int textWidth = maxCharacters *
                    display_state_->get_large_font()->get_baseline() *
                    display_state_->font_size_width_ratio_;
    for (int i = 0; i < maxCharacters; i++) {
      int colorIndex = i <= currentAnimationTick ? currentAnimationTick - i : 0;
      if (colorIndex > 0) {
        auto color = colors.size() > colorIndex
                         ? colors[colorIndex]
                         : display_state_->color_accent_primary_;
        int characterXPos =
            xPos - textWidth / 2 +
            (i * display_state_->get_large_font()->get_baseline() *
             display_state_->font_size_width_ratio_);
        display_buffer_->printf(
            characterXPos, yPos, display_state_->get_large_heavy_font(), color,
            display::TextAlign::TOP_LEFT, "%c", bootTitle[i]);
      }
    }
  } else if (animationTick >= animationLength) {
    display_buffer_->printf(xPos, yPos, display_state_->get_large_heavy_font(),
                            display_state_->color_accent_primary_,
                            display::TextAlign::TOP_CENTER,
                            "wifi connecting...");
    if (animationTick >= showSleepLength &&
        bootSequenceCanSleep(activeMenuState)) {
      yPos = display_state_->getBottomBarYPosition(
                 false, display_buffer_->get_height()) -
             display_state_->margin_size_ / 2 -
             display_state_->get_small_font()->get_baseline();
      display_buffer_->printf(xPos, yPos, display_state_->get_small_font(),
                              display_state_->color_accent_primary_,
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
    display_buffer_->printf(xPos, imageYPos,
                            display_state_->get_menu_font_logo(), color,
                            display::TextAlign::TOP_CENTER, "");
  } else if (animationTick >= totalDuration) {
    display_buffer_->printf(
        xPos, imageYPos, display_state_->get_menu_font_logo(),
        display_state_->color_white_, display::TextAlign::TOP_CENTER, "");
  }
  return totalDuration;
}

int HomeThingMenuBoot::drawBootSequenceHeader(
    const MenuStates activeMenuState) {
  float animationLength = 8;
  int delayTime = 20;
  int totalDuration = delayTime + animationLength;
  int maxValue = display_state_->header_height_;
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
  if (api_connected_->state) {
    if (speaker_group_ != NULL) {
      float totalPlayers = static_cast<float>(speaker_group_->totalPlayers());
      float loadedPlayers = static_cast<float>(speaker_group_->loadedPlayers);
      float progress = 0.8 * (loadedPlayers / totalPlayers);
      return 0.25 + progress;
    }
    return 0.2;
  } else if (wifi_connected_->state) {
    return 0.1;
  } else {
    return 0;
  }
}

void HomeThingMenuBoot::drawBootSequenceLoadingBar(int yPosOffset,
                                                   float progress) {
  int barMargin = 1;
  int barHeight = display_state_->get_small_font()->get_baseline();
  int iconMargin = display_state_->get_small_font()->get_baseline() *
                   display_state_->font_size_width_ratio_ * 3;
  int totalBarWidth = display_buffer_->get_width() - iconMargin * 2;
  int barWidth = (totalBarWidth - 4) * progress;
  int yPos = display_state_->getBottomBarYPosition(
                 false, display_buffer_->get_height()) +
             yPosOffset;

  display_buffer_->rectangle(iconMargin, yPos, totalBarWidth, barHeight,
                             display_state_->color_accent_primary_);
  display_buffer_->filled_rectangle(
      iconMargin + barMargin * 2, yPos + barMargin * 2, barWidth,
      barHeight - 2 - barMargin * 2, display_state_->color_accent_primary_);
}

int HomeThingMenuBoot::drawBootSequenceLoadingBarAnimation() {
  float animationLength = 8;
  int delayTime = 20;
  int totalDuration = delayTime + animationLength;
  int maxValue = display_state_->get_small_font()->get_baseline() +
                 display_state_->bottom_bar_margin_;

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
  return activeMenuState == bootMenu && speaker_group_ != NULL &&
         speaker_group_->loadedPlayers > 0;
}

void HomeThingMenuBoot::drawBootSequenceSkipTitle(
    int xPos, int imageYPos, const MenuStates activeMenuState) {
  if (bootSequenceCanSkip(activeMenuState)) {
    int yPos = display_state_->getBottomBarYPosition(
                   false, display_buffer_->get_height()) -
               display_state_->margin_size_ / 2 -
               display_state_->get_small_font()->get_baseline();
    display_buffer_->printf(xPos, yPos, display_state_->get_small_font(),
                            display_state_->color_accent_primary_,
                            display::TextAlign::TOP_CENTER, "skip >");
  }
}

void HomeThingMenuBoot::skipBootSequence(const MenuStates activeMenuState) {
  if (!bootSequenceCanSkip(activeMenuState)) {
    if (bootSequenceCanSleep(activeMenuState)) {
      // sleep_toggle_->turn_on();
    }
    return;
  }
  speaker_group_->selectFirstActivePlayer();
}

int HomeThingMenuBoot::drawBootSequenceTitle(int xPos, int imageYPos,
                                             const MenuStates activeMenuState) {
  int yPos = imageYPos + display_state_->boot_logo_size_ +
             display_state_->margin_size_;
  int maxAnimationDuration = 0;
  if (api_connected_->state) {
    if (speaker_group_ != NULL) {
      int totalPlayers = speaker_group_->totalPlayers();
      int loadedPlayers = speaker_group_->loadedPlayers;
      display_buffer_->printf(
          xPos, yPos, display_state_->get_large_heavy_font(),
          display_state_->color_accent_primary_, display::TextAlign::TOP_CENTER,
          "%d/%d players loaded", loadedPlayers, totalPlayers);
    } else {
      display_buffer_->printf(xPos, yPos,
                              display_state_->get_large_heavy_font(),
                              display_state_->color_accent_primary_,
                              display::TextAlign::TOP_CENTER, "api connected!");
    }
  } else if (wifi_connected_->state) {
    display_buffer_->printf(xPos, yPos, display_state_->get_large_heavy_font(),
                            display_state_->color_accent_primary_,
                            display::TextAlign::TOP_CENTER,
                            "api connecting...");
  } else {
    maxAnimationDuration =
        drawBootSequenceTitleRainbow(xPos, yPos, activeMenuState);
  }
  drawBootSequenceSkipTitle(xPos, imageYPos, activeMenuState);
  return maxAnimationDuration;
}

bool HomeThingMenuBoot::drawBootSequence(const MenuStates activeMenuState) {
  speaker_group_->findActivePlayer();

  int imageYPos =
      display_state_->header_height_ + display_state_->margin_size_ * 2;
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
    if (wifi_connected_->state && api_connected_->state) {
      return false;
    }
  }
  return true;
}
}  // namespace homething_menu_base
}  // namespace esphome
