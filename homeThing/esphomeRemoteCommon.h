#include <iterator>
#include <string>
#include <vector>

#include "esphome.h"
#pragma once

class StringUtils {
 public:
  static std::vector<std::string> split(const std::string& s,
                                        const std::string& delim) {
    size_t pos = 0;
    std::vector<std::string> elems;
    std::string token;
    std::string sCopy = s;
    while ((pos = sCopy.find(delim)) != std::string::npos) {
      elems.push_back(s.substr(0, pos));
      sCopy.erase(0, pos + delim.length());
    }
    // add rest of string to not miss the last part
    elems.push_back(sCopy);
    return elems;
  }
};
