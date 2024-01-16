#pragma once

#ifdef USE_MEDIA_PLAYER_GROUP

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#ifdef USE_IMAGE
#include "esphome/components/image/image.h"
#endif

#include "esphome/components/display/display_buffer.h"
#include "esphome/components/homeThing/homeThingMenuTitle.h"
#include "esphome/components/homeThing/homeThingOptionMenu.h"
#include "esphome/components/homeThingDisplayState/homeThingDisplayState.h"
#include "esphome/components/homeThingDisplayState/homeThingMenuTextHelpers.h"
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"
namespace esphome {
namespace homething_menu_now_playing {
class HomeThingMenuNowPlaying {
 public:
  HomeThingMenuNowPlaying(
      display::DisplayBuffer* new_display_buffer,
      homething_display_state::HomeThingDisplayState* new_display_state,
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          new_media_player_group,
      bool new_draw_now_playing_menu, int new_max_lines)
      : display_buffer_(new_display_buffer),
        display_state_(new_display_state),
        media_player_group_(new_media_player_group),
        draw_now_playing_menu_(new_draw_now_playing_menu),
        max_lines_(new_max_lines) {}
  PositionCoordinate get_coordinate(double radius, double angle);
  void drawNowPlaying(
      int menuIndex, HomeThingOptionMenu* option_menu,
      const std::vector<homething_menu_base::MenuTitleBase*>* active_menu);
#ifdef USE_IMAGE
  void set_now_playing_image(image::Image* now_playing_image) {
    now_playing_image_ = now_playing_image;
  }
  image::Image* get_now_playing_image() { return now_playing_image_; }
#endif
  void tickAnimation() { tick++; }
  void resetTick() { tick = -5; }

 private:
  display::DisplayBuffer* display_buffer_{nullptr};
  homething_display_state::HomeThingDisplayState* display_state_{nullptr};
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
      media_player_group_{nullptr};
  void drawCircleOptionMenu(
      std::vector<CircleOptionMenuItem> supported_features);
  void drawVolumeOptionMenu();
  std::string secondsToString(int seconds);
  void drawMediaDuration();
  bool drawOptionMenuAndStop(const HomeThingOptionMenu* option_menu);
  void drawNowPlayingSelectMenu(
      const std::vector<homething_menu_base::MenuTitleBase*>* menu_titles,
      int menu_index);
  display::TextAlign text_align_for_circle_position(
      CircleOptionMenuPosition position);
  void drawMediaText(int startYPos);
  void drawImage();
  void drawBottomText();
  void drawBottomBar(HomeThingOptionMenu* option_menu);
  int tick = 0;
  int getBottomBarYPosition();
  void tokenize(std::string const& str, std::string delim,
                std::vector<std::string>* out) {
    size_t start;
    size_t end = 0;

    while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
      end = str.find(delim, start);
      out->push_back(str.substr(start, end - start));
    }
  }
#ifdef USE_IMAGE
  image::Image* now_playing_image_{nullptr};
#endif
  bool draw_now_playing_menu_ = false;
  int max_lines_ = 5;

  const char* const TAG = "homething.menu.now_playing";
};
}  // namespace homething_menu_now_playing
}  // namespace esphome
#endif
