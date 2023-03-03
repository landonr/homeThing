#include "homeThingMenuDisplay.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_menu_base {

void HomeThingMenuDisplay::setup() {}

void HomeThingMenuDisplay::drawTitle(int menuState, int i, std::string title,
                                     int yPos, bool buttonSpace) {
  int xPos = buttonSpace ? display_state_->get_small_font()->get_height() +
                               display_state_->margin_size_ * 2
                         : display_state_->margin_size_;
  int textYPos = yPos + (display_state_->margin_size_ / 4);
  const int animationTick = animation_->animationTick;
  if (menuState == i) {
    int characterLimit = text_helpers_->getCharacterLimit(
        xPos, display_state_->get_medium_font()->get_height(),
        display::TextAlign::TOP_LEFT);
    if (animationTick > title.length() - characterLimit + 4) {
      // animationTick = -6;
      animation_->tickAnimation();
    }
    int marqueePositionMaxed = 0;
    if (title.length() > characterLimit) {
      animation_->animating = true;
      marqueePositionMaxed =
          animationTick < title.length() ? animationTick : title.length();
      if (marqueePositionMaxed < 0) {
        marqueePositionMaxed = 0;
      }
    } else {
      animation_->animating = false;
    }
    std::string marqueeTitle = title.erase(0, marqueePositionMaxed);
    display_buffer_->filled_rectangle(
        0, yPos, display_buffer_->get_width(),
        display_state_->get_medium_font()->get_height() +
            display_state_->margin_size_,
        display_state_->color_accent_primary_);
    display_buffer_->printf(
        xPos, textYPos, display_state_->get_medium_font(),
        text_helpers_->secondaryTextColor(display_state_->dark_mode_),
        display::TextAlign::TOP_LEFT, "%s", marqueeTitle.c_str());
  } else {
    display_buffer_->printf(
        xPos, textYPos, display_state_->get_medium_font(),
        text_helpers_->primaryTextColor(display_state_->dark_mode_),
        display::TextAlign::TOP_LEFT, "%s", title.c_str());
  }
}

void HomeThingMenuDisplay::drawMenu(
    std::vector<std::shared_ptr<MenuTitleBase>> active_menu,
    const int menuIndex) {
  if (idleTime > 16 && !charging) {
    menuDrawing = false;
    ESP_LOGW("WARNING", "not drawing");
    return;
  }

  if (!display_state_->dark_mode_ && activeMenuState != bootMenu) {
    display_buffer_->fill(display_state_->color_white_);
  }
  if (speakerGroup != NULL && speakerGroup->playerSearchFinished == false) {
    boot_->drawBootSequence();
    return;
  } else if (activeMenuState == bootMenu) {
    ESP_LOGW("WARNING", "finished boot");
    activeMenuState = rootMenu;
  } else {
    ESP_LOGW("WARNING", "wtf %s", menu_state_title(activeMenuState).c_str());
  }
  if (autoClearState != 0) {
    display_buffer_->set_auto_clear(true);
    autoClearState = 0;
  }
  switch (activeMenuState) {
    case nowPlayingMenu:
      now_playing_->drawNowPlaying();
      break;
    case lightsMenu:
      draw_menu(lightTitleSwitches(lightGroup->lights), menuIndex);
      break;
    case lightsDetailMenu:
      if (lightGroup->getActiveLight() != NULL) {
        draw_menu(lightTitleItems(lightGroup->getActiveLight(),
                                  display_buffer_->get_width()),
                  menuIndex);
      }
      break;
    case switchesMenu:
      draw_menu(switchTitleSwitches(switchGroup->switches), menuIndex);
      break;
    case groupMenu: {
      if (speakerGroup->newSpeakerGroupParent != NULL) {
        auto playerSwitches = groupTitleSwitches(
            speakerGroup->media_players_, speakerGroup->newSpeakerGroupParent);
        draw_menu({playerSwitches.begin(), playerSwitches.end()}, menuIndex);
        break;
      }
      auto playerStrings = groupTitleString(speakerGroup->media_players_);
      draw_menu({playerStrings.begin(), playerStrings.end()}, menuIndex);
      break;
    }
    default:
      draw_menu(active_menu, menuIndex);
      break;
  }
  header_->drawHeader(0);
  menuDrawing = false;
}

