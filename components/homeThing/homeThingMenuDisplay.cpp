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
        display_state_->get_color_accent_primary());
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

bool HomeThingMenuDisplay::draw_menu_titles(
    std::vector<std::shared_ptr<MenuTitleBase>> menuTitles, int menuIndex) {
  if (menuTitles.size() == 0 || menuTitles.size() < menuIndex) {
    return false;
  }
  scrollMenuPosition(menuIndex);
  int menuState = menuIndex;
  auto activeMenuTitle = menuTitles[menuIndex];
  int yPos = display_state_->get_header_height();
  int sliderExtra = 0;  // fake menu items as the slider uses two rows
  bool animating = false;
  for (int i = scrollTop; i < menuTitles.size(); i++) {
    if (i + sliderExtra > scrollTop + maxItems()) {
      break;
    }
    switch (menuTitles[i]->titleType) {
      case BaseMenuTitleType:
      case SourceMenuTitleType:
        animating = draw_menu_title(menuState, i, menuTitles[i]->get_name(),
                                    yPos, false) ||
                    animating;
        drawRightTitleIcon(menuTitles, i, menuState, yPos);
        yPos += display_state_->get_font_medium()->get_baseline() +
                display_state_->get_margin_size();
        break;
      case LightMenuTitleType: {
        auto lightTitle =
            std::static_pointer_cast<MenuTitleLight>(menuTitles[i]);
        if (lightTitle != NULL) {
          animating = draw_menu_title(menuState, i, menuTitles[i]->get_name(),
                                      yPos, lightTitle->indentLine());
          drawLeftTitleIcon(menuTitles, lightTitle, i, menuState, yPos);
          drawRightTitleIcon(menuTitles, i, menuState, yPos);
          yPos += display_state_->get_font_medium()->get_baseline() +
                  display_state_->get_margin_size();
        }
        break;
      }
      case ToggleMenuTitleType: {
        auto toggleTitle =
            std::static_pointer_cast<MenuTitleToggle>(menuTitles[i]);
        if (toggleTitle != NULL) {
          animating = draw_menu_title(menuState, i, menuTitles[i]->get_name(),
                                      yPos, toggleTitle->indentLine());
          drawLeftTitleIcon(menuTitles, toggleTitle, i, menuState, yPos);
          drawRightTitleIcon(menuTitles, i, menuState, yPos);
          yPos += display_state_->get_font_medium()->get_baseline() +
                  display_state_->get_margin_size();
        }
        break;
      }
      case SliderMenuTitleType: {
        bool lightDetailSelected = light_group_->lightDetailSelected;
        auto item = std::static_pointer_cast<MenuTitleSlider>(menuTitles[i]);
        SliderSelectionState sliderState =
            menuState == i && lightDetailSelected ? SliderSelectionStateActive
            : menuState == i                      ? SliderSelectionStateHover
                                                  : SliderSelectionStateNone;
        refactor_->drawLightSlider(0, yPos, sliderState, item, i == 2);
        sliderExtra += 0;

        yPos += (display_state_->get_font_medium()->get_baseline() +
                 display_state_->get_margin_size()) *
                2;
        break;
      }
      case PlayerMenuTitleType: {
        auto playerTitle =
            std::static_pointer_cast<MenuTitlePlayer>(menuTitles[i]);
        if (playerTitle != NULL) {
          draw_menu_title(menuState, i, menuTitles[i]->get_name(), yPos,
                          playerTitle->indentLine());
          int length = playerTitle->get_name().length() +
                       (playerTitle->indentLine() ? 2 : 0);
          drawTitleImage(length, yPos, playerTitle->media_player_->playerState,
                         menuState == i);
          drawLeftTitleIcon(menuTitles, playerTitle, i, menuState, yPos);
          drawRightTitleIcon(menuTitles, i, menuState, yPos);
          yPos += display_state_->get_font_medium()->get_baseline() +
                  display_state_->get_margin_size();
        }
        break;
      }
    }
  }
  drawScrollBar(menuTitles.size(), display_state_->get_header_height(),
                menuIndex);
  return animating;
}

bool HomeThingMenuDisplay::draw_menu_screen(
    MenuStates* activeMenuState,
    std::vector<std::shared_ptr<MenuTitleBase>> active_menu,
    const int menuIndex, const option_menuType option_menu) {
  if (!display_state_->get_dark_mode() && *activeMenuState != bootMenu) {
    display_buffer_->fill(display_state_->get_color_white());
  }
  if (!boot_->boot_complete()) {
    return boot_->drawBootSequence(*activeMenuState);
  } else if (*activeMenuState == bootMenu) {
    ESP_LOGW(TAG, "finished boot");
    *activeMenuState = rootMenu;
    return true;
  }

  bool animating = false;
  switch (*activeMenuState) {
    case nowPlayingMenu:
      now_playing_->drawNowPlaying(menuIndex, option_menu, active_menu);
      break;
    case groupMenu: {
      if (media_player_group_->newSpeakerGroupParent != NULL) {
        auto playerSwitches =
            groupTitleSwitches(media_player_group_->media_players_,
                               media_player_group_->newSpeakerGroupParent);
        animating = draw_menu_titles(
            {playerSwitches.begin(), playerSwitches.end()}, menuIndex);
        break;
      }
      auto playerStrings =
          groupTitleString(media_player_group_->media_players_);
      animating = draw_menu_titles({playerStrings.begin(), playerStrings.end()},
                                   menuIndex);
      break;
    }
    default:
      animating = draw_menu_titles(active_menu, menuIndex);
      break;
  }
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
        scrollBarWidth, screenHeight, display_state_->get_color_gray_dark_2());
    display_buffer_->filled_rectangle(
        display_buffer_->get_width() - scrollBarWidth + scrollBarMargin, yPos,
        scrollBarWidth - scrollBarMargin * 2, height - 1,
        display_state_->get_color_accent_primary());
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
                  : display_state_->get_color_white();
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

void HomeThingMenuDisplay::skipBootSequence(const MenuStates activeMenuState) {
  boot_->skipBootSequence(activeMenuState);
}

// private
void HomeThingMenuDisplay::drawTitleImage(
    int characterCount, int yPos,
    const homeassistant_media_player::RemotePlayerState& titleState,
    bool selected) {

  ESP_LOGW(TAG, "draw title image %d", titleState);
  int adjustedYPos = yPos;
  int xPos = ((characterCount + 0.5) *
              (display_state_->get_font_medium()->get_baseline() *
               display_state_->get_font_size_width_ratio())) +
             4;
  auto color =
      selected
          ? text_helpers_->primaryTextColor(display_state_->get_dark_mode())
          : display_state_->get_color_accent_primary();
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

void HomeThingMenuDisplay::updateDisplay(bool force) {
  // displayUpdate->updateDisplay(force);
}
}  // namespace homething_menu_base
}  // namespace esphome
