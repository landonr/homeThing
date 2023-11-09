#pragma once

#ifdef USE_LIGHT
#include "esphome/components/homeThing/homeThingMenuScreen.h"
#include "esphome/components/light/light_state.h"
#include "esphome/core/color.h"

namespace esphome {
namespace homething_menu_base {

static const char* const LIGHT_HELPERS_TAG = "homething.light.helpers";
class HomeThingLightHelpers {
 public:
  static void toggleLight(light::LightState* light) {
    if (light) {
      if (!light->remote_values.is_on() &&
          light->remote_values.get_brightness() == 0) {
        auto call = light->make_call();
        call.set_state(true);
        call.set_brightness(1.0);
        auto traits = light->get_traits();
        if (traits.supports_color_mode(light::ColorMode::RGB)) {
          call.set_color_mode(light::ColorMode::RGB);
          call.set_rgb(1, 1, 1);
        } else if (traits.supports_color_mode(light::ColorMode::RGB_WHITE)) {
          call.set_color_mode(light::ColorMode::RGB_WHITE);
          call.set_rgbw(1, 1, 1, 1);
        }
        call.perform();
        return;
      }
      light->toggle().perform();
    }
  }

  static void decTemperature(light::LightState* light) {
    if (light) {
      auto call = light->make_call();
      call.set_color_mode(light::ColorMode::COLOR_TEMPERATURE);
      if (!light->remote_values.is_on()) {
        call.set_state(true);
        call.set_brightness(0.1);
      }
      auto traits = light->get_traits();
      float mired_step = static_cast<float>(traits.get_max_mireds() -
                                            traits.get_min_mireds()) /
                         20.0;
      float color_temperature =
          light->remote_values.get_color_temperature() - mired_step;
      call.set_color_temperature(
          std::max(traits.get_min_mireds(), color_temperature));
      call.perform();
    }
  }

  static void incTemperature(light::LightState* light) {
    if (light) {
      auto call = light->make_call();
      call.set_color_mode(light::ColorMode::COLOR_TEMPERATURE);
      if (!light->remote_values.is_on()) {
        call.set_state(true);
        call.set_brightness(0.1);
      }
      auto traits = light->get_traits();
      float mired_step = static_cast<float>(traits.get_max_mireds() -
                                            traits.get_min_mireds()) /
                         20.0;
      float color_temperature =
          light->remote_values.get_color_temperature() + mired_step;
      call.set_color_temperature(
          std::min(traits.get_max_mireds(), color_temperature));
      call.perform();
    }
  }
  static void decBrightness(light::LightState* light) {
    if (light) {
      auto call = light->make_call();
      auto brightness = light->remote_values.get_brightness() - 0.1;
      if (brightness > 0) {
        call.set_brightness(brightness);
        ESP_LOGI(LIGHT_HELPERS_TAG, "'%s': brightness decreased to %f",
                 light->get_name().c_str(), brightness);
      } else {
        call.set_state(false);
        ESP_LOGI(LIGHT_HELPERS_TAG, "'%s': brightness decreased to off",
                 light->get_name().c_str());
      }
      call.perform();
    }
  }
  static void incBrightness(light::LightState* light) {
    if (light) {
      auto call = light->make_call();
      if (light->remote_values.is_on()) {
        auto brightness = light->remote_values.get_brightness() + 0.1;
        call.set_brightness(std::min(1.0, brightness));
        ESP_LOGI(LIGHT_HELPERS_TAG, "'%s': brightness increased to %f",
                 light->get_name().c_str(), brightness);
      } else {
        call.set_state(true);
        call.set_brightness(0.1);
        ESP_LOGI(LIGHT_HELPERS_TAG, "'%s': brightness set to %f",
                 light->get_name().c_str(), 0.1);
      }
      call.perform();
    }
  }
  static void toggle(light::LightState* light) {
    if (light) {
      light->toggle().perform();
    }
  }

  static void update_color_with_hsv(const float hsv_color,
                                    light::LightState* light) {
    if (light) {
      auto call = light->make_call();
      auto traits = light->get_traits();
      if (traits.supports_color_mode(light::ColorMode::RGB)) {
        call.set_color_mode(light::ColorMode::RGB);
      } else if (traits.supports_color_mode(light::ColorMode::RGB_WHITE)) {
        call.set_color_mode(light::ColorMode::RGB_WHITE);
      } else {
        ESP_LOGI(LIGHT_HELPERS_TAG, "'%s': doesnt support color!",
                 light->get_name().c_str());
        return;
      }
      if (!light->remote_values.is_on()) {
        call.set_state(true);
        call.set_brightness(0.1);
      }
      float red, green, blue;
      hsv_to_rgb(hsv_color, 1, 1, red, green, blue);
      call.set_rgb(red, green, blue);
      call.perform();
    }
  }

  static void decColor(light::LightState* light) {
    if (light) {
      float color_step = 360.0f / 20.0f;
      float hsv_color = std::max(0.1f, get_hsv_color(light) - color_step);
      update_color_with_hsv(hsv_color, light);
    }
  }
  static void incColor(light::LightState* light) {
    if (light) {
      float color_step = 360.0f / 20.0f;
      float hsv_color = std::min(359.9f, get_hsv_color(light) + color_step);
      update_color_with_hsv(hsv_color, light);
    }
  }
};
}  // namespace homething_menu_base
}  // namespace esphome
#endif