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
  // switch (activeMenuState) {
  //   case bootMenu:
  //     // displayUpdate.updateDisplay(true);
  //     return;
  //   default:
  //     return;
  // }
}

// void HomeThingMenuAnimation::marqueeTick(int titleLength) {
//   // sensors is the only menu with marqueed text currently
//   // switch (activeMenuState) {
//   //   case sensorsMenu:
//   //     break;
//   //   default:
//   //     return;
//   // }
//   ESP_LOGI(TAG, "marquee tick %f", animationTick->state);
//   updateMarqueePosition(titleLength);
// }

// void HomeThingMenuAnimation::updateMarqueePosition(int titleLength) {
//   ESP_LOGD(TAG, "charging %d idle %d animating %d", charging, idleTime,
//            animating);
//   if ((charging || idleTime < 15) && idleTime > 1 && (animating || charging)) {
//     ESP_LOGD(TAG, "1tick animation %f", animationTick->state);
//     if (animating && animationTick->state < titleLength - 4) {
//       ESP_LOGD(TAG, "2tick animation %f", animationTick->state);
//       tickAnimation();
//     } else {
//       animationTick->publish_state(0);
//     }
//   } else if (animationTick->state != 0) {
//     animationTick->publish_state(0);
//   }
// }

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
