#pragma once

#include <algorithm>
#include <memory>
#include <vector>
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/homeThing/homeThingMenuAnimation.h"
#include "esphome/components/homeThing/homeThingMenuDisplayState.h"
#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"

namespace esphome {
namespace homething_menu_base {

enum BootMenuState {
  BOOT_MENU_STATE_START,
  BOOT_MENU_STATE_NETWORK,
  BOOT_MENU_STATE_API,
  BOOT_MENU_STATE_PLAYERS,
  BOOT_MENU_STATE_COMPLETE
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
    return max(logo_config.delay_time + logo_config.animation_length,
               max(header_config.delay_time + header_config.animation_length,
                   loading_bar_config.delay_time +
                       loading_bar_config.animation_length));
  }
};

class HomeThingMenuBoot {
 public:
  HomeThingMenuBoot(display::DisplayBuffer* new_display_buffer,
                    HomeThingMenuDisplayState* new_display_state,
                    HomeThingMenuHeader* new_header,
                    homeassistant_media_player::HomeAssistantMediaPlayerGroup*
                        new_speaker_group)
      : display_buffer_(new_display_buffer),
        display_state_(new_display_state),
        header_(new_header),
        speaker_group_(new_speaker_group) {}
  bool drawBootSequence(const MenuStates activeMenuState);
  void skipBootSequence(const MenuStates activeMenuState);
  void set_api_connected(binary_sensor::BinarySensor* api_connected) {
    api_connected_ = api_connected;
  }
  void set_animation(HomeThingMenuAnimation* animation) {
    animation_ = animation;
  }
  bool boot_complete() {
    return get_boot_menu_state() == BOOT_MENU_STATE_COMPLETE;
  }

 private:
  bool bootSequenceCanSkip(const MenuStates activeMenuState);
  bool bootSequenceCanSleep(const MenuStates activeMenuState);
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
  HomeThingMenuDisplayState* display_state_{nullptr};
  HomeThingMenuAnimation* animation_{nullptr};
  HomeThingMenuHeader* header_{nullptr};
  homeassistant_media_player::HomeAssistantMediaPlayerGroup* speaker_group_;
  binary_sensor::BinarySensor* api_connected_{nullptr};
  const char* const TAG = "homething.boot";
  HomeThingMenuBootAnimationConfig animation_config_ =
      HomeThingMenuBootAnimationConfig();
};

}  // namespace homething_menu_base
}  // namespace esphome

// # interval:
// #   - interval: 1s
// #     then:
// #     - lambda: |-
// #         idleTick();
// #   - interval: 40ms
// #     then:
// #     - lambda: |-
// #         activeTick();
// #   - interval: 100ms
// #     then:
// #     - lambda: |-
// #         marqueeTick();
