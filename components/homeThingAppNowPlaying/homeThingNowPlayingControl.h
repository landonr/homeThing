#pragma once
#include <string>
#include <utility>
#include <vector>
#include "esphome/components/homeThing/homeThingMenuScreen.h"
#include "esphome/components/homeThing/homeThingMenuTitleSource.h"
#include "esphome/components/homeThing/homeThingOptionMenu.h"
#include "esphome/components/homeThingDisplayState/homeThingDisplayState.h"
#include "esphome/components/homeThingDisplayState/homeThingMenuTextHelpers.h"

#include "esphome/components/homeThingApp/homeThingApp.h"

#include "esphome/components/homeThingAppNowPlaying/HomeThingNowPlayingHeader.h"
#include "esphome/components/homeThingAppNowPlaying/NowPlayingMenuState.h"
#include "esphome/components/homeThingAppNowPlaying/homeThingNowPlayingDisplay.h"
#include "esphome/components/homeThingAppNowPlaying/homeThingNowPlayingOptionMenu.h"

#include "esphome/components/homeThingAppNowPlaying/homeThingNowPlayingMenuGroup.h"
#include "esphome/components/homeThingAppNowPlaying/homeThingNowPlayingMenuMediaPlayers.h"
#include "esphome/components/homeThingAppNowPlaying/homeThingNowPlayingMenuSources.h"

#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"

namespace esphome {
namespace homething_menu_now_playing {

class HomeThingMenuNowPlayingControl : public homething_menu_app::HomeThingApp {
 public:
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
  get_media_player_group();
  void set_media_player_group(
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          media_player_group);

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
  void active_tick();
  int root_menu_size();
  //   void selectNowPlayingMenu();
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

  // display
  void set_now_playing_display(
      display::DisplayBuffer* new_display_buffer,
      homething_display_state::HomeThingDisplayState* new_display_state,
      homeassistant_media_player::HomeAssistantMediaPlayerGroup*
          new_media_player_group) {
    now_playing_display_ = new HomeThingMenuNowPlaying(
        new_display_buffer, new_display_state, new_media_player_group,
        get_draw_now_playing_bottom_menu(), get_now_playing_max_lines());
    circle_menu_->set_bottom_menu(get_draw_now_playing_bottom_menu());
  }

#ifdef USE_IMAGE
  void set_now_playing_image(image::Image* now_playing_image) {
    now_playing_display_->set_now_playing_image(now_playing_image);
  }
#endif

  homething_menu_base::HomeThingMenuHeaderSource* get_header_source() {
    return header_source_;
  }
  HomeThingMenuHeaderSource* header_source_{nullptr};

  bool is_animating() { return animating_; }
  homething_menu_app::NavigationCoordination selectNowPlayingBottomMenu(
      int index);

  int get_now_playing_max_lines() { return now_playing_max_lines_; }
  void set_now_playing_max_lines(int now_playing_max_lines) {
    now_playing_max_lines_ = now_playing_max_lines;
  }

  bool get_draw_now_playing_bottom_menu() { return draw_now_playing_menu_; }
  void set_draw_now_playing_bottom_menu(bool draw_now_playing_menu) {
    draw_now_playing_menu_ = draw_now_playing_menu;
  }

  // state callback
  bool has_state_callback() { return true; }
  void add_on_state_callback(std::function<void()>&& callback) {
    this->callback_.add(std::move(callback));
  }

 protected:
  homeassistant_media_player::HomeAssistantMediaPlayerGroup*
      media_player_group_{nullptr};
  HomeThingMenuNowPlayingOptionMenu* circle_menu_ =
      new HomeThingMenuNowPlayingOptionMenu();
  HomeThingMenuNowPlaying* now_playing_display_{nullptr};
  NowPlayingMenuState menu_state_ = NOW_PLAYING_MENU_STATE_NOW_PLAYING;

 private:
  const char* const TAG = "homething.nowplaying.control";
  CallbackManager<void()> callback_;

  // controls
  void select_media_player_feature(
      homeassistant_media_player::MediaPlayerFeatureCommand* command);
  homething_menu_app::NavigationCoordination button_press_now_playing_option(
      CircleOptionMenuPosition position);
  bool animating_ = false;
  int now_playing_max_lines_;
  bool draw_now_playing_menu_;
};
}  // namespace homething_menu_now_playing
}  // namespace esphome
