#include "esphome.h"
#include <iterator>
#include <vector>
#pragma once

class StringUtils {
 public:
  static std::vector<std::string> split(std::string &s, const std::string &delim) {
    size_t pos = 0;
    std::vector<std::string> elems;
    std::string token;
    while ((pos = s.find(delim)) != std::string::npos) {
      elems.push_back(s.substr(0, pos));
      s.erase(0, pos + delim.length());
    }
    // add rest of string to not miss the last part
    elems.push_back(s);
    return elems;
  }
};
