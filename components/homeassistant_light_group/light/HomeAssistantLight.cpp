#include "HomeAssistantLight.h"
#include <utility>
#include "esphome/core/log.h"

namespace esphome {
namespace homeassistant_light {

static const char* const TAG = "homeassistant.light";

void HomeAssistantLight::subscribe_states() {
  ESP_LOGI(TAG, "'%s': Subscribe states", get_name().c_str());
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, {},
      std::bind(&HomeAssistantLight::state_changed, this,
                std::placeholders::_1));
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("min_mireds"),
      std::bind(&HomeAssistantLight::min_mireds_changed, this,
                std::placeholders::_1));
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("max_mireds"),
      std::bind(&HomeAssistantLight::max_mireds_changed, this,
                std::placeholders::_1));
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("brightness_changed"),
      std::bind(&HomeAssistantLight::brightness_changed, this,
                std::placeholders::_1));
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("color_temp"),
      std::bind(&HomeAssistantLight::color_temp_changed, this,
                std::placeholders::_1));
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("hs_color"),
      std::bind(&HomeAssistantLight::color_changed, this,
                std::placeholders::_1));
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("color_mode"),
      std::bind(&HomeAssistantLight::color_mode_changed, this,
                std::placeholders::_1));
  api::global_api_server->subscribe_home_assistant_state(
      this->entity_id_, optional<std::string>("supported_color_modes"),
      std::bind(&HomeAssistantLight::supported_color_modes_changed, this,
                std::placeholders::_1));
}

void HomeAssistantLight::setup() {
  ESP_LOGI(TAG, "'%s': Setup", get_name().c_str());
  subscribe_states();
}

light::LightTraits HomeAssistantLight::get_traits() {
  return light_traits_;
}

void HomeAssistantLight::setup_state(light::LightState* state) {
  ESP_LOGI(TAG, "'%s': Setting up state", get_name().c_str());
  light_state_ = state;
}

void HomeAssistantLight::update_state(light::LightState* state) {
  bool is_on = state->remote_values.is_on();
  auto brightness =
      static_cast<int>(state->remote_values.get_brightness() * 255);
  auto color_temp =
      static_cast<int>(state->remote_values.get_color_temperature() * 500);
  ESP_LOGI(TAG, "'%s': Update state %d %d %d", get_name().c_str(), is_on,
           brightness, color_temp);
}

void HomeAssistantLight::publish_api_state(light::LightState* state) {
  next_api_publish_ = false;
  if (state->remote_values.is_on()) {
    std::map<std::string, std::string> data = {
        {"entity_id", entity_id_.c_str()}};

    auto brightness =
        static_cast<int>(state->remote_values.get_brightness() * 255);
    auto color_temp =
        static_cast<int>(state->remote_values.get_color_temperature() * 500);

    if (brightness != 0) {
      data["brightness"] = to_string(brightness);
    }
    if (color_temp != 0) {
      data["color_temp"] = to_string(color_temp);
    }
    call_homeassistant_service("light.turn_on", data);
  } else {
    const std::map<std::string, std::string> data = {
        {"entity_id", entity_id_.c_str()},
    };
    call_homeassistant_service("light.turn_off", data);
  }
}

void HomeAssistantLight::write_state(light::LightState* state) {
  bool is_on = state->remote_values.is_on();
  auto brightness =
      static_cast<int>(state->remote_values.get_brightness() * 255);
  auto color_temp =
      static_cast<int>(state->remote_values.get_color_temperature() * 500);
  ESP_LOGI(TAG, "'%s': Writing state %d %d %d", get_name().c_str(), is_on,
           brightness, color_temp);

  if (next_api_publish_) {
    publish_api_state(state);
  }
}

void HomeAssistantLight::add_on_state_callback(
    std::function<void()>&& callback) {
  this->state_callback_.add(std::move(callback));
}

