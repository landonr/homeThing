#pragma once

#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif

#ifdef USE_SWITCH
#include "esphome/components/switch/switch.h"
#endif

namespace esphome {
namespace homething_menu_base {

enum MenuMode { MENU_MODE_ROTARY, MENU_MODE_3_BUTTON, MENU_MODE_2_BUTTON };

class HomeThingMenuSettings {
 public:
  MenuMode get_mode() { return mode_; }
  void set_mode(MenuMode mode) { mode_ = mode; }
  int get_display_timeout() { return display_timeout_; }
  void set_display_timeout(int display_timeout) {
    display_timeout_ = display_timeout;
  }
  int get_sleep_after() { return sleep_after_; }
  void set_sleep_after(int sleep_after) { sleep_after_ = sleep_after; }
  int get_lock_after() { return lock_after_; }
  void set_lock_after(int lock_after) { lock_after_ = lock_after; }

#ifdef USE_SWITCH
  switch_::Switch* get_sleep_switch() { return sleep_switch_; }
  void set_sleep_switch(switch_::Switch* sleep_switch) {
    sleep_switch_ = sleep_switch;
  }
#endif

  bool get_menu_rollover() { return menu_rollover_; }
  void set_menu_rollover(bool menu_rollover) { menu_rollover_ = menu_rollover; }

  bool get_menu_rollback() { return menu_rollback_; }
  void set_menu_rollback(bool menu_rollback) { menu_rollback_ = menu_rollback; }

 private:
  MenuMode mode_;
  int display_timeout_;
  int sleep_after_;
  bool menu_rollover_;
  bool menu_rollback_;

#ifdef USE_SWITCH
  switch_::Switch* sleep_switch_;
#endif

  int lock_after_;
};

}  // namespace homething_menu_base
}  // namespace esphome
