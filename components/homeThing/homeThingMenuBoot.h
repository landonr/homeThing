#pragma once

#include <memory>
#include <vector>
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/homeThing/homeThingMenuAnimation.h"
#include "esphome/components/homeThing/homeThingMenuDisplayState.h"
#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"

namespace esphome {
namespace homething_menu_base {

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
        speakerGroup(new_speaker_group) {}
  bool bootSequenceCanSkip();
  bool bootSequenceCanSleep();
  float bootSequenceLoadingProgress();
  int drawBootSequenceHeader();
  int drawBootSequenceLoadingBarAnimation();
  int drawBootSequenceLogo(int xPos, int imageYPos);
  int drawBootSequenceTitle(int xPos, int imageYPos);
  int drawBootSequenceTitleRainbow(int xPos, int yPos);
  void drawBootSequence();
  void drawBootSequenceLoadingBar(int yPosOffset, float progress);
  void drawBootSequenceSkipTitle(int xPos, int imageYPos);
  void skipBootSequence();
  void set_wifi_connected(binary_sensor::BinarySensor* wifi_connected) {
    wifi_connected_ = wifi_connected;
  }
  void set_api_connected(binary_sensor::BinarySensor* api_connected) {
    api_connected_ = api_connected;
  }
  void set_animation(HomeThingMenuAnimation* animation) {
    animation_ = animation;
  }

 private:
  bool setupFinished = false;
  display::DisplayBuffer* display_buffer_{nullptr};
  HomeThingMenuDisplayState* display_state_{nullptr};
  HomeThingMenuAnimation* animation_{nullptr};
  HomeThingMenuHeader* header_{nullptr};
  homeassistant_media_player::HomeAssistantMediaPlayerGroup* speakerGroup;
  display::Font* home_thing_logo_{nullptr};
  binary_sensor::BinarySensor* wifi_connected_{nullptr};
  binary_sensor::BinarySensor* api_connected_{nullptr};
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