void HomeAssistantLight::increaseProperty(int max, bool* inSync,
                                          int* localValue, int incStep,
                                          std::string serviceProperty,
                                          bool wrapData) {
  // if (id(keep_states_in_sync)) {
  if (!*inSync) {
    return;
  }
  *inSync = false;
  // }
  if (*localValue + incStep >= max) {
    *localValue = max;
  } else {
    *localValue += incStep;
  }
  if (wrapData) {
    const std::map<std::string, std::string> data = {
        {"entity_id", entity_id_.c_str()},
        {"hue", to_string(*localValue).c_str()},
        {"saturation", "100"},
    };
    // call_homeassistant_service("script.hs_light_set", data);
  } else {
    const std::map<std::string, std::string> data = {
        {"entity_id", entity_id_.c_str()},
        {serviceProperty, to_string(*localValue).c_str()},
    };
    setAttribute(data);
  }
}

void HomeAssistantLight::decreaseProperty(int min, bool* inSync,
                                          int* localValue, int decStep,
                                          std::string serviceProperty,
                                          bool wrapData) {
  // if (id(keep_states_in_sync)) {
  if (!*inSync) {
    return;
  }
  *inSync = false;
  // }
  if (*localValue - decStep <= min) {
    *localValue = min;
  } else {
    *localValue -= decStep;
  }
  if (wrapData) {
    const std::map<std::string, std::string> data = {
        {"entity_id", entity_id_.c_str()},
        {"hue", to_string(*localValue).c_str()},
        {"saturation", "100"},
    };
    // call_homeassistant_service("script.hs_light_set", data);
  } else {
    const std::map<std::string, std::string> data = {
        {"entity_id", entity_id_.c_str()},
        {serviceProperty, to_string(*localValue).c_str()},
    };
    setAttribute(data);
  }
}

void HomeAssistantLight::decTemperature() {
  // decreaseProperty(minMireds, &isColorTempInSync, &localColorTemp,
  //                  id(dec_color_temperature_step), "color_temp");
}

void HomeAssistantLight::incTemperature() {
  // increaseProperty(maxMireds, &isColorTempInSync, &localColorTemp,
  //                  id(inc_color_temperature_step), "color_temp");
}

void HomeAssistantLight::decBrightness() {
  // decreaseProperty(0, &isBrightnessInSync, &localBrightness,
  //                  id(dec_brightness_step), "brightness");
}

void HomeAssistantLight::incBrightness() {
  // increaseProperty(MAX_BRIGHTNESS, &isBrightnessInSync, &localBrightness,
  //                  id(inc_brightness_step), "brightness");
}

void HomeAssistantLight::decColor() {
  // if (localColorTemp != -1) {
  //   localColorTemp = -1;
  // }
  // bool temp = true;
  // decreaseProperty(0, &temp, &localColor, 10, "hs_color", true);
}

void HomeAssistantLight::incColor() {
  // if (localColorTemp != -1) {
  //   localColorTemp = -1;
  // }
  // bool temp = true;
  // increaseProperty(360, &temp, &localColor, 10, "hs_color", true);
}

void HomeAssistantLight::setAttribute(
    const std::map<std::string, std::string>& data) {
  // call_homeassistant_service("light.turn_on", data);
}

bool HomeAssistantLight::supportsBrightness() {
  return light_traits_.supports_color_capability(
      light::ColorCapability::BRIGHTNESS);
}
bool HomeAssistantLight::supportsColorTemperature() {
  return light_traits_.supports_color_capability(
      light::ColorCapability::COLOR_TEMPERATURE);
}
bool HomeAssistantLight::supportsColor() {
  return light_traits_.supports_color_capability(light::ColorCapability::RGB);
}

std::shared_ptr<MenuTitleSlider> HomeAssistantLight::makeSlider(
    int min, int max, int value, std::string title, std::string unit,
    int displayUnitMin, int displayUnitMax, int displayWidth) {
  int displayValue = value;
  float oldRange = max - min;
  float valueMinusMin = value - min;
  if (value > 0) {
    float displayNewRange = displayUnitMax - displayUnitMin;
    displayValue =
        static_cast<float>(((valueMinusMin * displayNewRange) / oldRange)) +
        displayUnitMin;
  }

  // float newMin = id(slider_margin_size);
  float newMin = 8;
  float newRange = displayWidth - 4 * newMin;
  int sliderValue = ((valueMinusMin * newRange) / oldRange) + newMin;
  return std::make_shared<MenuTitleSlider>(title.c_str(), entity_id_,
                                           NoMenuTitleRightIcon, sliderValue,
                                           displayValue, unit);
}

