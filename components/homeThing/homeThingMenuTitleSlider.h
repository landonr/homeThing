#pragma once

#include "homeThingMenuTitle.h"

#include <string>

namespace esphome {
namespace homething_menu_base {
class MenuTitleSlider : public MenuTitleBase {
 public:
  bool slider_{false};
  bool currentState;
  int sliderValue;
  int displayValue;
  std::string sliderUnit;
  int value_min_;
  int value_max_;
  MenuTitleSlider(std::string newTitle, std::string newEntityId,
                  MenuTitleRightIcon newRightIconState, int newSliderValue,
                  int newDisplayValue, std::string newSliderUnit, int value_min,
                  int value_max)
      : MenuTitleBase{newTitle, newEntityId, newRightIconState,
                      SliderMenuTitleType},
        sliderValue(newSliderValue),
        displayValue(newDisplayValue),
        sliderUnit(newSliderUnit),
        value_min_(value_min),
        value_max_(value_max) {}

  MenuTitleSlider(std::string title, std::string unit, std::string entity_id_,
                  int min, int max, int value, int displayUnitMin,
                  int displayUnitMax)
      : MenuTitleBase{title, entity_id_, NoMenuTitleRightIcon,
                      SliderMenuTitleType},
        sliderValue(value),
        displayValue(value),
        sliderUnit(unit),
        value_min_(min),
        value_max_(max) {
    float oldRange = max - min;
    float valueMinusMin = value - min;
    if (value > 0) {
      float displayNewRange = displayUnitMax - displayUnitMin;
      displayValue =
          static_cast<float>(((valueMinusMin * displayNewRange) / oldRange)) +
          displayUnitMin;
    }

    sliderValue = value;
  }

  float percent_value() const {
    float value_minus_min = sliderValue - value_min_;
    float old_range = value_max_ - value_min_;
    return value_minus_min / old_range;
  }
};
}  // namespace homething_menu_base
}  // namespace esphome
