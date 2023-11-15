#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "esphome/core/color.h"

#ifdef USE_MEDIA_PLAYER_GROUP
#include "esphome/components/homeassistant_media_player/HomeAssistantMediaPlayerGroup.h"
#endif

#include "esphome/core/log.h"
#include "version.h"

namespace esphome {
namespace homething_menu_base {

static const char* const MENU_TITLE_TAG = "homething.menutitle";

enum MenuStates {
  bootMenu,
  rootMenu,
  appMenu,
  // sourcesMenu,
  // groupMenu,
  // mediaPlayersMenu,
  lightsDetailMenu,
  // nowPlayingMenu,
  settingsMenu,
  entityMenu
};

enum MenuTitleLeftIcon {
  NoMenuTitleLeftIcon,
  OffMenuTitleLeftIcon,
  OnMenuTitleLeftIcon,
  GroupedMenuTitleLeftIcon
};

enum MenuTitleRightIcon { NoMenuTitleRightIcon, ArrowMenuTitleRightIcon };

enum MenuTitleType {
  BaseMenuTitleType,
  PlayerMenuTitleType,
  SliderMenuTitleType,
  ToggleMenuTitleType,
  LightMenuTitleType,
  SourceMenuTitleType
};

enum SliderSelectionState {
  SliderSelectionStateNone,
  SliderSelectionStateActive,
  SliderSelectionStateHover
};

class MenuTitleBase {
 protected:
  std::string name_;

 public:
  std::string entity_id_;
  MenuTitleRightIcon rightIconState;
  MenuTitleType titleType;
  MenuTitleBase(std::string new_name, std::string newEntityId,
                MenuTitleRightIcon newRightIconState,
                MenuTitleType newTitleType = BaseMenuTitleType)
      : name_(new_name),
        entity_id_(newEntityId),
        rightIconState(newRightIconState),
        titleType(newTitleType) {}

  std::string get_name() const {
    if (name_ != "") {
      return name_;
    } else {
      return entity_id_;
    }
  }

  static MenuTitleRightIcon menu_state_right_icon(MenuStates menu_state) {
    switch (menu_state) {
      case appMenu:
        return ArrowMenuTitleRightIcon;
      case lightsDetailMenu:
        return ArrowMenuTitleRightIcon;
      case rootMenu:
        return NoMenuTitleRightIcon;
      case bootMenu:
        return NoMenuTitleRightIcon;
      case settingsMenu:
        return ArrowMenuTitleRightIcon;
      case entityMenu:
        return NoMenuTitleRightIcon;
    }
    return NoMenuTitleRightIcon;
  }

  static std::string menu_state_title(MenuStates menu_state) {
    switch (menu_state) {
      case appMenu:
        return "App";
      case lightsDetailMenu:
        return "Light Detail";
      case rootMenu:
        return "Home";
      case bootMenu:
        return "Boot";
      case settingsMenu:
        return "Settings";
      case entityMenu:
        return "Entity";
    }
    return "";
  }
};

class MenuTitleSource : public MenuTitleBase {
 public:
  media_player_source::MediaPlayerSourceItem* media_source_;

  MenuTitleSource(std::string new_name, std::string newEntityId,
                  MenuTitleRightIcon newRightIconState,
                  media_player_source::MediaPlayerSourceItem* new_media_source)
      : MenuTitleBase{new_name, newEntityId, newRightIconState,
                      SourceMenuTitleType},
        media_source_(new_media_source) {}

  std::string sourceTypeString() { return media_source_->sourceTypeString(); }

  static void activePlayerSourceTitles(
      std::vector<media_player_source::MediaPlayerSourceBase*>* sources,
      std::vector<MenuTitleBase*>* menu_titles) {
    for (auto& source : *sources) {
      auto new_menu_title =
          new MenuTitleBase(source->get_name(), "", NoMenuTitleRightIcon);
      (*menu_titles).push_back(new_menu_title);
    }
  }

  static void activePlayerSourceItemTitles(
      const std::vector<media_player_source::MediaPlayerSourceItem*>*
          sourceItems,
      std::vector<MenuTitleBase*>* menu_titles) {
    if (sourceItems->size() == 0) {
      ESP_LOGW(MENU_TITLE_TAG, "activePlayerSourceItemTitles: empty list");
      return;
    }
    for (auto& sourceItem : (*sourceItems)) {
      auto name = sourceItem->get_name();
      ESP_LOGD(MENU_TITLE_TAG, "activePlayerSourceItemTitles: name %s",
               name.c_str());
      auto new_menu_title =
          new MenuTitleSource(name, sourceItem->get_media_content_id(),
                              NoMenuTitleRightIcon, sourceItem);
      (*menu_titles).push_back(new_menu_title);
    }
  }
};

}  // namespace homething_menu_base
}  // namespace esphome