void HomeThingMenuDisplay::draw_menu(
    std::vector<std::shared_ptr<MenuTitleBase>> menuTitles, int menuIndex) {
  activeMenuTitleCount = menuTitles.size();
  if (menuTitles.size() == 0) {
    return;
  }
  scrollMenuPosition();
  int menuState = menuIndex;
  auto activeMenuTitle = menuTitles[menuIndex];
  int yPos = display_state_->header_height_;
  int sliderExtra = 0;  // fake menu items as the slider uses two rows
  for (int i = scrollTop; i < menuTitles.size(); i++) {
    if (i + sliderExtra > scrollTop + maxItems()) {
      break;
    }
    switch (menuTitles[i]->titleType) {
      case BaseMenuTitleType:
        drawTitle(menuState, i, menuTitles[i]->get_name(), yPos, false);
        drawRightTitleIcon(menuTitles, i, menuState, yPos);
        yPos += display_state_->get_medium_font()->get_height() +
                display_state_->margin_size_;
        break;
      case LightMenuTitleType: {
        auto lightTitle =
            std::static_pointer_cast<MenuTitleLight>(menuTitles[i]);
        if (lightTitle != NULL) {
          drawTitle(menuState, i, menuTitles[i]->get_name(), yPos,
                    lightTitle->indentLine());
          drawLeftTitleIcon(menuTitles, lightTitle, i, menuState, yPos);
          drawRightTitleIcon(menuTitles, i, menuState, yPos);
          yPos += display_state_->get_medium_font()->get_height() +
                  display_state_->margin_size_;
        }
        break;
      }
      case ToggleMenuTitleType: {
        auto toggleTitle =
            std::static_pointer_cast<MenuTitleToggle>(menuTitles[i]);
        if (toggleTitle != NULL) {
          drawTitle(menuState, i, menuTitles[i]->get_name(), yPos,
                    toggleTitle->indentLine());
          drawLeftTitleIcon(menuTitles, toggleTitle, i, menuState, yPos);
          drawRightTitleIcon(menuTitles, i, menuState, yPos);
          yPos += display_state_->get_medium_font()->get_height() +
                  display_state_->margin_size_;
        }
        break;
      }
      case SliderMenuTitleType: {
        bool lightDetailSelected = lightGroup->lightDetailSelected;
        auto item = std::static_pointer_cast<MenuTitleSlider>(menuTitles[i]);
        SliderSelectionState sliderState =
            menuState == i && lightDetailSelected ? SliderSelectionStateActive
            : menuState == i                      ? SliderSelectionStateHover
                                                  : SliderSelectionStateNone;
        refactor_->drawLightSlider(display_state_->slider_margin_size_, yPos,
                                   sliderState, item, i == 2);
        sliderExtra += 0;

        yPos += (display_state_->get_medium_font()->get_height() +
                 display_state_->margin_size_) *
                2;
        break;
      }
      case PlayerMenuTitleType: {
        auto playerTitle =
            std::static_pointer_cast<MenuTitlePlayer>(menuTitles[i]);
        if (playerTitle != NULL) {
          drawTitle(menuState, i, menuTitles[i]->get_name(), yPos,
                    playerTitle->indentLine());
          int length = playerTitle->get_name().length() +
                       (playerTitle->indentLine() ? 2 : 0);
          drawTitleImage(length, yPos, playerTitle->media_player_->playerState,
                         menuState == i);
          drawLeftTitleIcon(menuTitles, playerTitle, i, menuState, yPos);
          drawRightTitleIcon(menuTitles, i, menuState, yPos);
          yPos += display_state_->get_medium_font()->get_height() +
                  display_state_->margin_size_;
        }
        break;
      }
    }
  }
  drawScrollBar(menuTitles.size(), display_state_->header_height_);
}

void HomeThingMenuDisplay::drawScrollBar(int menuTitlesCount,
                                         int headerHeight) {
  int scrollBarMargin = 1;
  int scrollBarWidth = display_state_->scroll_bar_width_;
  if (menuTitlesCount > maxItems() + 1) {
    double screenHeight = display_buffer_->get_height() - headerHeight;
    double height = maxItems() * (screenHeight / menuTitlesCount);
    double yPos =
        (((screenHeight - height) / (menuTitlesCount - 1)) * menuIndex) + 1 +
        headerHeight;
    display_buffer_->filled_rectangle(
        display_buffer_->get_width() - scrollBarWidth, headerHeight,
        scrollBarWidth, screenHeight, display_state_->color_gray_dark_2_);
    display_buffer_->filled_rectangle(
        display_buffer_->get_width() - scrollBarWidth + scrollBarMargin, yPos,
        scrollBarWidth - scrollBarMargin * 2, height - 1,
        display_state_->color_accent_primary_);
  }
}

