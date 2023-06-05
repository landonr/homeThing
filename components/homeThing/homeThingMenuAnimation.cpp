#include "homeThingMenuAnimation.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_menu_base {

void HomeThingMenuAnimation::resetAnimation(bool force) {
  // if (!force) {
  //   return;
  // }
  ESP_LOGD(TAG, "resetAnimation: %f force", animationTick->state, force);
  if (animationTick->state != 0 || force) {
    animating = false;
    animationTick->publish_state(0);
  }
}

void HomeThingMenuAnimation::activeTick() {
  if (!animating) {
    return;
  }
}

void HomeThingMenuAnimation::tickAnimation() {
  // if ((charging || idleTime < 15) && idleTime > 1 && (animating || charging)) {
  if (animationTick->has_state()) {
    animationTick->publish_state(animationTick->state + 1);
  } else {
    animationTick->publish_state(0);
  }
  ESP_LOGD(TAG, "tick animation %f", animationTick->state);
  // }
}
}  // namespace homething_menu_base
}  // namespace esphome
