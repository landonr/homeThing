#pragma once
#include <string>
#include <vector>
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/time/real_time_clock.h"

namespace esphome {
namespace homething_menu_base {

enum option_menuType {
  noOptionMenu,
  volumeOptionMenu,
  tvOptionMenu,
  speakerOptionMenu,
  playingNewSourceMenu
};
}  // namespace homething_menu_base
}  // namespace esphome
