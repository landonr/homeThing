#pragma once

#ifdef USE_NOW_PLAYING
#include <string>
#include <vector>
#include "esphome/components/homeThing/homeThingMenuTitle.h"

namespace esphome {
namespace homething_menu_base {

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

#endif
