#include "homeThingMenuAnimation.h"
#include "esphome/components/homeThing/homeThingMenuGlobals.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_menu_base {

void HomeThingMenuAnimation::resetAnimation(bool force) {
  if (!force) {
    return;
  }
  animating = false;
  animationTick = 0;
}

void HomeThingMenuAnimation::activeTick() {
  if (!animating) {
    return;
  }
  // switch (activeMenuState) {
  //   case bootMenu:
  //     // displayUpdate.updateDisplay(true);
  //     return;
  //   default:
  //     return;
  // }
}

void HomeThingMenuAnimation::marqueeTick() {
  // sensors is the only menu with marqueed text currently
  // switch (activeMenuState) {
  //   case sensorsMenu:
  //     break;
  //   default:
  //     return;
  // }
  updateMarqueePosition();
}

void HomeThingMenuAnimation::updateMarqueePosition() {
  if ((charging || idleTime < 15) && idleTime > 1 && (animating || charging)) {
    if (animating) {
      tickAnimation();
    } else if (animationTick->state != 0) {
      animationTick->publish_state(0);
    }
    if (animationTick->state >= 0) {
      // displayUpdate.updateDisplay(true);
    }
  } else if (animationTick != 0) {
    animationTick->publish_state(0);
    if (animating) {
      // displayUpdate.updateDisplay(true);
    }
  }
}

void HomeThingMenuAnimation::tickAnimation() {
  // if ((charging || idleTime < 15) && idleTime > 1 && (animating || charging)) {
  if (animationTick->has_state()) {
    animationTick->publish_state(animationTick->state + 1);
  } else {
    animationTick->publish_state(0);
  }
  ESP_LOGI(TAG, "tick animation %f", animationTick->state);
  // }
}
}  // namespace homething_menu_base
}  // namespace esphome
