#pragma once
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace homething_menu_base {

class HomeThingMenuAnimation {
 public:
  HomeThingMenuAnimation() {
    animationTick = new sensor::Sensor();
    animationTick->publish_state(0);
    auto filter = new sensor::DebounceFilter(17);
    animationTick->add_filter(filter);
  }
  void resetAnimation(bool force = false);
  void updateMarqueePosition(int titleLength);
  // void marqueeTick(int titleLength);
  void activeTick();
  sensor::Sensor* animationTick;
  bool animating = true;  // boot sequence is animated
  void tickAnimation();

 private:
  const char* const TAG = "homething.animation";
};

}  // namespace homething_menu_base
}  // namespace esphome
