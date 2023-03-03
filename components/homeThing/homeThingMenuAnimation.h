#pragma once

namespace esphome {
namespace homething_menu_base {

class HomeThingMenuAnimation {
 public:
  void resetAnimation(bool force = false);
  void updateMarqueePosition();
  void marqueeTick();
  void activeTick();
  int animationTick = 0;
  bool animating = true;  // boot sequence is animated
  void tickAnimation();
};

}  // namespace homething_menu_base
}  // namespace esphome