std::vector<std::shared_ptr<MenuTitleBase>> HomeAssistantLight::lightTitleItems(
    int displayWidth) {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  if (supportsBrightness()) {
    auto brightness =
        static_cast<int>(light_state_->remote_values.get_brightness() * 255);
    out.push_back(makeSlider(0, MAX_BRIGHTNESS, brightness, "Brightness", "%%",
                             0, 100, displayWidth));
  }
  if (supportsColorTemperature()) {
    auto max_mireds = light_traits_.get_max_mireds();
    auto min_mireds = light_traits_.get_min_mireds();
    out.push_back(makeSlider(
        min_mireds, max_mireds,
        light_state_->remote_values.get_color_temperature(), "Temperature", "K",
        1000000 / min_mireds, 1000000 / max_mireds, displayWidth));
  }
  if (supportsColor()) {
    out.push_back(
        makeSlider(0, 360, hsv_color(), "Color", "", 0, 360, displayWidth));
  }
  return out;
}

int HomeAssistantLight::hsv_color() {
  auto red = light_state_->remote_values.get_red();
  auto green = light_state_->remote_values.get_green();
  auto blue = light_state_->remote_values.get_blue();
  int hue = 0;
  float saturation = 0;
  float value = 0;
  rgb_to_hsv(red, green, blue, hue, saturation, value);
  return hue;
}

Color HomeAssistantLight::rgbLightColor() {
  if (!supportsBrightness()) {
    return Color(255, 255, 255);
  }
  auto red = light_state_->remote_values.get_red() * 255;
  auto green = light_state_->remote_values.get_green() * 255;
  auto blue = light_state_->remote_values.get_blue() * 255;
  return Color(red, green, blue);
}

std::string HomeAssistantLight::icon() {
  if (light_state_->remote_values.is_on()) {
    return "󰌵";
  } else {
    return "󰌶";
  }
}

void HomeAssistantLight::state_changed(std::string state) {
  ESP_LOGI(TAG, "'%s': state changed to %s", get_name().c_str(), state.c_str());
  auto onState = parse_on_off(state.c_str());
  auto call = this->light_state_->make_call();
  call.set_state(onState);
  call.perform();
}
void HomeAssistantLight::min_mireds_changed(std::string state) {
  ESP_LOGI(TAG, "'%s': min_mireds changed to %s", get_name().c_str(), state.c_str());
  auto min_mireds = atoi(state.c_str());
  light_traits_.set_min_mireds(min_mireds);
}
void HomeAssistantLight::max_mireds_changed(std::string state) {
  ESP_LOGI(TAG, "'%s': max_mireds changed to %s", get_name().c_str(), state.c_str());
  auto max_mireds = atoi(state.c_str());
  light_traits_.set_max_mireds(max_mireds);
}
void HomeAssistantLight::brightness_changed(std::string state) {
  auto brightness = parse_number<float>(state).value_or(0.0f);
  ESP_LOGI(TAG, "'%s': brightness changed to %f", get_name().c_str(),
           parse_number<float>(state).value_or(0.0f));
  update_supported_color_mode(light::ColorMode::BRIGHTNESS);
  auto call = this->light_state_->make_call();
  call.set_brightness(brightness / this->max_value_);
  call.perform();
  // if (id(keep_states_in_sync) || localBrightness == -1) {
  // localBrightness = atoi(newOnState.c_str());
  // isBrightnessInSync = true;
  // }
  // display->updateDisplay(false);
}
void HomeAssistantLight::color_temp_changed(std::string state) {
  auto color_temp = parse_number<float>(state).value_or(0.0f);
  ESP_LOGI(TAG, "'%s': color_temp changed to %f", get_name().c_str(),
           color_temp);
  update_supported_color_mode(light::ColorMode::COLOR_TEMPERATURE);

  auto call = this->light_state_->make_call();
  call.set_color_mode(light::ColorMode::COLOR_TEMPERATURE);
  call.set_color_temperature(color_temp);
  call.perform();
  // }
  // display->updateDisplay(false);
}
void HomeAssistantLight::color_changed(std::string state) {
  ESP_LOGI(TAG, "'%s': color changed to %s", get_name().c_str(), state.c_str());
  // if (id(keep_states_in_sync) || localColor == -1) {

  update_supported_color_mode(light::ColorMode::RGB);
  auto localColor = TextHelpers::extractFirstNumber(state);
  float red, green, blue;
  hsv_to_rgb(localColor, 1, 1, red, green, blue);
  auto call = this->light_state_->make_call();
  call.set_color_mode(light::ColorMode::RGB);
  call.set_rgb(red, green, blue);
  call.perform();
  // }
  // display->updateDisplay(false);
}