void HomeThingMenuDisplay::scrollMenuPosition() {
  int menuState = menuIndex;

  if (menuState - maxItems() > scrollTop) {
    scrollTop = menuState - maxItems();
    // menu down
  } else if (menuState - scrollTop < 0) {
    scrollTop = menuState;
    // menu up
  }
}

int HomeThingMenuDisplay::maxItems() {
  int maxItems =
      ((display_buffer_->get_height() - display_state_->header_height_) /
       (display_state_->get_medium_font()->get_height() +
        display_state_->margin_size_)) -
      1;
  return maxItems;
}

void HomeThingMenuDisplay::drawLeftTitleIcon(
    std::vector<std::shared_ptr<MenuTitleBase>> menuTitles,
    std::shared_ptr<MenuTitleToggle> toggleTitle, int i, int menuState,
    int yPos) {
  switch (toggleTitle->leftIconState) {
    case NoMenuTitleLeftIcon:
      break;
    case OffMenuTitleLeftIcon:
    case OnMenuTitleLeftIcon:
      if (toggleTitle->titleType == LightMenuTitleType) {
        auto lightToggleTitle =
            std::static_pointer_cast<MenuTitleLight>(toggleTitle);
        if (lightToggleTitle != NULL) {
          auto lightColor =
              lightToggleTitle->leftIconState == OnMenuTitleLeftIcon
                  ? lightToggleTitle->lightColor
                  : display_state_->color_white_;
          refactor_->drawLightSwitch(
              lightToggleTitle->leftIconState == OnMenuTitleLeftIcon, yPos,
              lightColor, menuState == i);
        }
      } else {
        refactor_->drawSwitch(toggleTitle->leftIconState == OnMenuTitleLeftIcon,
                              yPos);
      }
      break;
    case GroupedMenuTitleLeftIcon:
      bool extend = false;
      if (i < menuTitles.size() - 1) {
        auto nextToggleTitle =
            std::static_pointer_cast<MenuTitleToggle>(menuTitles[i + 1]);
        if (nextToggleTitle != NULL) {
          extend = nextToggleTitle->leftIconState == GroupedMenuTitleLeftIcon;
        }
      }
      refactor_->drawGroupedBar(yPos, extend);
      break;
  }
}

void HomeThingMenuDisplay::drawRightTitleIcon(
    std::vector<std::shared_ptr<MenuTitleBase>> menuTitles, int i,
    int menuState, int yPos) {
  switch (menuTitles[i]->rightIconState) {
    case NoMenuTitleRightIcon:
      break;
    case ArrowMenuTitleRightIcon:
      if (menuState == i) {
        refactor_->drawArrow(yPos, menuTitles.size(), maxItems());
      }
      break;
  }
}

void HomeThingMenuDisplay::skipBootSequence() {
  boot_->skipBootSequence();
}

// private
void HomeThingMenuDisplay::drawTitleImage(
    int characterCount, int yPos,
    homeassistant_media_player::RemotePlayerState& titleState, bool selected) {
  int adjustedYPos = yPos;
  int xPos = ((characterCount + 0.5) *
              (display_state_->get_medium_font()->get_height() *
               display_state_->font_size_width_ratio_)) +
             4;
  auto color = selected
                   ? text_helpers_->primaryTextColor(display_state_->dark_mode_)
                   : display_state_->color_accent_primary_;
  switch (titleState) {
    case homeassistant_media_player::RemotePlayerState::
        PlayingRemotePlayerState:
      display_buffer_->printf(
          xPos, yPos, display_state_->get_material_font_large(), color, "󰐊");
      break;
    case homeassistant_media_player::RemotePlayerState::PausedRemotePlayerState:
      display_buffer_->printf(
          xPos, yPos, display_state_->get_material_font_large(), color, "󰏤");
      break;
    case homeassistant_media_player::RemotePlayerState::
        StoppedRemotePlayerState:
      display_buffer_->printf(
          xPos, yPos, display_state_->get_material_font_large(), color, "󰓛");
      break;
    case homeassistant_media_player::RemotePlayerState::
        PowerOffRemotePlayerState:
      display_buffer_->printf(
          xPos, yPos, display_state_->get_material_font_large(), color, "󰽥");
      break;
    default:
      break;
  }
}

void HomeThingMenuDisplay::updateDisplay(bool force) {
  displayUpdate->updateDisplay(force);
}
}  // namespace homething_menu_base
}  // namespace esphome
