#include "esphomeRemote.h"

#pragma once

enum RemoteLightState { OffRemoteLightState, OnRemoteLightState, ChargingLightState, EffectRemoteLightState };

RemoteLightState lightState = OffRemoteLightState;
int previousBatteryLevel = 0;

void updateLight(double brightness, double red, double green, double blue) {
  auto offCall = id(side_light).turn_off();
  offCall.perform();
  auto call = id(side_light).turn_on();
  call.set_brightness(brightness);
  call.set_rgb(red, green, blue);
  call.perform();
}

void manageLight() {
  if (!id(backlight).state) {
    if (lightState != OffRemoteLightState) {
      auto call = id(side_light).turn_off();
      call.perform();
      lightState = OffRemoteLightState;
    }
  } else if (lightState == OffRemoteLightState && !speakerGroup->playerSearchFinished) {
    auto call = id(side_light).turn_on();
    call.set_effect("Rainbow Effect");
    call.perform();
    lightState = EffectRemoteLightState;
  } else if (speakerGroup->playerSearchFinished) {
    if (!charging && lightState != OnRemoteLightState) {
      updateLight(0.2, 0.75, 0.5, 1.0);
      lightState = OnRemoteLightState;
    } else if (charging && lightState != ChargingLightState) {
      updateLight(0.5, 1.0, 0.75, 0.1);
      lightState = ChargingLightState;
    }
  }
}
