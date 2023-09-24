#pragma once

#include <algorithm>
#include <memory>
#include <vector>
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/homeThing/homeThingMenuAnimation.h"
#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeThingDisplayState/homeThingDisplayState.h"
#include "esphome/components/homeThingDisplayState/homeThingMenuTextHelpers.h"
#ifdef USE_MEDIA_PLAYER_GROUP
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"
#endif
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace homething_menu_base {

enum BootMenuState {
  BOOT_MENU_STATE_START,
  BOOT_MENU_STATE_NETWORK,
  BOOT_MENU_STATE_API,
  BOOT_MENU_STATE_PLAYERS,
  BOOT_MENU_STATE_COMPLETE
};

enum BootMenuSkipState {
  BOOT_MENU_SKIP_STATE_NONE,
  BOOT_MENU_SKIP_STATE_SLEEP,
  BOOT_MENU_SKIP_STATE_MENU
};

struct HomeThingMenuAnimationConfig {
  int delay_time;
  int animation_length;
};

class HomeThingMenuBootAnimationConfig {
 public:
  HomeThingMenuAnimationConfig logo_config = {2, 6};
  HomeThingMenuAnimationConfig header_config = {8, 20};
  HomeThingMenuAnimationConfig loading_bar_config = {20, 9};

  int total_animation_length() {
    return std::max(
        logo_config.delay_time + logo_config.animation_length,
        std::max(header_config.delay_time + header_config.animation_length,
                 loading_bar_config.delay_time +
                     loading_bar_config.animation_length));
  }
};

class HomeThingMenuBoot {
 public:
  HomeThingMenuBoot(
      display::DisplayBuffer* new_display_buffer,
      homething_display_state::HomeThingDisplayState* new_display_state,
      HomeThingMenuHeader* new_header)
      : display_buffer_(new_display_buffer),
        display_state_(new_display_state),
        header_(new_header) {}
  bool drawBootSequence(const MenuStates activeMenuState);
  BootMenuSkipState bootSequenceCanSkip(const MenuStates activeMenuState);
  void set_api_connected(binary_sensor::BinarySensor* api_connected) {
    api_connected_ = api_connected;
  }
  void set_animation(HomeThingMenuAnimation* animation) {
    animation_ = animation;
  }
  bool boot_complete() {
    return get_boot_menu_state() == BOOT_MENU_STATE_COMPLETE;
  }
#ifdef USE_MEDIA_PLAYER_GROUP
  void set_media_player_group(
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          media_player_group) {
    media_player_group_ = media_player_group;
  }
#endif

 private:
  void drawBootSequenceLoadingBar(int yPosOffset, float progress);
  void drawBootSequenceSkipTitle(int xPos, int imageYPos,
                                 const MenuStates activeMenuState);
  int drawBootSequenceTitleRainbow(int xPos, int yPos,
                                   const MenuStates activeMenuState);
  float bootSequenceLoadingProgress();
  int drawBootSequenceHeader(const MenuStates activeMenuState);
  int drawBootSequenceLoadingBarAnimation();
  int drawBootSequenceLogo(int xPos, int imageYPos);
  int drawBootSequenceTitle(int xPos, int imageYPos,
                            const MenuStates activeMenuState);

  BootMenuState get_boot_menu_state();
  display::DisplayBuffer* display_buffer_{nullptr};
  homething_display_state::HomeThingDisplayState* display_state_{nullptr};
  HomeThingMenuAnimation* animation_{nullptr};
  HomeThingMenuHeader* header_{nullptr};
#ifdef USE_MEDIA_PLAYER_GROUP
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
      media_player_group_{nullptr};
#endif
  binary_sensor::BinarySensor* api_connected_{nullptr};
  const char* const TAG = "homething.boot";
  HomeThingMenuBootAnimationConfig animation_config_ =
      HomeThingMenuBootAnimationConfig();
  bool boot_animation_complete_ = false;
};

}  // namespace homething_menu_base
}  // namespace esphome
