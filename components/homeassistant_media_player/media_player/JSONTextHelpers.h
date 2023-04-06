#pragma once

#include "esphome/components/json/json_util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_media_player {

static std::string replaceAll(std::string str, const std::string& from,
                              const std::string& to) {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos +=
        to.length();  // Handles case where 'to' is a substring of 'from'
  }
  return str;
}
}  // namespace homeassistant_media_player
}  // namespace esphome