optional<light::ColorMode> HomeAssistantLight::parse_color_mode(
    std::string color_mode) {
  if (color_mode.find("onoff") != std::string::npos) {
    return light::ColorMode::ON_OFF;
  } else if (color_mode.find("brightness") != std::string::npos) {
    return light::ColorMode::BRIGHTNESS;
  } else if (color_mode.find("color_temp") != std::string::npos) {
    return light::ColorMode::COLOR_TEMPERATURE;
  } else if (color_mode.find("hs") != std::string::npos) {
    return light::ColorMode::COLOR_TEMPERATURE;
  } else if (color_mode.find("rgb") != std::string::npos) {
    return light::ColorMode::RGB;
  } else if (color_mode.find("rgbw") != std::string::npos) {
    return light::ColorMode::RGB_WHITE;
  } else if (color_mode.find("rgbww") != std::string::npos) {
    return light::ColorMode::RGB_COLD_WARM_WHITE;
  } else if (color_mode.find("white") != std::string::npos) {
    return light::ColorMode::WHITE;
  } else if (color_mode.find("xy") != std::string::npos) {
    return light::ColorMode::RGB_WHITE;
  }
  return {};
}

void HomeAssistantLight::update_supported_color_mode(light::ColorMode mode) {
  auto supported_modes = light_traits_.get_supported_color_modes();
  if (!supported_modes.count(mode)) {
    supported_modes.insert(mode);
    light_traits_.set_supported_color_modes(supported_modes);
  }
}

void HomeAssistantLight::color_mode_changed(std::string state) {
  ESP_LOGI(TAG, "'%s': color_mode changed changed to %s", get_name().c_str(),
           state.c_str());
  auto parsed_color_mode = parse_color_mode(state);
  if (parsed_color_mode.has_value()) {
    update_supported_color_mode(parsed_color_mode.value());
    auto call = this->light_state_->make_call();
    call.set_color_mode(parsed_color_mode.value());
    call.perform();
  }
}

void HomeAssistantLight::supported_color_modes_changed(std::string state) {
  // add all modes for the light to supportedColorModes and then check
  // the vector if the light supports a particular mode when needed
  ESP_LOGI(TAG, "'%s': supported_color_modes_changed changed to %s",
           get_name().c_str(), state.c_str());
  auto modes = StringUtils::split(state, ",");
  std::set<light::ColorMode> supportedModes;
  for (auto cmode : modes) {
    auto parsed_color_mode = parse_color_mode(cmode);
    if (parsed_color_mode.has_value()) {
      supportedModes.insert(parsed_color_mode.value());
    }
  }
  light_traits_.set_supported_color_modes(supportedModes);
  // display->updateDisplay(false);
}

bool HomeAssistantLight::get_state() {
  return light_state_->remote_values.is_on();
}

}  // namespace homeassistant_light
}  // namespace esphome
