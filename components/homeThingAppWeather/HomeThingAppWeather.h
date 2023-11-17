#pragma once

#include <string>
#include <vector>
#include "esphome/components/homeThing/homeThingMenuHeader.h"
#include "esphome/components/homeThingApp/homeThingApp.h"
namespace esphome {
namespace homething_app_weather {

class HomeThingAppWeatherHeader
    : public homething_menu_base::HomeThingMenuHeaderSource {
 public:
  std::string get_header_title() { return "Weather"; }

  int draw_header_details(
      int xPos, int yPos, display::DisplayBuffer* display_buffer,
      homething_display_state::HomeThingDisplayState* display_state,
      homething_display_state::HomeThingMenuTextHelpers* text_helpers) {
    return 0;
  }

 private:
  const char* const TAG = "homething.app.weather.header";
};

class HomeThingAppWeather : public homething_menu_app::HomeThingApp {
 public:
  // menu titles
  void rootMenuTitles(
      std::vector<homething_menu_base::MenuTitleBase*>* menu_titles);
  void app_menu_titles(
      std::vector<homething_menu_base::MenuTitleBase*>* menu_titles);

  // menu screens
  homething_menu_app::NavigationCoordination app_menu_select(int index);
  bool should_draw_app();
  void draw_app(
      int menuIndex,
      const std::vector<homething_menu_base::MenuTitleBase*>* active_menu);

  bool idleTick(int idleTime, int display_timeout);
  int root_menu_size();
  void reset_menu();
  void set_app_menu_index(int app_menu_index);
  // buttons
  homething_menu_app::NavigationCoordination rotaryScrollClockwise(int rotary);
  homething_menu_app::NavigationCoordination rotaryScrollCounterClockwise(
      int rotary);
  homething_menu_app::NavigationCoordination buttonPressUp();
  homething_menu_app::NavigationCoordination buttonPressDown();
  homething_menu_app::NavigationCoordination buttonPressLeft();
  homething_menu_app::NavigationCoordination buttonPressRight();
  homething_menu_app::NavigationCoordination buttonPressSelect(int menuIndex);
  homething_menu_app::NavigationCoordination buttonPressOption();

  homething_menu_base::HomeThingMenuHeaderSource* get_header_source() {
    return header_source_;
  }
  HomeThingMenuHeaderSource* header_source_ = new HomeThingAppWeatherHeader();

  void set_display_buffer(display::DisplayBuffer* display_buffer) {
    display_buffer_ = display_buffer;
  }

  void set_display_state(
      homething_display_state::HomeThingDisplayState* display_state) {
    display_state_ = display_state;
  }

  bool is_animating() { return false; }

  // state callback
  bool has_state_callback() { return false; }
  void add_on_state_callback(std::function<void()>&& callback) {}

  // sensors
  void set_temperature_sensor(sensor::Sensor* temperature_sensor) {
    temperature_sensor_ = temperature_sensor;
  }

  void set_humidity_sensor(sensor::Sensor* humidity_sensor) {
    humidity_sensor_ = humidity_sensor;
  }

  void set_condition_sensor(text_sensor::TextSensor* condition_sensor) {
    condition_sensor_ = condition_sensor;
  }

  void set_cloudy_image(image::Image* cloudy_image) {
    cloudy_image_ = cloudy_image;
  }

  void set_fog_image(image::Image* fog_image) { fog_image_ = fog_image; }

  void set_snow_image(image::Image* snow_image) { snow_image_ = snow_image; }

  void set_sunny_image(image::Image* sunny_image) {
    sunny_image_ = sunny_image;
  }

  void set_rainy_image(image::Image* rainy_image) {
    rainy_image_ = rainy_image;
  }

  void set_night_image(image::Image* night_image) {
    night_image_ = night_image;
  }

 private:
  const char* const TAG = "homething.app.weather";

  sensor::Sensor* temperature_sensor_{nullptr};
  sensor::Sensor* humidity_sensor_{nullptr};
  text_sensor::TextSensor* condition_sensor_{nullptr};

  // display
  display::DisplayBuffer* display_buffer_{nullptr};
  homething_display_state::HomeThingDisplayState* display_state_{nullptr};
  float temperature_icon_width = 48;

  // images

  image::Image* cloudy_image_{nullptr};
  image::Image* fog_image_{nullptr};
  image::Image* snow_image_{nullptr};
  image::Image* sunny_image_{nullptr};
  image::Image* rainy_image_{nullptr};
  image::Image* night_image_{nullptr};

  void display_temperature(int xPos, int yPos, float temperature);
  void display_humidity(int xPos, int yPos, float humidity);
  void display_condition(int xPos, int yPos, const std::string& condition);
  void display_condition_image(int xPos, int yPos,
                               const std::string& condition);
};
}  // namespace homething_app_weather
}  // namespace esphome
