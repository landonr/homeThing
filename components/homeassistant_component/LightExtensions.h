#pragma once

#include <string>
#include "esphome/components/light/light_output.h"
#include "esphome/components/light/light_state.h"
#include "esphome/core/color.h"

namespace esphome {
namespace light {
bool supportsBrightness(light::LightState* light);
bool supportsColorTemperature(light::LightState* light);
bool supportsColor(light::LightState* light);
int get_hsv_color(light::LightState* light);
Color rgbLightColor(light::LightState* light);
std::string icon(light::LightState* light);
}  // namespace light
}  // namespace esphome
