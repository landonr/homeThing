#include "esphomeRemote.h"

#ifndef REMOTEFIRELIGHT
#define REMOTEFIRELIGHT

enum RemoteLightState {
  OffRemoteLightState,
  OnRemoteLightState,
  EffectRemoteLightState
};

RemoteLightState lightState = OffRemoteLightState;

void manageLight() {
  if (!id(backlight).state) {
    if (lightState != OffRemoteLightState) {
      auto call = id(side_light).turn_off();
      call.perform();
      lightState = OffRemoteLightState;
    }
  } else if (lightState == OffRemoteLightState && !speakerGroup -> playerSearchFinished) {
    auto call = id(side_light).turn_on();
    call.set_effect("Rainbow Effect");
    call.perform();
    lightState = EffectRemoteLightState;
  } else if (lightState != OnRemoteLightState && speakerGroup -> playerSearchFinished) {
    auto offCall = id(side_light).turn_off();
    offCall.perform();
    auto call = id(side_light).turn_on();
    call.set_brightness(0.5);
    call.set_rgb(0.5, 0.25, 1.0);
    call.perform();
    lightState = OnRemoteLightState;
  }
}

#endif