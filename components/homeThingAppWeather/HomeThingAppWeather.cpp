#include "HomeThingAppWeather.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_app_weather {
void HomeThingAppWeather::rootMenuTitles(
    std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {
  menu_titles->push_back(new homething_menu_base::MenuTitleBase(
      "Weather", "", homething_menu_base::ArrowMenuTitleRightIcon));
}

void HomeThingAppWeather::app_menu_titles(
    std::vector<homething_menu_base::MenuTitleBase*>* menu_titles) {}

// menu screens
homething_menu_app::NavigationCoordination HomeThingAppWeather::app_menu_select(
    int index) {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

bool HomeThingAppWeather::should_draw_app() {
  return true;
}

void HomeThingAppWeather::display_temperature(int xPos, int yPos,
                                              float temperature) {
  // Display temperature icon
  // display_buffer_->drawIcon(10, 10, temperature_icon_data, temperature_icon_width, temperature_icon_height, display_state_->get_color_palette()->get_accent_primary());
  // Draw temperature information
  auto state = "Temperature: " + to_string(static_cast<int>(temperature)) + "C";
  display_state_->drawTextWrapped(
      xPos, yPos, display_state_->get_font_medium(),
      display_state_->get_color_palette()->get_accent_primary(),
      display::TextAlign::TOP_CENTER, state.c_str(), 2, display_buffer_);
}

void HomeThingAppWeather::display_humidity(int xPos, int yPos, float humidity) {
  auto state = "Humidity: " + to_string(static_cast<int>(humidity)) + "%%";
  display_state_->drawTextWrapped(
      xPos, yPos, display_state_->get_font_medium(),
      display_state_->get_color_palette()->get_accent_primary(),
      display::TextAlign::TOP_CENTER, state.c_str(), 2, display_buffer_);
}

void HomeThingAppWeather::display_condition(int xPos, int yPos,
                                            const std::string& condition) {
  display_state_->drawTextWrapped(
      xPos, yPos, display_state_->get_font_medium(),
      display_state_->get_color_palette()->get_accent_primary(),
      display::TextAlign::CENTER_LEFT, condition, 2, display_buffer_);
}

void HomeThingAppWeather::display_condition_image(
    int xPos, int yPos, const std::string& condition) {
  int newXPos = xPos - cloudy_image_->get_width() / 2 - 10;
  if (condition == "clear-night" || condition == "night") {
    display_buffer_->image(newXPos, yPos, clear_image_,
                           display::ImageAlign::TOP_CENTER,
                           Color(255, 255, 255));
  } else if (condition == "cloudy") {
    display_buffer_->image(newXPos, yPos, cloudy_image_,
                           display::ImageAlign::TOP_CENTER,
                           Color(255, 255, 255));
  } else if (condition == "fog") {
    display_buffer_->image(newXPos, yPos, fog_image_,
                           display::ImageAlign::TOP_CENTER,
                           Color(255, 255, 255));
  } else if (condition == "hail") {
    display_buffer_->image(newXPos, yPos, hail_image_,
                           display::ImageAlign::TOP_CENTER,
                           Color(255, 255, 255));
  } else if (condition == "rainy" || condition == "lightning-rainy" ||
             condition == "pouring") {
    display_buffer_->image(newXPos, yPos, rainy_image_,
                           display::ImageAlign::TOP_CENTER,
                           Color(255, 255, 255));
  } else if (condition == "snowy" || condition == "snowy-rainy") {
    display_buffer_->image(newXPos, yPos, snow_image_,
                           display::ImageAlign::TOP_CENTER,
                           Color(255, 255, 255));
  } else if (condition == "sunny") {
    display_buffer_->image(newXPos, yPos, sunny_image_,
                           display::ImageAlign::TOP_CENTER,
                           Color(255, 255, 255));
  } else if (condition == "windy" || condition == "windy-variant") {
    display_buffer_->image(newXPos, yPos, windy_image_,
                           display::ImageAlign::TOP_CENTER,
                           Color(255, 255, 255));
  } else if (condition == "partly-cloudy") {
    display_buffer_->image(newXPos, yPos, partly_cloudy_image_,
                           display::ImageAlign::TOP_CENTER,
                           Color(255, 255, 255));
  } else if (condition == "lightning" || condition == "lightning-rainy") {
    display_buffer_->image(newXPos, yPos, lightning_image_,
                           display::ImageAlign::TOP_CENTER,
                           Color(255, 255, 255));
  }
}

void HomeThingAppWeather::draw_app(
    int menuIndex,
    const std::vector<homething_menu_base::MenuTitleBase*>* active_menu) {
  int centerX = display_buffer_->get_width() / 2;
  int imageXPos = centerX;
  int imageYPos = display_state_->get_header_height() + 2;
  int textXPos = centerX;
  int textYPos = imageYPos + cloudy_image_->get_height() + 8;
  int textSpacing = display_state_->get_font_medium()->get_height() + 2;

  // Display temperature, humidity, status, and weather condition with icons/images
  display_condition_image(imageXPos, imageYPos, condition_sensor_->state);
  display_condition(textXPos + 10,
                    imageYPos + (cloudy_image_->get_height() / 2),
                    condition_sensor_->state);
  display_temperature(textXPos, textYPos, temperature_sensor_->state);
  display_humidity(textXPos, textYPos + textSpacing, humidity_sensor_->state);
}

bool HomeThingAppWeather::idleTick(int idleTime, int display_timeout) {
  return false;
}

int HomeThingAppWeather::root_menu_size() {
  return 1;
}
void HomeThingAppWeather::reset_menu() {}
void HomeThingAppWeather::set_app_menu_index(int app_menu_index) {}

// buttons
homething_menu_app::NavigationCoordination
HomeThingAppWeather::rotaryScrollClockwise(int rotary) {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}
homething_menu_app::NavigationCoordination
HomeThingAppWeather::rotaryScrollCounterClockwise(int rotary) {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}
homething_menu_app::NavigationCoordination
HomeThingAppWeather::buttonPressUp() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationPop;
}

homething_menu_app::NavigationCoordination
HomeThingAppWeather::buttonPressDown() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingAppWeather::buttonPressLeft() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingAppWeather::buttonPressRight() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingAppWeather::buttonPressSelect(int menuIndex) {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

homething_menu_app::NavigationCoordination
HomeThingAppWeather::buttonPressOption() {
  return homething_menu_app::NavigationCoordination::NavigationCoordinationNone;
}

}  // namespace homething_app_weather
}  // namespace esphome
