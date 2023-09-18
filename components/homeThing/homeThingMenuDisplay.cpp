#include "homeThingMenuDisplay.h"
#include "esphome/core/log.h"

namespace esphome {
namespace homething_menu_base {

void HomeThingMenuDisplay::setup() {}

bool HomeThingMenuDisplay::draw_menu_title(int menuState, int i,
                                           std::string title, int yPos,
                                           bool buttonSpace) {
  bool animating = false;
  int xPos = buttonSpace ? display_state_->get_font_small()->get_baseline() +
                               display_state_->get_margin_size() * 2
                         : display_state_->get_margin_size();
  int textYPos = yPos + (display_state_->get_margin_size() / 4);
  if (menuState == i) {
    int characterHeight = display_state_->get_font_medium()->get_baseline();
    int characterLimit = text_helpers_->getCharacterLimit(
        xPos, characterHeight, display::TextAlign::TOP_LEFT);
    ESP_LOGD(TAG, "characterLimit %d, title %d height %d", characterLimit,
             title.length(), characterHeight);

    int marqueePositionMaxed = 0;
    if (title.length() > characterLimit) {
      animating = true;
      const int animationTick =
          static_cast<int>(animation_->animationTick->state) % title.length();
      marqueePositionMaxed = animationTick;
      if (marqueePositionMaxed < 0) {
        marqueePositionMaxed = 0;
      }
      // animation_->marqueeTick(title.length());
    }
    std::string marqueeTitle = title.erase(0, marqueePositionMaxed);
    display_buffer_->filled_rectangle(
        0, yPos, display_buffer_->get_width(),
        display_state_->get_font_medium()->get_baseline() +
            display_state_->get_margin_size(),
        display_state_->get_color_palette()->get_accent_primary());
    display_buffer_->printf(
        xPos, textYPos, display_state_->get_font_medium(),
        text_helpers_->secondaryTextColor(display_state_->get_dark_mode()),
        display::TextAlign::TOP_LEFT, "%s", marqueeTitle.c_str());
  } else {
    display_buffer_->printf(
        xPos, textYPos, display_state_->get_font_medium(),
        text_helpers_->primaryTextColor(display_state_->get_dark_mode()),
        display::TextAlign::TOP_LEFT, "%s", title.c_str());
  }
  return animating;
}

void HomeThingMenuDisplay::draw_lock_screen(int unlock_presses) {
  double yPos = display_buffer_->get_height() / 2;
  double xPos = display_buffer_->get_width() / 2;
  double box_size_height = display_buffer_->get_height() * 0.6;
  double box_size_width = display_buffer_->get_width() * 0.8;
  auto text_font = display_state_->get_font_large();
  auto number_font = display_state_->get_font_large_heavy();
  auto background_color =
      display_state_->get_color_palette()->get_accent_primary();
  auto text_color =
      text_helpers_->primaryTextColor(display_state_->get_dark_mode());
  display_buffer_->filled_rectangle(xPos - box_size_width / 2,
                                    yPos - box_size_height / 2, box_size_width,
                                    box_size_height, background_color);
  display_buffer_->printf(xPos, yPos - text_font->get_baseline(), text_font,
                          text_color, display::TextAlign::CENTER, "Locked,");
  display_buffer_->printf(xPos, yPos, text_font, text_color,
                          display::TextAlign::CENTER, "Press Option");
  display_buffer_->printf(xPos, yPos + text_font->get_baseline(), number_font,
                          text_color, display::TextAlign::CENTER, "%d/2",
                          unlock_presses);
}

bool HomeThingMenuDisplay::draw_menu_titles(
    const std::vector<MenuTitleBase*>* menuTitles, int menuIndex,
    bool editing_menu_item) {
  if (menuTitles->size() == 0 || menuTitles->size() < menuIndex) {
    return false;
  }
  scrollMenuPosition(menuIndex);
  int menuState = menuIndex;
  auto activeMenuTitle = (*menuTitles)[menuIndex];
  int yPos = display_state_->get_header_height();
  int sliderExtra = 0;  // fake menu items as the slider uses two rows
  bool animating = false;
  auto menuTitlesSize = menuTitles->size();
  for (int i = scrollTop; i < menuTitles->size(); i++) {
    if (i + sliderExtra > scrollTop + maxItems()) {
      break;
    }
    auto rightIconState = (*menuTitles)[i]->rightIconState;
    auto titleName = (*menuTitles)[i]->get_name();
    if (i == menuIndex && editing_menu_item) {
      titleName = "*" + titleName;
    }
    ESP_LOGD(TAG, "draw_menu_titles: %s", titleName.c_str());
    switch ((*menuTitles)[i]->titleType) {
      case BaseMenuTitleType:
        animating =
            draw_menu_title(menuState, i, titleName, yPos, false) || animating;
        drawRightTitleIcon(menuTitlesSize, rightIconState, i, menuState, yPos);
        yPos += display_state_->get_font_medium()->get_baseline() +
                display_state_->get_margin_size();
        break;
      case SourceMenuTitleType:
#ifdef USE_MEDIA_PLAYER_GROUP
        animating =
            draw_menu_title(menuState, i, titleName, yPos, false) || animating;
        yPos += display_state_->get_font_medium()->get_baseline() +
                display_state_->get_margin_size();
#endif
        break;
      case LightMenuTitleType: {
#ifdef USE_LIGHT
        auto lightTitle = static_cast<MenuTitleLight*>((*menuTitles)[i]);
        if (lightTitle != NULL) {
          animating = draw_menu_title(menuState, i, titleName, yPos,
                                      lightTitle->indentLine());
          drawLeftTitleIcon(menuTitlesSize, lightTitle, i, menuState, yPos);
          drawRightTitleIcon(menuTitlesSize, rightIconState, i, menuState,
                             yPos);
          yPos += display_state_->get_font_medium()->get_baseline() +
                  display_state_->get_margin_size();
        }
#endif
        break;
      }
      case ToggleMenuTitleType: {
        auto toggleTitle = static_cast<MenuTitleToggle*>((*menuTitles)[i]);
        if (toggleTitle != NULL) {
          animating = draw_menu_title(menuState, i, titleName, yPos,
                                      toggleTitle->indentLine());
          drawLeftTitleIcon(menuTitlesSize, toggleTitle, i, menuState, yPos);
          drawRightTitleIcon(menuTitlesSize, rightIconState, i, menuState,
                             yPos);
          yPos += display_state_->get_font_medium()->get_baseline() +
                  display_state_->get_margin_size();
        }
        break;
      }
      case SliderMenuTitleType: {
#ifdef USE_LIGHT
        auto item = static_cast<MenuTitleSlider*>((*menuTitles)[i]);
        SliderSelectionState sliderState =
            menuState == i && editing_menu_item ? SliderSelectionStateActive
            : menuState == i                    ? SliderSelectionStateHover
                                                : SliderSelectionStateNone;
        refactor_->drawLightSlider(0, yPos, sliderState, item, i == 2);
        sliderExtra += 0;

        yPos += (display_state_->get_font_medium()->get_baseline() +
                 display_state_->get_margin_size()) *
                2;
#endif
        break;
      }
      case PlayerMenuTitleType: {
#ifdef USE_MEDIA_PLAYER_GROUP
        auto playerTitle = static_cast<MenuTitlePlayer*>((*menuTitles)[i]);
        if (playerTitle != NULL) {
          draw_menu_title(menuState, i, titleName, yPos,
                          playerTitle->indentLine());
          int length = playerTitle->get_name().length() +
                       (playerTitle->indentLine() ? 2 : 0);
          drawTitleImage(length, yPos, playerTitle->media_player_->playerState,
                         menuState == i);
          drawLeftTitleIcon(menuTitlesSize, playerTitle, i, menuState, yPos);
          drawRightTitleIcon(menuTitlesSize, rightIconState, i, menuState,
                             yPos);
          yPos += display_state_->get_font_medium()->get_baseline() +
                  display_state_->get_margin_size();
        }
#endif
        break;
      }
    }
  }
  drawScrollBar(menuTitlesSize, display_state_->get_header_height(), menuIndex);
  return animating;
}

bool HomeThingMenuDisplay::draw_menu_screen(
    MenuStates* activeMenuState, const std::vector<MenuTitleBase*>* active_menu,
    const int menuIndex,
    homething_menu_now_playing::HomeThingOptionMenu* option_menu,
    bool editing_menu_item) {
  bool boot_complete = boot_->boot_complete();
  if (!display_state_->get_dark_mode() && *activeMenuState != bootMenu) {
    display_buffer_->fill(display_state_->get_color_palette()->get_white());
  }
  if (!boot_complete && *activeMenuState == bootMenu) {
    return boot_->drawBootSequence(*activeMenuState);
  } else if (boot_complete && *activeMenuState == bootMenu) {
    ESP_LOGW(TAG, "finished boot");
    *activeMenuState = rootMenu;
    return true;
  } else if (!boot_complete && *activeMenuState != bootMenu) {
    ESP_LOGW(TAG, "boot not complete but we got to the menu %d state %d",
             boot_complete, *activeMenuState);
  }

  bool animating = draw_menu_titles(active_menu, menuIndex, editing_menu_item);
  header_->drawHeader(0, *activeMenuState);
  return animating;
}

void HomeThingMenuDisplay::drawScrollBar(int menuTitlesCount, int headerHeight,
                                         int menuIndex) {
  int scrollBarMargin = 1;
  int scrollBarWidth = display_state_->get_scroll_bar_width();
  if (menuTitlesCount > maxItems() + 1) {
    double screenHeight = display_buffer_->get_height() - headerHeight;
    double height = maxItems() * (screenHeight / menuTitlesCount);
    double yPos =
        (((screenHeight - height) / (menuTitlesCount - 1)) * menuIndex) + 1 +
        headerHeight;
    display_buffer_->filled_rectangle(
        display_buffer_->get_width() - scrollBarWidth, headerHeight,
        scrollBarWidth, screenHeight,
        display_state_->get_color_palette()->get_gray_dark_2());
    display_buffer_->filled_rectangle(
        display_buffer_->get_width() - scrollBarWidth + scrollBarMargin, yPos,
        scrollBarWidth - scrollBarMargin * 2, height - 1,
        display_state_->get_color_palette()->get_accent_primary());
  }
}

void HomeThingMenuDisplay::scrollMenuPosition(int menuIndex) {
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
      ((display_buffer_->get_height() - display_state_->get_header_height()) /
       (display_state_->get_font_medium()->get_baseline() +
        display_state_->get_margin_size())) -
      1;
  return maxItems;
}

void HomeThingMenuDisplay::drawLeftTitleIcon(int menuTitleSize,
                                             const MenuTitleToggle* toggleTitle,
                                             int i, int menuState, int yPos) {
  switch (toggleTitle->leftIconState) {
    case NoMenuTitleLeftIcon:
      break;
    case OffMenuTitleLeftIcon:
    case OnMenuTitleLeftIcon:
      if (toggleTitle->titleType == LightMenuTitleType) {
#ifdef USE_LIGHT
        auto lightToggleTitle = static_cast<const MenuTitleLight*>(toggleTitle);
        if (lightToggleTitle != NULL) {
          auto lightColor =
              lightToggleTitle->leftIconState == OnMenuTitleLeftIcon
                  ? lightToggleTitle->lightColor
                  : display_state_->get_color_palette()->get_white();
          refactor_->drawLightSwitch(
              lightToggleTitle->leftIconState == OnMenuTitleLeftIcon, yPos,
              lightColor, menuState == i);
        }
#endif
      } else {
        refactor_->drawSwitch(toggleTitle->leftIconState == OnMenuTitleLeftIcon,
                              yPos);
      }
      break;
    case GroupedMenuTitleLeftIcon:
      bool extend = false;
      // if (i < menuTitleSize - 1) {
      //   auto nextToggleTitle =
      //       std::static_pointer_cast<MenuTitleToggle>(menuTitles[i + 1]);
      //   if (nextToggleTitle != NULL) {
      //     extend = nextToggleTitle->leftIconState == GroupedMenuTitleLeftIcon;
      //   }
      // }
      refactor_->drawGroupedBar(yPos, extend);
      break;
  }
}

void HomeThingMenuDisplay::drawRightTitleIcon(int menuTitleSize,
                                              MenuTitleRightIcon iconState,
                                              int i, int menuState, int yPos) {
  switch (iconState) {
    case NoMenuTitleRightIcon:
      break;
    case ArrowMenuTitleRightIcon:
      if (menuState == i) {
        refactor_->drawArrow(yPos, menuTitleSize, maxItems());
      }
      break;
  }
}

// private
#ifdef USE_MEDIA_PLAYER_GROUP
void HomeThingMenuDisplay::drawTitleImage(
    int characterCount, int yPos,
    const homeassistant_media_player::RemotePlayerState& titleState,
    bool selected) {

  ESP_LOGD(TAG, "draw title image %d", titleState);
  int adjustedYPos = yPos;
  int xPos = ((characterCount + 0.5) *
              (display_state_->get_font_medium()->get_baseline() *
               display_state_->get_font_size_width_ratio())) +
             4;
  auto color =
      selected
          ? text_helpers_->primaryTextColor(display_state_->get_dark_mode())
          : display_state_->get_color_palette()->get_accent_primary();
  switch (titleState) {
    case homeassistant_media_player::RemotePlayerState::
        PlayingRemotePlayerState:
      display_buffer_->printf(
          xPos, yPos, display_state_->get_font_material_large(), color, "󰐊");
      break;
    case homeassistant_media_player::RemotePlayerState::PausedRemotePlayerState:
      display_buffer_->printf(
          xPos, yPos, display_state_->get_font_material_large(), color, "󰏤");
      break;
    case homeassistant_media_player::RemotePlayerState::
        StoppedRemotePlayerState:
      display_buffer_->printf(
          xPos, yPos, display_state_->get_font_material_large(), color, "󰓛");
      break;
    case homeassistant_media_player::RemotePlayerState::
        PowerOffRemotePlayerState:
      display_buffer_->printf(
          xPos, yPos, display_state_->get_font_material_large(), color, "󰽥");
      break;
    default:
      break;
  }
}
#endif

void HomeThingMenuDisplay::updateDisplay(bool force) {
  // displayUpdate->updateDisplay(force);
}
}  // namespace homething_menu_base
}  // namespace esphome
