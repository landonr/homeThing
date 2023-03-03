#pragma once
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "esphome/core/color.h"
#include "esphome/core/log.h"

namespace esphome {

class TextHelpers {
 public:
  static void tokenize(std::string const& str, std::string delim,
                       std::vector<std::string>* out) {
    size_t start;
    size_t end = 0;

    while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
      end = str.find(delim, start);
      out->push_back(str.substr(start, end - start));
    }
  }

  static int extractFirstNumber(std::string input) {
    // Remove the opening and closing parentheses
    input = input.substr(1, input.size() - 2);
    // Split the string on the comma
    std::istringstream iss(input);
    std::string first, second;
    std::getline(iss, first, ',');
    // Convert the first part to a int
    int num = std::stod(first);
    return num;
  }

  static Color hsvToRGB(double hue, double saturation, double value) {
    double r = 0, g = 0, b = 0;
    if (saturation == 0) {
      r = value;
      g = value;
      b = value;
    } else {
      int i;
      double f, p, q, t;

      if (hue == 360)
        hue = 0;
      else
        hue = hue / 60;

      i = static_cast<int>(trunc(hue));
      f = hue - i;

      p = value * (1.0 - saturation);
      q = value * (1.0 - (saturation * f));
      t = value * (1.0 - (saturation * (1.0 - f)));

      switch (i) {
        case 0:
          r = value;
          g = t;
          b = p;
          break;
        case 1:
          r = q;
          g = value;
          b = p;
          break;
        case 2:
          r = p;
          g = value;
          b = t;
          break;
        case 3:
          r = p;
          g = q;
          b = value;
          break;
        case 4:
          r = t;
          g = p;
          b = value;
          break;
        default:
          r = value;
          g = p;
          b = q;
          break;
      }
    }
    return Color((unsigned char)(r * 255), (unsigned char)(g * 255),
                 (unsigned char)(b * 255));
  }

 private:
  TextHelpers() {}
};

}  // namespace esphome
