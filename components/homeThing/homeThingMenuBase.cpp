#include "homeThingMenuBase.h"
#include "esphome/core/log.h"
#include "version.h"

namespace esphome {
namespace homething_menu_base {

void HomeThingMenuBase::setup() {
  menu_display_->set_animation(animation_);
  menu_display_->add_on_state_callback([this]() {
    ESP_LOGI(TAG, "menu_display_->add_on_state_callback");
    this->update_display();
  });

  this->animation_->animationTick->add_on_state_callback(
      [this](float state) { this->displayUpdateDebounced(); });

  display_update_tick_ = new sensor::Sensor();
  auto filter = new sensor::DebounceFilter(17);
  display_update_tick_->add_filter(filter);
  this->display_update_tick_->add_on_state_callback(
      [this](float state) { this->displayUpdateDebounced(); });

#ifdef USE_HOMETHING_APP
  for (auto menu_app : menu_apps_) {
    if (menu_app->has_state_callback()) {
      menu_app->add_on_state_callback([this]() {
        switch (menuTree.back()) {
          case rootMenu:
          case appMenu:
            ESP_LOGI(TAG, "menu_app state_callback");
            reload_menu_items_ = true;
            this->update_display();
            break;
          default:
            break;
        }
      });
    }
  }
#endif

  for (auto screen : menu_screens_) {
    screen->add_on_state_callback([this]() {
      switch (menuTree.back()) {
        case settingsMenu:
          reload_menu_items_ = true;
          this->update_display();
        default:
          break;
      }
    });
  }
  if (home_screen_) {
    home_screen_->add_on_state_callback([this]() {
      switch (menuTree.back()) {
        case rootMenu:
          reload_menu_items_ = true;
          this->update_display();
        default:
          break;
      }
    });
  }
}

void HomeThingMenuBase::draw_menu_screen() {
  auto activeMenuState = menuTree.back();
  if (display_can_sleep()) {
    ESP_LOGI(TAG, "draw_menu_screen: display can sleep, not drawing");
    sleep_display();
    return;
  }
  if (menu_drawing_) {
    return;
  }
#ifdef USE_LIGHT
  if (backlight_ && !backlight_->remote_values.is_on()) {
    ESP_LOGD(TAG, "draw_menu_screen: display off, not drawing");
    return;
  }
#endif
  if (menuTree.front() == bootMenu && menu_display_->boot_complete()) {
    finish_boot();
    return;
  }
  menu_drawing_ = true;
  menu_display_->draw_background();
  auto title_name = MenuTitleBase::menu_state_title(activeMenuState);
  if (reload_menu_items_ ||
      (menu_titles.size() == 0 && activeMenuState != bootMenu)) {
    ESP_LOGI(TAG, "draw_menu_screen: reload %d index %d %s #%d",
             reload_menu_items_, menuIndex, title_name.c_str(),
             menu_titles.size());
    for (auto title : menu_titles) {
      delete title;
    }
    menu_titles.clear();
    activeMenu(&menu_titles);
    reload_menu_items_ = false;
  }
  ESP_LOGD(TAG, "draw_menu_screen: draw %d %s #%d", menuIndex,
           title_name.c_str(), menu_titles.size());

#ifdef USE_HOMETHING_APP
  if (active_app_ != nullptr) {
    ESP_LOGD(TAG, "draw_menu_screen: draw app header %d %s #%d", menuIndex,
             title_name.c_str(), menu_titles.size());
    menu_display_->draw_menu_header(active_app_->get_header_source());
  }
  if (menuTree.back() == appMenu && active_app_ != nullptr &&
      active_app_->should_draw_app()) {
    active_app_->draw_app(menuIndex, &menu_titles);
    this->animation_->animating = active_app_->is_animating();
    if (this->animation_->animating) {
      this->animation_->tickAnimation();
    }
    if (notifications_) {
      notifications_->drawNotifications();
    }
    menu_drawing_ = false;
    return;
  }
#endif

  if (menu_display_->draw_menu_screen(&activeMenuState, &menu_titles, menuIndex,
                                      nullptr, editing_menu_item)) {
    this->animation_->tickAnimation();
    this->animation_->animating = true;
  } else {
    this->animation_->animating = false;
  }
  if (device_locked_ && idleTime < 3) {
    menu_display_->draw_lock_screen(unlock_presses_);
  } else {
    ESP_LOGD(TAG,
             "draw_menu_screen: unlocked, not drawing - time: %d locked: %d",
             idleTime, device_locked_);
  }
  if (notifications_) {
    notifications_->drawNotifications();
  }
  menu_drawing_ = false;
}

void HomeThingMenuBase::topMenu() {
  ESP_LOGI(TAG, "topMenu: active menu state %d", menuTree.back());
  if (menuTree.back() == bootMenu) {
    return;
  }
  menuTree.assign(1, rootMenu);
  reset_menu();
  update_display();
}

void HomeThingMenuBase::update() {
  idleTick();
}

bool HomeThingMenuBase::selectMenu() {
  auto activeMenuTitle = menu_titles[menuIndex];
  switch (menuTree.back()) {
    case rootMenu:
      return selectRootMenu();
    case lightsDetailMenu:
      ESP_LOGI(TAG, "selectMenu: began editing light detail");
      editing_menu_item = true;
      break;
    case settingsMenu:
      if (active_menu_screen && active_menu_screen->select_menu(menuIndex)) {
        auto selected_entity = active_menu_screen->get_selected_entity();
        if (selected_entity) {
          MenuItemType menu_item_type = std::get<0>(*selected_entity);
          ESP_LOGI(TAG, "selectMenu: began editing type %d", menu_item_type);
          editing_menu_item = true;
        }
        update_display();
      }
      break;
    case appMenu:
#ifdef USE_HOMETHING_APP
      ESP_LOGI(TAG, "selectMenu: select app menu %d", menuIndex);
      if (active_app_) {
        switch (active_app_->app_menu_select(menuIndex)) {
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationNone:
            return false;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationReturn:
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationUpdate:
            return true;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationPop:
            menuTree.pop_back();
            menuIndex = 0;
            if (menuTree.size() == 1) {
              reset_menu();
              return true;
            }
            return true;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationRoot:
            topMenu();
            return true;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationReload:
            menuIndex = 0;
            reload_menu_items_ = true;
            return true;
        }
      }
#endif
      return false;
    default:
      ESP_LOGW(TAG, "selectMenu: menu state is bad but its an enum");
      return false;
  }
  return true;
}

bool HomeThingMenuBase::selectLightEntity(
    const std::tuple<MenuItemType, EntityBase*>* menu_item) {
  MenuItemType menu_item_type = std::get<0>(*menu_item);
  ESP_LOGW(TAG, "selectLightEntity: %d type: %d", menuIndex, menu_item_type);
  if (menu_item_type == MenuItemTypeLight) {
    ESP_LOGW(TAG, "selectLightEntit2y: %d type: %d", menuIndex, menu_item_type);
#ifdef USE_LIGHT
    auto light = static_cast<light::LightState*>(std::get<1>(*menu_item));
    ESP_LOGW(TAG, "selectLightEntity: name %s", light->get_name().c_str());
    if (supportsBrightness(light)) {
      active_menu_screen->set_selected_entity(menu_item);
      menuIndex = 0;
      menuTree.push_back(lightsDetailMenu);
      return true;
    }
#endif
  }
  ESP_LOGW(TAG, "selectLightEntity: %d returned", menuIndex);
  return false;
}

bool HomeThingMenuBase::selectDetailMenu() {
  ESP_LOGW(TAG, "selectDetailMenu: %d", menuIndex);
  switch (menuTree.back()) {
    case rootMenu: {
      if (home_screen_) {
        int offset = 0;
#ifdef USE_HOMETHING_APP
        for (auto& menu_app : menu_apps_) {
          int appMenuSize = menu_app->root_menu_size();
          if (menuIndex < (offset + appMenuSize)) {
            ESP_LOGI(TAG, "selectDetailMenu: app %d offset %d", menuIndex,
                     offset);
            menu_app->buttonPressOption();
            return false;
          }
          offset += appMenuSize;
        }
#endif
        int index = menuIndex - offset;
        ESP_LOGW(TAG, "selectDetailMenu: %d offset %d index %d", menuIndex,
                 offset, index);
        if (menuIndex >= offset && index < home_screen_->get_entity_count()) {
          auto menu_item = home_screen_->get_menu_item(index);
          ESP_LOGW(TAG, "selectDetailMenu: %d type: %d, name %s", index,
                   std::get<0>(*menu_item),
                   home_screen_->entity_name_at_index(index).c_str());
          return selectLightEntity(menu_item);
        }
      }
      break;
    }
    case settingsMenu: {
      if (active_menu_screen) {
        auto menu_item = active_menu_screen->get_menu_item(menuIndex);
        return selectLightEntity(menu_item);
      }
      break;
    }
    default:
      break;
  }
  return false;
}

bool HomeThingMenuBase::selectRootMenu() {
  int offset = 0;
#ifdef USE_HOMETHING_APP
  for (auto& menu_app : menu_apps_) {
    int appMenuSize = menu_app->root_menu_size();
    if (menuIndex < (offset + appMenuSize)) {
      ESP_LOGI(TAG, "selectRootMenu: app %d offset %d", menuIndex, offset);
      menuTree.push_back(appMenu);
      active_app_ = menu_app;
      active_app_->set_app_menu_index(menuIndex - offset);
      menuIndex = 0;
      return true;
    }
    offset += appMenuSize;
  }
#endif
  int index = menuIndex - offset;
  if (home_screen_ && index < home_screen_->get_entity_count()) {
    ESP_LOGI(TAG, "selectRootMenu: home screen %d offset %d", menuIndex,
             offset);
    return home_screen_->select_menu(index);
  } else {
    ESP_LOGI(TAG, "selectRootMenu: screen %d offset %d", menuIndex, offset);
    menuTree.push_back(settingsMenu);
    int home_screen_count = home_screen_ ? home_screen_->get_entity_count() : 0;
    offset = home_screen_count + offset;
    index = menuIndex - offset;
    ESP_LOGI(TAG,
             "selectRootMenu: 3 %d new offset %d menu screens %d index %d home "
             "screen count %d",
             menuIndex, offset, menu_screens_.size(), index, home_screen_count);
    active_menu_screen = menu_screens_[index];
  }
  menuIndex = 0;
  return true;
}

MenuTitleBase* HomeThingMenuBase::menuTitleForType(MenuStates stringType,
                                                   int index) {
  if (stringType == settingsMenu && menu_screens_.size() > 0) {
    int offset = home_screen_ ? home_screen_->get_entity_count() : 0;
    HomeThingMenuScreen* menu_screen = menu_screens_[index - offset];
    std::string menu_name = menu_screen->get_name();
    return new MenuTitleBase(menu_name, "", ArrowMenuTitleRightIcon);
  } else if (stringType == entityMenu && home_screen_) {
    std::string menu_name = home_screen_->entity_name_at_index(index);
    return new MenuTitleBase(menu_name, "", NoMenuTitleRightIcon);
  }
  return new MenuTitleBase(MenuTitleBase::menu_state_title(stringType), "",
                           MenuTitleBase::menu_state_right_icon(stringType));
}

void HomeThingMenuBase::finish_boot() {
  ESP_LOGI(TAG, "finish_boot: finished boot %d", menuTree.size());
  menuTree.assign(1, rootMenu);
  idleTime = 0;
  topMenu();
  menu_display_->clearBoot();
}

void HomeThingMenuBase::activeMenu(std::vector<MenuTitleBase*>* menu_titles) {
  switch (menuTree.back()) {
    case rootMenu: {
#ifdef USE_HOMETHING_APP
      ESP_LOGD(TAG, "activeMenu: root menu apps %d", menu_apps_.size());
      for (auto& menu_app : menu_apps_) {
        menu_app->rootMenuTitles(menu_titles);
      }
#endif
      if (home_screen_) {
        // for (int i = 0; i < home_screen_->get_entity_count(); i++) {
        //   out.push_back(entityMenu);
        // }
        home_screen_->menu_titles(menu_titles, false);
        active_menu_screen = home_screen_;
      }
      for (auto& menu_screen : menu_screens_) {
        std::string menu_name = menu_screen->get_name();
        menu_titles->push_back(
            new MenuTitleBase(menu_name, "", ArrowMenuTitleRightIcon));
      }
      return;
    }
    case lightsDetailMenu: {
#ifdef USE_LIGHT
      auto selectedEntity = active_menu_screen->get_selected_entity();
      if (selectedEntity != NULL &&
          std::get<0>(*selectedEntity) == MenuItemTypeLight) {
        auto light =
            static_cast<light::LightState*>(std::get<1>(*selectedEntity));
        lightTitleItems(light, menu_titles);
        return;
      } else {
        return;
      }
#endif
      break;
    }
    case bootMenu:
      break;
    case settingsMenu:
      active_menu_screen->menu_titles(menu_titles, true);
      break;
    case appMenu:
#ifdef USE_HOMETHING_APP
      if (active_app_) {
        active_app_->app_menu_titles(menu_titles);
      }
#endif
      return;
    default:
      ESP_LOGW(TAG, "activeMenu: menu is bad %d, %s", menuIndex,
               MenuTitleBase::menu_state_title(menuTree.back()).c_str());
      break;
  }
}

bool HomeThingMenuBase::skipBootPressed() {
  switch (menuTree.back()) {
    case bootMenu: {
      switch (menu_display_->bootSequenceCanSkip(menuTree.back())) {
        case BOOT_MENU_SKIP_STATE_SLEEP:
          ESP_LOGI(TAG, "skipBootPressed: sleep");
#ifdef USE_SWITCH
          sleep_switch_->turn_on();
#endif
          return true;
        case BOOT_MENU_SKIP_STATE_MENU:
          ESP_LOGI(TAG, "skipBootPressed: menu");
          // #ifdef USE_MEDIA_PLAYER_GROUP
          //           media_player_group_->selectFirstActivePlayer();
          // #endif
          finish_boot();
          return true;
        case BOOT_MENU_SKIP_STATE_NONE:
          ESP_LOGI(TAG, "skipBootPressed: none");
          break;
      }
      break;
    }
    default:
      break;
  }
  return false;
}

void HomeThingMenuBase::addNotification(const std::string& title,
                                        const std::string& subtitle,
                                        const std::string& text,
                                        bool autoClear) {
  if (notifications_) {
    notifications_->addNotification(title, subtitle, text, autoClear);
    ESP_LOGD(TAG, "addNotification: add notification %s", title.c_str());
  } else {
    ESP_LOGD(TAG, "addNotification: no notifications");
  }
  if (!buttonPressWakeUpDisplay()) {
    update_display();
  }
}

bool HomeThingMenuBase::clearNotifications() {
  if (notifications_) {
    return notifications_->clearNotifications();
  }
  return false;
}

bool HomeThingMenuBase::buttonPressWakeUpDisplay() {
  if (idleTime != -1) {
    idleTime = 0;
  }

#ifdef USE_LIGHT
  if (backlight_) {
    if (!backlight_->remote_values.is_on()) {
      ESP_LOGI(TAG, "buttonPressWakeUpDisplay: turning on display");
      turn_on_backlight();
      update_display();
      return true;
    } else if (backlight_->remote_values.get_brightness() < 1) {
      ESP_LOGI(TAG, "buttonPressWakeUpDisplay: turning on display 2");
      backlight_->turn_on()
          .set_transition_length(250)
          .set_brightness(1)
          .perform();
    }
  }
#endif
  return false;
}

void HomeThingMenuBase::buttonPressSelect() {
  if (!button_press_and_continue())
    return;
  reload_menu_items_ = true;
  if (menu_settings_->get_mode() == MENU_MODE_ROTARY) {
    switch (menuTree.back()) {
      case appMenu:
#ifdef USE_HOMETHING_APP
        if (active_app_ && active_app_->should_draw_app()) {
          switch (active_app_->buttonPressSelect(menuIndex)) {
            case homething_menu_app::NavigationCoordination::
                NavigationCoordinationReturn:
              return;
            case homething_menu_app::NavigationCoordination::
                NavigationCoordinationNone:
              break;
            case homething_menu_app::NavigationCoordination::
                NavigationCoordinationUpdate:
              update_display();
              return;
            case homething_menu_app::NavigationCoordination::
                NavigationCoordinationPop:
              upMenu();
              return;
            case homething_menu_app::NavigationCoordination::
                NavigationCoordinationRoot:
              topMenu();
              return;
            case homething_menu_app::NavigationCoordination::
                NavigationCoordinationReload:
              menuIndex = 0;
              for (auto title : menu_titles) {
                delete title;
              }
              menu_titles.clear();
              reload_menu_items_ = true;
              update_display();
              return;
          }
        }
#endif
        break;
      case lightsDetailMenu:
#ifdef USE_LIGHT
        if (!editing_menu_item && HomeThingMenuControls::selectLightDetail(
                                      active_menu_screen->get_selected_entity(),
                                      menuIndex, editing_menu_item)) {
          return;
        }
#endif
      case settingsMenu:
        if (editing_menu_item) {
          // deselect light if selected and stay in lightsDetailMenu
          editing_menu_item = false;
          reload_menu_items_ = true;
          update_display();
          return;
        }
        break;
      default:
        break;
    }
  } else {
    // 3 button
    switch (menuTree.back()) {
      case lightsDetailMenu:
#ifdef USE_LIGHT
        if (editing_menu_item) {
          // deselect light if selected and stay in lightsDetailMenu
          editing_menu_item = false;
          reload_menu_items_ = true;
          update_display();
          return;
        }
#endif
        break;
      default:
        break;
    }
  }
  if (selectMenu()) {
    update_display();
  }
}

void HomeThingMenuBase::buttonPressOption() {
  if (buttonPressWakeUpDisplay()) {
    return;
  }
  if (menuTree.back() == bootMenu) {
    return;
  }
  switch (menuTree.back()) {
    case appMenu:
#ifdef USE_HOMETHING_APP
      if (active_app_ && active_app_->should_draw_app()) {
        switch (active_app_->buttonPressOption()) {
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationReturn:
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationNone:
            break;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationUpdate:
            update_display();
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationPop:
            upMenu();
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationRoot:
            topMenu();
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationReload:
            menuIndex = 0;
            reload_menu_items_ = true;
            update_display();
            return;
        }
      }
#endif
      break;
    default:
      break;
  }
  if (selectDetailMenu()) {
    reload_menu_items_ = true;
    update_display();
  }
}

bool HomeThingMenuBase::upMenu() {
  if (menuTree.back() == lightsDetailMenu) {
    if (active_menu_screen)
      active_menu_screen->set_selected_entity(nullptr);
  }
  if (menuTree.size() > 1) {
    menuTree.pop_back();
    if (menuTree.back() == rootMenu) {
      reset_menu();
      update_display();
      return true;
    }
    menuIndex = 0;
    reload_menu_items_ = true;
    menu_titles.clear();
    update_display();
    return true;
  }
  return false;
}

void HomeThingMenuBase::rotaryScrollCounterClockwise() {
  if (!button_press_and_continue())
    return;
  rotary_ -= 1;
  if (menuIndex == 0 && menu_settings_->get_menu_rollback()) {
    // && menuTree.back() != nowPlayingMenu) {
    switch (menuTree.back()) {
      case appMenu: {
#ifdef USE_HOMETHING_APP
        if (active_app_) {
          if (active_app_->should_draw_app()) {
            break;
          }
        }
#endif
        upMenu();
        return;
      }
      default:
        upMenu();
        return;
    }
  }
  if (menu_settings_->get_mode() == MENU_MODE_ROTARY) {
    switch (menuTree.back()) {
      case appMenu:
#ifdef USE_HOMETHING_APP
        if (active_app_ && active_app_->should_draw_app()) {
          switch (active_app_->rotaryScrollCounterClockwise(rotary_)) {
            case homething_menu_app::NavigationCoordination::
                NavigationCoordinationReturn:
              return;
            case homething_menu_app::NavigationCoordination::
                NavigationCoordinationNone:
              break;
            case homething_menu_app::NavigationCoordination::
                NavigationCoordinationUpdate:
              update_display();
              return;
            case homething_menu_app::NavigationCoordination::
                NavigationCoordinationPop:
              upMenu();
              return;
            case homething_menu_app::NavigationCoordination::
                NavigationCoordinationRoot:
              topMenu();
              return;
            case homething_menu_app::NavigationCoordination::
                NavigationCoordinationReload:
              menuIndex = 0;
              reload_menu_items_ = true;
              update_display();
              return;
          }
        }
#endif
        break;
      case lightsDetailMenu:
#ifdef USE_LIGHT
        if (HomeThingMenuControls::editingScrollBack(
                active_menu_screen->get_selected_entity(), menuIndex,
                editing_menu_item)) {
          reload_menu_items_ = true;
          debounceUpdateDisplay();
          return;
        }
#endif
        break;
      case settingsMenu:
        if (editing_menu_item && active_menu_screen &&
            active_menu_screen->get_selected_entity() &&
            HomeThingMenuControls::editingScrollBack(
                active_menu_screen->get_selected_entity(), menuIndex,
                editing_menu_item)) {
          reload_menu_items_ = true;
          debounceUpdateDisplay();
          return;
        }
        break;
      default:
        break;
    }
    if (menuIndex > 0) {
      menuIndex--;
    } else if (menu_settings_->get_menu_rollover() && menuIndex == 0) {
      menuIndex = menu_titles.size() - 1;
    }
  } else {
    // 3 button
    switch (menuTree.back()) {
#ifdef USE_LIGHT
      case lightsDetailMenu:
        if (HomeThingMenuControls::editingScrollBack(
                active_menu_screen->get_selected_entity(), menuIndex,
                editing_menu_item)) {
          debounceUpdateDisplay();
          return;
        }
#endif
      default:
        break;
    }

    if (menuIndex > 0) {
      menuIndex--;
      // } else if (menuTree.back() == nowPlayingMenu) {
      //   menuIndex = menu_titles.size() - 1;
    }
  }
  debounceUpdateDisplay();
}

void HomeThingMenuBase::rotaryScrollClockwise() {
  if (menu_settings_->get_mode() == MENU_MODE_3_BUTTON && skipBootPressed())
    return;
  if (!button_press_and_continue())
    return;
  rotary_ += 1;
  if (menu_settings_->get_mode() == MENU_MODE_ROTARY) {
    switch (menuTree.back()) {
      case appMenu:
#ifdef USE_HOMETHING_APP
        if (active_app_ && active_app_->should_draw_app()) {
          switch (active_app_->rotaryScrollClockwise(rotary_)) {
            case homething_menu_app::NavigationCoordination::
                NavigationCoordinationReturn:
              return;
            case homething_menu_app::NavigationCoordination::
                NavigationCoordinationNone:
              break;
            case homething_menu_app::NavigationCoordination::
                NavigationCoordinationUpdate:
              update_display();
              return;
            case homething_menu_app::NavigationCoordination::
                NavigationCoordinationPop:
              upMenu();
              return;
            case homething_menu_app::NavigationCoordination::
                NavigationCoordinationRoot:
              topMenu();
              return;
            case homething_menu_app::NavigationCoordination::
                NavigationCoordinationReload:
              menuIndex = 0;
              reload_menu_items_ = true;
              update_display();
              return;
          }
        }
#endif
        break;
      case lightsDetailMenu:
#ifdef USE_LIGHT
        if (HomeThingMenuControls::editingScrollForward(
                active_menu_screen->get_selected_entity(), menuIndex,
                editing_menu_item)) {
          reload_menu_items_ = true;
          debounceUpdateDisplay();
          return;
        }
#endif
        break;
      case settingsMenu:
        if (editing_menu_item && active_menu_screen &&
            active_menu_screen->get_selected_entity() &&
            HomeThingMenuControls::editingScrollForward(
                active_menu_screen->get_selected_entity(), menuIndex,
                editing_menu_item)) {
          reload_menu_items_ = true;
          debounceUpdateDisplay();
          return;
        }
      default:
        break;
    }
    if (menuIndex < menu_titles.size() - 1) {
      menuIndex++;
    } else if (menu_settings_->get_menu_rollover() &&
               menuIndex == menu_titles.size() - 1) {
      menuIndex = 0;
    }
  } else {
    // 3 button
    switch (menuTree.back()) {
#ifdef USE_LIGHT
      case lightsDetailMenu:
        if (HomeThingMenuControls::editingScrollForward(
                active_menu_screen->get_selected_entity(), menuIndex,
                editing_menu_item)) {
          debounceUpdateDisplay();
          return;
        }
#endif
      default:
        break;
    }

    if (menuIndex < menu_titles.size() - 1) {
      menuIndex++;
    } else if (menuIndex == menu_titles.size() - 1) {
      menuIndex = 0;
    } else {
      menuIndex = 0;
    }
  }
  debounceUpdateDisplay();
}

void HomeThingMenuBase::buttonPressUp() {
  if (!button_press_and_continue())
    return;
  switch (menuTree.back()) {
    case appMenu:
#ifdef USE_HOMETHING_APP
      if (active_app_) {
        switch (active_app_->buttonPressUp()) {
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationReturn:
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationNone:
            break;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationUpdate:
            update_display();
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationPop:
            upMenu();
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationRoot:
            topMenu();
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationReload:
            menuIndex = 0;
            reload_menu_items_ = true;
            update_display();
            return;
        }
      }
#endif
      break;
    case lightsDetailMenu:
#ifdef USE_LIGHT
      if (editing_menu_item) {
        // deselect light if selected and stay in lightsDetailMenu
        editing_menu_item = false;
        reload_menu_items_ = true;
        update_display();
        return;
      }
      // if no light is selected go up menu
      if (upMenu())
        return;
#endif
      break;
    default:
    case settingsMenu:
      if (editing_menu_item) {
        editing_menu_item = false;
        reload_menu_items_ = true;
        update_display();
        return;
      }
      break;
  }
  if (active_menu_screen)
    active_menu_screen->set_selected_entity(nullptr);
  topMenu();
  update_display();
}

void HomeThingMenuBase::buttonPressDown() {
  if (!button_press_and_continue())
    return;
  switch (menuTree.back()) {
    case appMenu:
#ifdef USE_HOMETHING_APP
      if (active_app_ && active_app_->should_draw_app()) {
        switch (active_app_->buttonPressDown()) {
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationReturn:
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationNone:
            break;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationUpdate:
            update_display();
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationPop:
            upMenu();
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationRoot:
            topMenu();
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationReload:
            menuIndex = 0;
            reload_menu_items_ = true;
            update_display();
            return;
        }
      }
#endif
      break;
    default:
      break;
  }
}

void HomeThingMenuBase::buttonPressLeft() {
  if (!button_press_and_continue())
    return;
  switch (menuTree.back()) {
    case appMenu:
#ifdef USE_HOMETHING_APP
      if (active_app_ && active_app_->should_draw_app()) {
        switch (active_app_->buttonPressLeft()) {
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationReturn:
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationNone:
            break;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationUpdate:
            update_display();
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationPop:
            upMenu();
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationRoot:
            topMenu();
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationReload:
            menuIndex = 0;
            reload_menu_items_ = true;
            update_display();
            return;
        }
      }
#endif
      break;
    default:
      break;
  }
}

void HomeThingMenuBase::buttonPressRight() {
  if (skipBootPressed())
    return;
  if (!button_press_and_continue())
    return;
  switch (menuTree.back()) {
    case appMenu:
#ifdef USE_HOMETHING_APP
      if (active_app_ && active_app_->should_draw_app()) {
        switch (active_app_->buttonPressRight()) {
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationReturn:
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationNone:
            break;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationUpdate:
            update_display();
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationPop:
            upMenu();
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationRoot:
            topMenu();
            return;
          case homething_menu_app::NavigationCoordination::
              NavigationCoordinationReload:
            menuIndex = 0;
            reload_menu_items_ = true;
            update_display();
            return;
        }
      }
#endif
      break;
    default:
      break;
  }
}

bool HomeThingMenuBase::buttonPressUnlock() {
  if (device_locked_) {
    unlock_presses_++;
    if (unlock_presses_ >= 2) {
      device_locked_ = false;
      unlock_presses_ = 0;
    }
    idleTime = 0;
    update_display();
    return true;
  }
  return false;
}

void HomeThingMenuBase::displayUpdateDebounced() {
  bool app_animating = false;
#ifdef USE_HOMETHING_APP
  if (active_app_ != nullptr && active_app_->is_animating()) {
    app_animating = true;
  }
#endif
  if (idleTime < 2 || animation_->animating || get_charging() ||
      app_animating) {
    update_display();
  }
}

void HomeThingMenuBase::debounceUpdateDisplay() {
  if (menu_settings_->get_mode() == MENU_MODE_ROTARY) {
    if (display_update_tick_->state != rotary_) {
      display_update_tick_->publish_state(rotary_);
    }
  } else {
    update_display();
  }
}

bool HomeThingMenuBase::display_can_fade_out() {
  int display_timeout_while_charging =
      menu_settings_->get_display_timeout_while_charging();
  if (get_charging()) {
    if (display_timeout_while_charging > 0)
      return idleTime == display_timeout_while_charging - 4;
    else
      return false;
  }
  return idleTime == menu_settings_->get_display_timeout() - 4;
}

bool HomeThingMenuBase::display_can_sleep() {
#ifdef USE_LIGHT
  if (backlight_ && !backlight_->remote_values.is_on()) {
    return false;
  } else if (!backlight_) {
    return false;
  }
#else
  return false;
#endif
  int timeout = menu_settings_->get_display_timeout();
  bool idle_timeout = timeout != 0 && idleTime >= timeout;

  int display_timeout_while_charging =
      menu_settings_->get_display_timeout_while_charging();

  ESP_LOGD(TAG,
           "screen timeout %d, charging %d, display_timeout_while_charging %d "
           "idle %d",
           idle_timeout, get_charging(), display_timeout_while_charging,
           idleTime);

  if (get_charging()) {
    if (display_timeout_while_charging > 0)
      return idleTime >= display_timeout_while_charging;
    else
      return false;
  } else {
    return idle_timeout;
  }
}

void HomeThingMenuBase::turn_on_backlight() {
#ifdef USE_LIGHT
  if (backlight_ && !backlight_->remote_values.is_on()) {
    ESP_LOGI(TAG, "turn_on_backlight: turning on display");
    backlight_->turn_on()
        .set_transition_length(250)
        .set_brightness(1)
        .perform();
  }
#endif
}

void HomeThingMenuBase::sleep_display() {
#ifdef USE_LIGHT
  if (backlight_ != nullptr &&
      (backlight_->remote_values.is_on() ||
       backlight_->remote_values.get_brightness() > 0)) {
    ESP_LOGI(TAG, "sleep_display: turning off display %d, %d, %f",
             backlight_ == nullptr, backlight_->remote_values.is_on(),
             backlight_->remote_values.get_brightness());
    backlight_->turn_off()
        .set_brightness(0)
        .set_transition_length(500)
        .perform();
  } else {
    ESP_LOGI(TAG, "sleep_display: NOT turning off display %d, %d, %f",
             backlight_ == nullptr, backlight_->remote_values.is_on(),
             backlight_->remote_values.get_brightness());
  }
#endif
}

void HomeThingMenuBase::fade_out_display() {
#ifdef USE_LIGHT
  auto brightness = backlight_->remote_values.get_brightness();
  if (backlight_ != nullptr && backlight_->remote_values.is_on() &&
      brightness > 0.3f) {
    ESP_LOGI(TAG, "fade_out_display: fading out display %d, %d, %f",
             backlight_ == nullptr, backlight_->remote_values.is_on(),
             brightness);
    backlight_->turn_on()
        .set_brightness(0.3)
        .set_transition_length(500)
        .perform();
  }
#endif
}

void HomeThingMenuBase::lockDevice() {
  if (!device_locked_) {
    ESP_LOGI(TAG, "lockDevice: locking device");
    device_locked_ = true;
    unlock_presses_ = 0;
  }
}

void HomeThingMenuBase::idleTick() {
  ESP_LOGD(TAG, "idleTick: idle %d", idleTime);
  if (menuTree.back() == bootMenu) {
    if (display_can_sleep()) {
      sleep_display();
    }
    idleTime++;
    return;
  }
#ifdef USE_HOMETHING_APP
  for (auto app : menu_apps_) {
    if (app->idleTick(idleTime, menu_settings_->get_display_timeout())) {
      update_display();
    }
  }
#endif
  if (menu_settings_->get_lock_after() != 0 &&
      idleTime >= menu_settings_->get_lock_after()) {
    lockDevice();
  }
  if (idleTime == 2) {
    unlock_presses_ = 0;
  } else if (idleTime == 3) {
    update_display();
  } else if (display_can_fade_out()) {
    fade_out_display();
  } else if (display_can_sleep()) {
    ESP_LOGD(TAG, "idleTick: turning off display");
    sleep_display();
    idleMenu();
    idleTime++;
    return;
  } else if (idleTime > menu_settings_->get_sleep_after()) {
    ESP_LOGD(TAG, "idleTick: night night? %d", get_charging());
#ifdef USE_SWITCH
    if (!get_charging() && sleep_switch_) {
      ESP_LOGD(TAG, "idleTick: night night");
      sleep_switch_->turn_on();
      return;
    }
#endif
  }
  idleTime++;
}

void HomeThingMenuBase::goToScreenFromString(std::string screenName) {
  ESP_LOGI(TAG, "goToScreenFromString: %s", screenName.c_str());
  if (screenName == "lightDetail") {
    menuTree.push_back(lightsDetailMenu);
  } else if (screenName == "home") {
    menuTree.assign(1, rootMenu);
  } else if (screenName == "boot") {
    menuTree.push_back(bootMenu);
  }
  reset_menu();
  update_display();
}

void HomeThingMenuBase::idleMenu() {
  ESP_LOGI(TAG, "idleMenu");
  if (menuTree.back() == bootMenu) {
    ESP_LOGI(TAG, "idleMenu boot menu");
    return;
  }
  reset_menu();
#ifdef USE_HOMETHING_APP
  auto idle_app = menu_settings_->get_idle_app();
  if (idle_app) {
    ESP_LOGI(TAG, "idleMenu: set idle app");
    menuTree.push_back(appMenu);
    active_app_ = idle_app;
    active_app_->set_app_menu_index(0);
  }
#endif
}
}  // namespace homething_menu_base
}  // namespace esphome
