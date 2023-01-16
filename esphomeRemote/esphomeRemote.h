#include "MenuGlobals.h"
#include "TextHelpers.h"
#include "esphome.h"
#include "esphomeRemoteLight.h"
#include "esphomeRemoteSwitch.h"
#include "esphomeRemotePlayer.h"
#include "esphomeRemoteSensor.h"
#include "esphomeRemoteService.h"

#pragma once

bool menuDrawing = false;

class DisplayUpdateImpl : public DisplayUpdateInterface {
 public:
  virtual void updateDisplay(bool force) {
    switch(activeMenuState) {
    case bootMenu:
      return;
    default:
      break;
    }
    if (menuDrawing && id(backlight).state) {
      ESP_LOGW("WARNING", "menu already drawing");
      return;
    }
    if (force) {
      menuDrawing = true;
      id(my_display).update();
      if (!id(backlight).state) {
        id(backlight).turn_on();
      }
      return;
    }

    if (id(backlight).state) {
      switch (activeMenuState) {
        case sourcesMenu:
          return;
        default:
          menuDrawing = true;
          id(my_display).update();
          break;
      }
    }
  }

  virtual void setUpdateInterval(int updateInterval) {
    ESP_LOGI("setUpdateInterval", "setting display update interval %d", updateInterval);
    id(my_display).set_update_interval(updateInterval);
    id(my_display).call_setup();
  }
};

auto displayUpdate = DisplayUpdateImpl();
SceneGroupComponent *sceneGroup;
SensorGroupComponent *sensorGroup;
SonosSpeakerGroupComponent *speakerGroup;
LightGroupComponent *lightGroup;
SwitchGroupComponent *switchGroup;
std::shared_ptr<MenuTitleBase> activeMenuTitle = std::make_shared<MenuTitleBase>("", "", NoMenuTitleRightIcon);
double marqueePosition = 0;
bool marqueeText = false;

Color primaryTextColor() {
  if (id(dark_mode)) {
    return id(my_white);
  } else {
    return id(my_black);
  }
}

Color secondaryTextColor() {
  if (id(dark_mode)) {
    return id(my_white);
  } else {
    return id(my_white);
  }
}

void resetMarquee() { marqueePosition = 0; }

std::string textWrap(std::string text, unsigned per_line) {
  unsigned line_begin = 0;
  while (line_begin < text.size()) {
    const unsigned ideal_end = line_begin + per_line;
    unsigned line_end = ideal_end <= text.size() ? ideal_end : text.size() - 1;

    if (line_end == text.size() - 1) {
      ++line_end;
    } else if (std::isspace(text[line_end])) {
      text[line_end] = '\n';
      ++line_end;
    } else {
      // backtrack
      unsigned end = line_end;
      while (end > line_begin && !std::isspace(text[end]))
        --end;

      if (end != line_begin) {
        line_end = end;
        text[line_end++] = '\n';
      } else {
        text.insert(line_end++, 1, '\n');
      }
    }

    line_begin = line_end;
  }

  return text;
}

int getCharacterLimit(int xPos, int fontSize, TextAlign alignment) {
  int characterLimit = (id(my_display).get_width() - xPos) / (fontSize * id(font_size_width_ratio)) - 1;
  if (xPos == id(my_display).get_width() / 2 && alignment == TextAlign::TOP_CENTER) {
    characterLimit = id(my_display).get_width() / (fontSize * id(font_size_width_ratio)) - 1;
  }
  return characterLimit;
}

std::shared_ptr<MenuTitleBase> menuTitleForType(MenuStates stringType) {
  switch (stringType) {
    case nowPlayingMenu:
      return std::make_shared<MenuTitleBase>("Now Playing", "", ArrowMenuTitleRightIcon);
    case sourcesMenu:
      return std::make_shared<MenuTitleBase>("Sources", "", ArrowMenuTitleRightIcon);
    case backlightMenu:
      return std::make_shared<MenuTitleBase>("Backlight", "", NoMenuTitleRightIcon);
    case sleepMenu:
      return std::make_shared<MenuTitleBase>("Sleep", "", NoMenuTitleRightIcon);
    case mediaPlayersMenu:
      return std::make_shared<MenuTitleBase>("Media Players", "", ArrowMenuTitleRightIcon);
    case lightsMenu:
      return std::make_shared<MenuTitleBase>("Lights", "", ArrowMenuTitleRightIcon);
    case lightsDetailMenu:
      return std::make_shared<MenuTitleBase>("Light Detail", "", ArrowMenuTitleRightIcon);
    case switchesMenu:
      return std::make_shared<MenuTitleBase>("Switches", "", ArrowMenuTitleRightIcon);
    case scenesMenu:
      return std::make_shared<MenuTitleBase>("Scenes and Actions", "", ArrowMenuTitleRightIcon);
    case rootMenu:
      return std::make_shared<MenuTitleBase>("Home", "", NoMenuTitleRightIcon);
    case groupMenu:
      return std::make_shared<MenuTitleBase>("Speaker Group", "", ArrowMenuTitleRightIcon);
    case sensorsMenu:
      return std::make_shared<MenuTitleBase>("Sensors", "", ArrowMenuTitleRightIcon);
    case bootMenu:
      return std::make_shared<MenuTitleBase>("Boot", "", NoMenuTitleRightIcon);
  }
  return std::make_shared<MenuTitleBase>("", "", NoMenuTitleRightIcon);
}

void goToScreenFromString(std::string screenName) {
  if (screenName == "nowPlaying") {
    activeMenuState = nowPlayingMenu;
  } else if (screenName == "sensors") {
    activeMenuState = sensorsMenu;
  }
  menuIndex = 0;
  displayUpdate.updateDisplay(true);
}

int getTextWidth(int fontSize, int characterCount) { return (fontSize * id(font_size_width_ratio) * characterCount); }

int getHeaderTextYPos(int yPosOffset) { return ((id(header_height) - id(small_font_size) * 1.2) / 2) - yPosOffset; }

int drawPlayPauseIcon(int oldXPos, MenuTitlePlayer menuTitle) {
  int yPos = getHeaderTextYPos(0);
  int xPos = oldXPos;
  switch (menuTitle.playerState) {
    case PlayingRemotePlayerState: {
      id(my_display)
          .printf(xPos, yPos, &id(material_font_small), menuTitle.mediaSourceIconColor(),
                  menuTitle.mediaSourceIcon().c_str());
      break;
    }
    case PausedRemotePlayerState:
      id(my_display).printf(xPos, yPos, &id(material_font_small), id(color_accent_primary), "󰏤");
      break;
    case StoppedRemotePlayerState:
      id(my_display).printf(xPos, yPos, &id(material_font_small), id(color_accent_primary), "󰓛");
      break;
    case PowerOffRemotePlayerState:
      id(my_display).printf(xPos, yPos, &id(material_font_small), id(color_accent_primary), "󰽥");
      break;
    default:
      return oldXPos;
  }
  return xPos + id(icon_size) + id(margin_size) / 2;
}

void drawHeaderTitleWithString(std::string title, int xPos, int yPosOffset = 0) {
  int yPos = getHeaderTextYPos(yPosOffset);
  id(my_display).printf(xPos, yPos, &id(small_font), primaryTextColor(), title.c_str());
}

int drawHeaderIcon(std::string title, int xPos, Color iconColor) {
  int yPos = getHeaderTextYPos(0);
  id(my_display).printf(xPos, yPos, &id(material_font_small), iconColor, title.c_str());
  return xPos + id(icon_size) + id(margin_size) / 2;
}

void drawHeaderTitle(int yPosOffset) {
  int xPos = 2;
  switch (activeMenuState) {
    case rootMenu:
    case backlightMenu:
    case sleepMenu:
    case nowPlayingMenu: {
      if (speakerGroup != NULL) {
        auto headerMenuTitle = speakerGroup->headerMediaPlayerTitle();
        xPos = drawPlayPauseIcon(xPos, headerMenuTitle);
        drawHeaderTitleWithString((headerMenuTitle).friendlyName, xPos);
      } else {
        drawHeaderTitleWithString("Remote", xPos);
      }
      break;
    }
    case sourcesMenu:
      drawHeaderTitleWithString("Sources", xPos);
      break;
    case groupMenu:
      drawHeaderTitleWithString("Group Speakers", xPos);
      break;
    case mediaPlayersMenu:
      drawHeaderTitleWithString("Media Players", xPos);
      break;
    case scenesMenu:
      drawHeaderTitleWithString("Scenes/Actions", xPos);
      break;
    case lightsMenu:
      drawHeaderTitleWithString("Lights", xPos);
      break;
    case lightsDetailMenu: {
      if (lightGroup->getActiveLight() != NULL) {
        auto activeLight = lightGroup->getActiveLight();
        auto headerMenuTitle = activeLight->friendlyName;
        int newXPos = drawHeaderIcon(activeLight->icon(), xPos, activeLight->rgbLightColor());
        drawHeaderTitleWithString(headerMenuTitle, newXPos + 1);
      } else {
        drawHeaderTitleWithString("LightDetail", xPos);
      }
      break;
    }
    case switchesMenu:
      drawHeaderTitleWithString("Switches", xPos);
      break;
    case sensorsMenu:
      drawHeaderTitleWithString("Sensors", xPos);
      break;
    case bootMenu:
      drawHeaderTitleWithString(id(boot_device_name), xPos, yPosOffset);
      break;
  }
}

int drawVolumeLevel(int oldXPos, int yPosOffset) {
  if (speakerGroup == NULL) {
    return oldXPos;
  }
  if (!id(draw_volume_level)) {
    return oldXPos;
  }
  int xPos = oldXPos - id(margin_size) / 2;
  int yPos = getHeaderTextYPos(yPosOffset);
  id(my_display)
      .printf(xPos, yPos, &id(small_font), primaryTextColor(), TextAlign::TOP_RIGHT, "%.0f%%",
              speakerGroup->getVolumeLevel());
  return xPos;
}

int drawShuffle(int oldXPos, int yPosOffset) {
  if (speakerGroup == NULL || speakerGroup->activePlayer == NULL) {
    return oldXPos;
  }
  if (speakerGroup->activePlayer->playerType == TVRemotePlayerType) {
    return oldXPos;
  }
  if (speakerGroup->activePlayer->playerState != StoppedRemotePlayerState) {
    int xPos = oldXPos - id(icon_size) + id(margin_size) / 2;
    int yPos = getHeaderTextYPos(yPosOffset);
    if (speakerGroup->mediaShuffling()) {
      id(my_display).printf(xPos, yPos, &id(material_font_small), id(color_accent_primary), "󰒝");
    } else if (id(draw_shuffle_disabled)) {
      id(my_display).printf(xPos, yPos, &id(material_font_small), id(color_accent_primary), "󰒞");
    } else {
      return oldXPos;
    }
    return xPos - id(margin_size) / 2;
  }
  return oldXPos;
}

int drawHeaderTime(int oldXPos, int yPosOffset) {
  if (!id(draw_header_time) || !id(esptime).now().is_valid()) {
    return oldXPos;
  }
  switch (activeMenuState) {
    case rootMenu:
    case nowPlayingMenu:
      break;
    default:
      if (id(my_display).get_width() < 200) {
        return oldXPos;
      }
      break;
  }
  int yPos = getHeaderTextYPos(yPosOffset);
  std::string timeString = id(esptime).now().strftime("%I:%M%P");
  if (timeString.length() > 0 && timeString[0] == '0') {
    timeString.erase(0, 1);
  }
  int xPos = oldXPos - getTextWidth(id(small_font_size), timeString.length());
  id(my_display).printf(xPos, yPos, &id(small_font), primaryTextColor(), timeString.c_str());
  return xPos - id(margin_size) / 2;
}

int drawBattery(int oldXPos, int yPosOffset) {
  if (!id(draw_battery_level)) {
    return oldXPos;
  }
  int batteryWidth = 24;
  int batteryHeight = id(header_height) - 5;
  int yPos = 2 - yPosOffset;
  int capHeight = 6;
  int capWidth = 3;
  int xPos = oldXPos - batteryWidth;
  id(my_display).rectangle(xPos, yPos, batteryWidth, batteryHeight, id(my_gray_dark));
  id(my_display)
      .rectangle(xPos + batteryWidth - 1, yPos + (batteryHeight / 2) - (capHeight / 2), capWidth, capHeight,
                 id(my_gray_dark));
  if (id(batteryPercent).state <= 100 && !charging) {
    id(my_display)
        .filled_rectangle(xPos + 1, yPos + 1, (batteryWidth * id(batteryPercent).state * 0.01) - 2, batteryHeight - 2,
                          id(my_green));
  } else {
    id(my_display).filled_rectangle(xPos + 1, yPos + 1, batteryWidth - 2, batteryHeight - 2, id(my_yellow));
  }
  return xPos - id(margin_size);
}

void drawHeader(int yPosOffset) {
  id(my_display).rectangle(0, id(header_height) - yPosOffset, id(my_display).get_width(), 1, id(color_accent_primary));
  drawHeaderTitle(yPosOffset);
  int xPos = id(my_display).get_width() - id(margin_size) / 2;
  drawVolumeLevel(drawHeaderTime(drawShuffle(drawBattery(xPos, yPosOffset), yPosOffset), yPosOffset), yPosOffset);
}

void drawTitle(int menuState, int i, std::string title, int yPos, bool buttonSpace) {
  int xPos = buttonSpace ? id(small_font_size) + id(margin_size) * 2 : id(margin_size);
  int textYPos = yPos + (id(margin_size) / 4);
  if (menuState == i) {
    int characterLimit = getCharacterLimit(xPos, id(medium_font_size), TextAlign::TOP_LEFT);
    if (marqueePosition > title.length() - characterLimit + 4) {
      marqueePosition = -6;
    }
    int marqueePositionMaxed = 0;
    if (title.length() > characterLimit) {
      marqueeText = true;
      marqueePositionMaxed = marqueePosition < title.length() ? marqueePosition : title.length();
      if (marqueePositionMaxed < 0) {
        marqueePositionMaxed = 0;
      }
    } else {
      marqueeText = false;
    }
    std::string marqueeTitle = title.erase(0, marqueePositionMaxed);
    id(my_display)
        .filled_rectangle(0, yPos, id(my_display).get_width(), id(medium_font_size) + id(margin_size),
                          id(color_accent_primary));
    id(my_display)
        .printf(xPos, textYPos, &id(medium_font), secondaryTextColor(), TextAlign::TOP_LEFT, "%s",
                marqueeTitle.c_str());
  } else {
    id(my_display)
        .printf(xPos, textYPos, &id(medium_font), primaryTextColor(), TextAlign::TOP_LEFT, "%s", title.c_str());
  }
}

int maxItems() {
  int maxItems = ((id(my_display).get_height() - id(header_height)) / (id(medium_font_size) + id(margin_size))) - 1;
  return maxItems;
}

void drawScrollBar(int menuTitlesCount, int headerHeight) {
  int scrollBarMargin = 1;
  int scrollBarWidth = id(scroll_bar_width);
  if (menuTitlesCount > maxItems() + 1) {
    double screenHeight = id(my_display).get_height() - headerHeight;
    double height = maxItems() * (screenHeight / menuTitlesCount);
    double yPos = (((screenHeight - height) / (menuTitlesCount - 1)) * menuIndex) + 1 + headerHeight;
    id(my_display)
        .filled_rectangle(id(my_display).get_width() - scrollBarWidth, headerHeight, scrollBarWidth, screenHeight,
                          id(my_gray_dark_2));
    id(my_display)
        .filled_rectangle(id(my_display).get_width() - scrollBarWidth + scrollBarMargin, yPos,
                          scrollBarWidth - scrollBarMargin * 2, height - 1, id(color_accent_primary));
  }
}

void drawSwitch(bool switchState, int yPos) {
  int circleSize = id(small_font_size) / 2;
  int xPos = id(margin_size) + circleSize;
  int centerYPos = yPos + (id(medium_font_size) + id(margin_size)) / 2;
  id(my_display).circle(xPos, centerYPos, circleSize, primaryTextColor());
  if (switchState) {
    id(my_display).filled_circle(xPos, centerYPos, circleSize - 2, primaryTextColor());
  }
}

void drawLightSwitch(bool switchState, int yPos, Color lightColor, bool rowSelected) {
  int xPos = id(margin_size) - 2;
  if (rowSelected) {
    id(my_display)
        .filled_rectangle(xPos - 1, yPos, id(icon_size) + 1, id(medium_font_size) + id(margin_size), id(my_black));
  }
  yPos += (id(margin_size) / 4);
  if (switchState) {
    id(my_display).printf(xPos, yPos, &id(material_font_small), lightColor, "󰌵");
  } else {
    id(my_display).printf(xPos, yPos, &id(material_font_small), lightColor, "󰌶");
  }
}

void drawArrow(int yPos, int menuTitlesCount) {
  int xPos = id(my_display).get_width() - 8;
  if (menuTitlesCount > maxItems() + 1) {
    xPos = xPos - id(scroll_bar_width);
  }
  id(my_display)
      .line(xPos, yPos + 4, xPos + 3, yPos + (id(medium_font_size) + id(margin_size)) / 2, primaryTextColor());
  id(my_display)
      .line(xPos, yPos + (id(medium_font_size) + id(margin_size)) - 4, xPos + 3,
            yPos + (id(medium_font_size) + id(margin_size)) / 2, primaryTextColor());
}

void scrollMenuPosition() {
  int menuState = menuIndex;

  if (menuState - maxItems() > scrollTop) {
    scrollTop = menuState - maxItems();
    // menu down
  } else if (menuState - scrollTop < 0) {
    scrollTop = menuState;
    // menu up
  }
}

void drawTitleImage(int characterCount, int yPos, RemotePlayerState titleState, bool selected) {
  int adjustedYPos = yPos;
  int xPos = ((characterCount + 0.5) * (id(medium_font_size) * id(font_size_width_ratio))) + 4;
  auto color = selected ? primaryTextColor() : id(color_accent_primary);
  switch (titleState) {
    case PlayingRemotePlayerState:
      id(my_display).printf(xPos, yPos, &id(material_font_large), color, "󰐊");
      break;
    case PausedRemotePlayerState:
      id(my_display).printf(xPos, yPos, &id(material_font_large), color, "󰏤");
      break;
    case StoppedRemotePlayerState:
      id(my_display).printf(xPos, yPos, &id(material_font_large), color, "󰓛");
      break;
    case PowerOffRemotePlayerState:
      id(my_display).printf(xPos, yPos, &id(material_font_large), color, "󰽥");
      break;
    default:
      break;
  }
}

void drawGroupedBar(int yPos, bool extend) {
  int xPos = id(margin_size) * 2;
  int width = 8;
  int lineHeight = extend ? id(medium_font_size) + id(margin_size) : (id(medium_font_size) + id(margin_size)) / 2;
  id(my_display).line(xPos, yPos, xPos, yPos + lineHeight, primaryTextColor());
  id(my_display)
      .line(xPos, yPos + (id(medium_font_size) + id(margin_size)) / 2, xPos + width,
            yPos + (id(medium_font_size) + id(margin_size)) / 2, primaryTextColor());
}

void drawLightSliderRGBBar(int xPos, int yPos, int sliderHeight) {
  int rgbBarWidth = id(my_display).get_width() - 2 * id(slider_margin_size);
  id(my_display).filled_rectangle(xPos - 1, yPos - 1, rgbBarWidth + 2, sliderHeight + 2, id(my_gray_dark_2));
  for (int i = 0; i < rgbBarWidth; i++) {
    double hue = ((double) 360 / (double) rgbBarWidth) * ((double) i);
    Color dotColor = TextHelpers::hsvToRGB(hue, 1, 1);
    id(my_display).line(i + id(slider_margin_size), yPos, i + id(slider_margin_size), yPos + sliderHeight, dotColor);
  }
}

void drawLightSliderBar(int xPos, int yPos, int sliderHeight, SliderSelectionState sliderState,
                        std::shared_ptr<MenuTitleSlider> slider, bool drawRGB) {
  if (drawRGB) {
    drawLightSliderRGBBar(xPos, yPos, sliderHeight);
    return;
  }
  id(my_display)
      .filled_rectangle(xPos, yPos, id(my_display).get_width() - 2 * id(slider_margin_size), sliderHeight,
                        id(my_gray_dark_2));
  switch (sliderState) {
    case SliderSelectionStateActive:
      id(my_display).filled_rectangle(xPos, yPos, slider->sliderValue, sliderHeight, id(color_accent_primary));
      break;
    case SliderSelectionStateHover:
      id(my_display).filled_rectangle(xPos, yPos, slider->sliderValue, sliderHeight, id(my_white));
      break;
    case SliderSelectionStateNone:
      break;
  }
}

void drawLightSliderCircle(int xPos, int yPos, int sliderHeight, SliderSelectionState sliderState,
                           std::shared_ptr<MenuTitleSlider> slider, bool drawRGB) {
  int circleSize = 5;
  int dotPositionX = slider->sliderValue + id(slider_margin_size);
  int dotPositionY = yPos + (sliderHeight / 2);
  switch (sliderState) {
    case SliderSelectionStateActive:
      if (drawRGB) {
        auto rgbColor = TextHelpers::hsvToRGB(slider->displayValue, 1, 1);
        id(my_display).filled_circle(dotPositionX, dotPositionY, circleSize, rgbColor);
        id(my_display).circle(dotPositionX, dotPositionY, circleSize + 1, id(my_gray_dark_2));
      } else {
        id(my_display).filled_circle(dotPositionX, dotPositionY, circleSize - 1, id(color_accent_primary));
        id(my_display).circle(dotPositionX, dotPositionY, circleSize, id(my_white));
      }
      break;
    case SliderSelectionStateHover:
      id(my_display).filled_circle(dotPositionX, dotPositionY, circleSize, id(my_white));
      break;
    case SliderSelectionStateNone:
      id(my_display).filled_circle(dotPositionX, dotPositionY, circleSize, id(my_white));
      break;
  }
}

void drawLightSliderTitle(int xPos, int yPos, int sliderHeight, SliderSelectionState sliderState,
                          std::shared_ptr<MenuTitleSlider> slider, bool drawRGB) {
  std::string sliderTitle = "";
  if (drawRGB) {
    sliderTitle = slider->friendlyName;
  } else if (slider->displayValue > 0) {
    sliderTitle = slider->friendlyName + " - " + to_string(slider->displayValue) + slider->sliderUnit;
  } else {
    sliderTitle = slider->friendlyName;
  }
  switch (sliderState) {
    case SliderSelectionStateActive:
      if (drawRGB) {
        auto rgbColor = TextHelpers::hsvToRGB(slider->displayValue, 1, 1);
        id(my_display).printf(xPos, yPos + 1, &id(medium_font), rgbColor, sliderTitle.c_str());
      } else {
        id(my_display).printf(xPos, yPos + 1, &id(medium_font), id(color_accent_primary), sliderTitle.c_str());
      }
      break;
    case SliderSelectionStateHover:
    case SliderSelectionStateNone:
      id(my_display).printf(xPos, yPos + 1, &id(medium_font), id(my_white), sliderTitle.c_str());
      break;
  }
}

void drawLightSlider(int xPos, int yPos, SliderSelectionState sliderState, std::shared_ptr<MenuTitleSlider> slider,
                     bool drawRGB) {
  int sliderHeight = 3;
  if (sliderState == SliderSelectionStateHover) {
    id(my_display)
        .filled_rectangle(0, yPos, id(my_display).get_width(), (id(margin_size) + id(medium_font_size)) * 2,
                          id(color_accent_primary));
  }
  int sliderOffset = (int) (id(medium_font_size) + id(margin_size)) / 2;
  int oneRow = id(medium_font_size) + id(margin_size);
  drawLightSliderBar(xPos, yPos + sliderOffset + oneRow, sliderHeight, sliderState, slider, drawRGB);
  drawLightSliderCircle(xPos, yPos + sliderOffset + oneRow, sliderHeight, sliderState, slider, drawRGB);
  drawLightSliderTitle(xPos, yPos, sliderHeight, sliderState, slider, drawRGB);
}

void drawLeftTitleIcon(std::vector<std::shared_ptr<MenuTitleBase>> menuTitles,
                       std::shared_ptr<MenuTitleToggle> toggleTitle, int i, int menuState, int yPos) {
  switch (toggleTitle->leftIconState) {
    case NoMenuTitleLeftIcon:
      break;
    case OffMenuTitleLeftIcon:
    case OnMenuTitleLeftIcon:
      if (toggleTitle->titleType == LightMenuTitleType) {
        auto lightToggleTitle = std::static_pointer_cast<MenuTitleLight>(toggleTitle);
        if (lightToggleTitle != NULL) {
          auto lightColor =
              lightToggleTitle->leftIconState == OnMenuTitleLeftIcon ? lightToggleTitle->lightColor : id(my_white);
          drawLightSwitch(lightToggleTitle->leftIconState == OnMenuTitleLeftIcon, yPos, lightColor, menuState == i);
        }
      } else {
        drawSwitch(toggleTitle->leftIconState == OnMenuTitleLeftIcon, yPos);
      }
      break;
    case GroupedMenuTitleLeftIcon:
      bool extend = false;
      if (i < menuTitles.size() - 1) {
        auto nextToggleTitle = std::static_pointer_cast<MenuTitleToggle>(menuTitles[i + 1]);
        if (nextToggleTitle != NULL) {
          extend = nextToggleTitle->leftIconState == GroupedMenuTitleLeftIcon;
        }
      }
      drawGroupedBar(yPos, extend);
      break;
  }
}

void drawRightTitleIcon(std::vector<std::shared_ptr<MenuTitleBase>> menuTitles, int i, int menuState, int yPos) {
  switch (menuTitles[i]->rightIconState) {
    case NoMenuTitleRightIcon:
      break;
    case ArrowMenuTitleRightIcon:
      if (menuState == i) {
        drawArrow(yPos, menuTitles.size());
      }
      break;
  }
}

void drawMenu(std::vector<std::shared_ptr<MenuTitleBase>> menuTitles) {
  activeMenuTitleCount = menuTitles.size();
  if (menuTitles.size() == 0) {
    return;
  }
  scrollMenuPosition();
  int menuState = menuIndex;
  activeMenuTitle = menuTitles[menuIndex];
  int yPos = id(header_height);
  int sliderExtra = 0;  // fake menu items as the slider uses two rows
  for (int i = scrollTop; i < menuTitles.size(); i++) {
    if (i + sliderExtra > scrollTop + maxItems()) {
      break;
    }
    switch (menuTitles[i]->titleType) {
      case BaseMenuTitleType:
        drawTitle(menuState, i, menuTitles[i]->friendlyName, yPos, false);
        drawRightTitleIcon(menuTitles, i, menuState, yPos);
        yPos += id(medium_font_size) + id(margin_size);
        break;
      case LightMenuTitleType: {
        auto lightTitle = std::static_pointer_cast<MenuTitleLight>(menuTitles[i]);
        if (lightTitle != NULL) {
          drawTitle(menuState, i, menuTitles[i]->friendlyName, yPos, lightTitle->indentLine());
          drawLeftTitleIcon(menuTitles, lightTitle, i, menuState, yPos);
          drawRightTitleIcon(menuTitles, i, menuState, yPos);
          yPos += id(medium_font_size) + id(margin_size);
        }
        break;
      }
      case ToggleMenuTitleType: {
        auto toggleTitle = std::static_pointer_cast<MenuTitleToggle>(menuTitles[i]);
        if (toggleTitle != NULL) {
          drawTitle(menuState, i, menuTitles[i]->friendlyName, yPos, toggleTitle->indentLine());
          drawLeftTitleIcon(menuTitles, toggleTitle, i, menuState, yPos);
          drawRightTitleIcon(menuTitles, i, menuState, yPos);
          yPos += id(medium_font_size) + id(margin_size);
        }
        break;
      }
      case SliderMenuTitleType: {
        bool lightDetailSelected = lightGroup->lightDetailSelected;
        auto item = std::static_pointer_cast<MenuTitleSlider>(menuTitles[i]);
        SliderSelectionState sliderState = menuState == i && lightDetailSelected ? SliderSelectionStateActive
                                           : menuState == i                      ? SliderSelectionStateHover
                                                                                 : SliderSelectionStateNone;
        drawLightSlider(id(slider_margin_size), yPos, sliderState, item, i == 2);
        sliderExtra += 0;

        yPos += (id(medium_font_size) + id(margin_size)) * 2;
        break;
      }
      case PlayerMenuTitleType: {
        auto playerTitle = std::static_pointer_cast<MenuTitlePlayer>(menuTitles[i]);
        if (playerTitle != NULL) {
          drawTitle(menuState, i, menuTitles[i]->friendlyName, yPos, playerTitle->indentLine());
          int length = playerTitle->friendlyName.length() + (playerTitle->indentLine() ? 2 : 0);
          drawTitleImage(length, yPos, playerTitle->playerState, menuState == i);
          drawLeftTitleIcon(menuTitles, playerTitle, i, menuState, yPos);
          drawRightTitleIcon(menuTitles, i, menuState, yPos);
          yPos += id(medium_font_size) + id(margin_size);
        }
        break;
      }
    }
  }
  drawScrollBar(menuTitles.size(), id(header_height));
}

std::vector<std::shared_ptr<MenuTitleBase>> menuTypesToTitles(std::vector<MenuStates> menu) {
  std::vector<std::shared_ptr<MenuTitleBase>> out;
  for (auto &menuItem : menu) {
    out.push_back(menuTitleForType(menuItem));
  }
  return out;
}

std::vector<std::shared_ptr<MenuTitleBase>> activeMenu() {
  int x = menuIndex;
  switch (activeMenuState) {
    case rootMenu:
      return menuTypesToTitles(rootMenuTitles(speakerGroup != NULL, sceneGroup != NULL, sensorGroup != NULL,
                                              lightGroup != NULL, switchGroup != NULL));
    case sourcesMenu: {
      auto sourceTitles = speakerGroup->activePlayerSourceMenu();
      return {sourceTitles.begin(), sourceTitles.end()};
    }
    case mediaPlayersMenu: {
      auto mediaPlayersTitles = speakerGroup->mediaPlayersTitleString();
      return {mediaPlayersTitles.begin(), mediaPlayersTitles.end()};
    }
    case scenesMenu:
      return sceneGroup->sceneTitleStrings();
    case sensorsMenu:
      return sensorGroup->sensorTitles();
    default:
      ESP_LOGW("WARNING", "menu is bad  %d", x);
      std::vector<std::shared_ptr<MenuTitleBase>> out;
      return out;
  }
}

void topMenu() {
  menuIndex = 0;
  if (speakerGroup != NULL)
    speakerGroup->newSpeakerGroupParent = NULL;
  optionMenu = noOptionMenu;
  activeMenuState = MenuStates::rootMenu;
}

void idleMenu(bool force) {
  if (!charging || force) {
    lightGroup->clearActiveLight();
    menuIndex = 0;
    if (speakerGroup != NULL)
      speakerGroup->newSpeakerGroupParent = NULL;
    optionMenu = noOptionMenu;
    activeMenuState = MenuStates::nowPlayingMenu;
    if (force) {
      displayUpdate.updateDisplay(true);
    }
  }
}

void activeTick() {
  switch (activeMenuState) {
  case sensorsMenu:
    break;
  default:
    return;
  }
  if ((charging || idleTime < 15) && idleTime > 1 && (marqueeText || charging)) {
    if (marqueeText) {
      marqueePosition += 1.5;
    } else if (marqueePosition != 0) {
      marqueePosition = 0;
    }
    if (marqueePosition >= 0) {
      displayUpdate.updateDisplay(true);
    }
  } else if (marqueePosition != 0) {
    marqueePosition = 0;
    if (marqueeText) {
      displayUpdate.updateDisplay(true);
    }
  }
}

void idleTick() {
  if (activeMenuState == bootMenu) {
    if (idleTime == id(display_timeout) && !charging) {
      ESP_LOGD("idle", "turning off display");
      id(backlight).turn_off();
    }
    idleTime++;
    return;
  }
  if (idleTime == 3) {
    optionMenu = noOptionMenu;
    displayUpdate.updateDisplay(true);
  } else if (idleTime == id(display_timeout)) {
    if (speakerGroup != NULL && speakerGroup->playerSearchFinished) {
      idleMenu(false);
      displayUpdate.updateDisplay(false);
    }
    if (!charging) {
      ESP_LOGD("idle", "turning off display");
      id(backlight).turn_off();
    }
    idleTime++;
    return;
  } else if (idleTime == 180 && charging) {
    idleMenu(true);
    idleTime++;
    return;
  } else if (idleTime > id(sleep_after)) {
    if (!charging) {
      ESP_LOGI("idle", "night night");
      id(sleep_toggle).turn_on();
      return;
    }
  }
  if (speakerGroup != NULL) {
    bool updatedMediaPositions = speakerGroup->updateMediaPosition();
    if (updatedMediaPositions) {
      switch (activeMenuState) {
        case nowPlayingMenu:
          if (idleTime < 16 || charging) {
            displayUpdate.updateDisplay(true);
          }
          break;
        default:
          break;
      }
    }
  }
  idleTime++;
}

void drawTVOptionMenu() {
  id(my_display)
      .circle(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.45 + 24, 48, id(my_gray));
  id(my_display)
      .printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.15 + 16, &id(small_font),
              primaryTextColor(), TextAlign::TOP_CENTER, "Remote Menu");
  id(my_display)
      .printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.75 + 16, &id(small_font),
              primaryTextColor(), TextAlign::TOP_CENTER, "Pause");
  id(my_display)
      .printf(id(my_display).get_width() * 0.2, (id(my_display).get_height() - 16) * 0.45 + 16, &id(small_font),
              primaryTextColor(), TextAlign::TOP_CENTER, "Back");
  id(my_display)
      .printf(id(my_display).get_width() * 0.8, (id(my_display).get_height() - 16) * 0.45 + 16, &id(small_font),
              primaryTextColor(), TextAlign::TOP_CENTER, "Home");
  id(my_display)
      .printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.45 + 16, &id(small_font),
              primaryTextColor(), TextAlign::TOP_CENTER, "TV Power");
}

void drawSpeakerOptionMenu() {
  id(my_display)
      .circle(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.45 + 24, 48, id(my_gray));
  id(my_display)
      .printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.15 + 16, &id(small_font),
              primaryTextColor(), TextAlign::TOP_CENTER, speakerGroup->shuffleString().c_str());
  id(my_display)
      .printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.75 + 16, &id(small_font),
              primaryTextColor(), TextAlign::TOP_CENTER, "Group");
  id(my_display)
      .printf(id(my_display).get_width() * 0.8, (id(my_display).get_height() - 16) * 0.45 + 16, &id(small_font),
              primaryTextColor(), TextAlign::TOP_CENTER, speakerGroup->muteString().c_str());
}

void drawVolumeOptionMenu() {
  int barMargin = 1;
  int barHeight = id(small_font_size);
  int iconMargin = id(small_font_size) * id(font_size_width_ratio) * 3;
  int totalBarWidth = id(my_display).get_width() - iconMargin * 2;
  int barWidth = (totalBarWidth - 4) * (speakerGroup->getVolumeLevel() / 100);
  int yPos = id(my_display).get_height() - barHeight - id(bottom_bar_margin);

  id(my_display)
      .printf(iconMargin / 2 - id(icon_size) / 2, yPos + 1, &id(material_font_small), id(color_accent_primary), "󰕿");
  id(my_display)
      .printf(id(my_display).get_width() - iconMargin / 2 - id(icon_size) / 2, yPos + 1, &id(material_font_small),
              id(color_accent_primary), "󰕾");

  id(my_display).rectangle(iconMargin, yPos, totalBarWidth, barHeight, id(color_accent_primary));
  id(my_display)
      .filled_rectangle(iconMargin + barMargin * 2, yPos + barMargin * 2, barWidth, barHeight - 2 - barMargin * 2,
                        id(color_accent_primary));
}

std::string secondsToString(int seconds) {
  if (seconds == -1) {
    return "00:00";
  }
  return seconds % 60 < 10 ? "0" + to_string(seconds % 60) : to_string(seconds % 60);
}

void drawMediaDuration() {
  if (speakerGroup->activePlayer->playerType != TVRemotePlayerType) {
    SonosSpeakerComponent *activeSpeaker = static_cast<SonosSpeakerComponent *>(speakerGroup->activePlayer);
    int mediaDuration = activeSpeaker->mediaDuration;
    int mediaPosition = activeSpeaker->mediaPosition;
    if (mediaDuration <= 0 && mediaPosition <= 0) {
      return;
    }
    int barMargin = 1;
    int barHeight = id(small_font_size);
    int textWidth = (id(small_font_size) * id(font_size_width_ratio) * 5) + id(margin_size) / 2;
    int totalBarWidth = id(my_display).get_width() - textWidth * 2;
    int barWidth = 0;
    if (mediaDuration > 0 && mediaPosition > 0) {
      barWidth = (totalBarWidth - 4) * (static_cast<double>(mediaPosition) / static_cast<double>(mediaDuration));
    }

    int yPos = id(my_display).get_height() - barHeight - id(bottom_bar_margin);
    id(my_display).rectangle(textWidth, yPos, totalBarWidth, barHeight, primaryTextColor());
    id(my_display)
        .filled_rectangle(textWidth + barMargin * 2, yPos + barMargin * 2, barWidth, barHeight - 2 - barMargin * 2,
                          primaryTextColor());

    int textYPos = yPos - id(small_font_size) * 0.1;
    std::string mediaDurationSeconds = secondsToString(mediaDuration);
    std::string mediaPositionSeconds = secondsToString(mediaPosition);
    id(my_display)
        .printf(id(margin_size), textYPos, &id(small_font), primaryTextColor(), TextAlign::TOP_LEFT, "%d:%s",
                mediaPosition / 60, mediaPositionSeconds.c_str());
    id(my_display)
        .printf(id(my_display).get_width() - id(margin_size), textYPos, &id(small_font), primaryTextColor(),
                TextAlign::TOP_RIGHT, "%d:%s", mediaDuration / 60, mediaDurationSeconds.c_str());
  }
}

std::vector<std::string> getWrappedTitles(int xPos, int fontSize, TextAlign alignment, std::string text) {
  std::vector<std::string> output;
  if (text.size() == 0) {
    return output;
  }
  std::string wrappedTitles = textWrap(text, getCharacterLimit(xPos, fontSize, alignment));
  TextHelpers::tokenize(wrappedTitles, "\n", output);
  return output;
}

int drawTextWrapped(int xPos, int yPos, int fontSize, Font *font, Color color, TextAlign alignment,
                    std::vector<std::string> wrappedTitles, int maxLines) {
  int characterLimit = getCharacterLimit(xPos, fontSize, alignment);
  int max = maxLines != 0 && maxLines < wrappedTitles.size() ? maxLines : wrappedTitles.size();
  for (int i = 0; i < max; i++) {
    if (maxLines != 0 && maxLines - 1 == i && i == max - 1) {
      std::string title = wrappedTitles[i];
      if (wrappedTitles[i].size() > characterLimit - 2) {
        title.erase(title.length() - 3);
      }
      title = title + "...";
      id(my_display).printf(xPos, yPos + (i * fontSize), font, color, alignment, title.c_str());
      break;
    } else {
      id(my_display).printf(xPos, yPos + (i * fontSize), font, color, alignment, wrappedTitles[i].c_str());
    }
  }
  return yPos + (max * fontSize);
}

bool drawOptionMenuAndStop() {
  switch (optionMenu) {
    case tvOptionMenu:
      drawTVOptionMenu();
      return true;
    case speakerOptionMenu:
      drawSpeakerOptionMenu();
      return true;
    case volumeOptionMenu:
      // called later so it's over text
      return false;
    case noOptionMenu:
      return false;
    case playingNewSourceMenu:
      id(my_display)
          .printf(id(my_display).get_width() / 2, id(header_height) + id(margin_size), &id(medium_font),
                  primaryTextColor(), TextAlign::TOP_CENTER, "Playing...");
      auto playingNewSourceWrappedText = getWrappedTitles(id(my_display).get_width() / 2, id(large_font_size),
                                                          TextAlign::TOP_CENTER, speakerGroup->playingNewSourceText);
      drawTextWrapped(id(my_display).get_width() / 2, id(header_height) + id(margin_size) * 2 + id(medium_font_size),
                      24, &id(large_font), primaryTextColor(), TextAlign::TOP_CENTER, playingNewSourceWrappedText, 0);
      return true;
  }
  return true;
}

std::string stringForNowPlayingMenuState(NowPlayingMenuState state) {
  switch (state) {
    case pauseNowPlayingMenuState:
      return speakerGroup->playTitleString();
    case volumeUpNowPlayingMenuState:
      return "Vol Up";
    case volumeDownNowPlayingMenuState:
      return "Vol Down";
    case nextNowPlayingMenuState:
      return "Next";
    case menuNowPlayingMenuState:
      return "Menu";
    case backNowPlayingMenuState:
      return "Back";
    case TVPowerNowPlayingMenuState:
      return "Power";
    case homeNowPlayingMenuState:
      return "TV Home";
    case groupNowPlayingMenuState:
      return "Group";
    case shuffleNowPlayingMenuState:
      if (speakerGroup->mediaShuffling()) {
        return "Shfl on";
      } else {
        return "Shfl off";
      }
  }
  return "";
}

std::vector<NowPlayingMenuState> getNowPlayingMenuStates() {
  if (speakerGroup->activePlayer->playerType == TVRemotePlayerType) {
    return TVNowPlayingMenuStates();
  }
  return speakerNowPlayingMenuStates();
}

void drawNowPlayingSelectMenu() {
  auto menuTitles = getNowPlayingMenuStates();
  activeMenuTitleCount = menuTitles.size();
  int yPos = id(my_display).get_height() - id(margin_size) - id(large_font_size);
  if (activeMenuTitleCount < 1) {
    return;
  }
  id(my_display)
      .printf(id(my_display).get_width() * 0.5, yPos, &id(large_font), primaryTextColor(), TextAlign::TOP_CENTER,
              stringForNowPlayingMenuState(menuTitles[menuIndex]).c_str());
  if (menuIndex + 1 < activeMenuTitleCount) {
    id(my_display)
        .printf(id(my_display).get_width() * 0.85, yPos, &id(small_font), primaryTextColor(), TextAlign::TOP_CENTER,
                stringForNowPlayingMenuState(menuTitles[menuIndex + 1]).c_str());
  } else {
    id(my_display)
        .printf(id(my_display).get_width() * 0.85, yPos, &id(small_font), primaryTextColor(), TextAlign::TOP_CENTER,
                stringForNowPlayingMenuState(menuTitles[0]).c_str());
  }
  if (menuIndex - 1 >= 0) {
    id(my_display)
        .printf(id(my_display).get_width() * 0.15, yPos, &id(small_font), primaryTextColor(), TextAlign::TOP_CENTER,
                stringForNowPlayingMenuState(menuTitles[menuIndex - 1]).c_str());
  } else {
    id(my_display)
        .printf(id(my_display).get_width() * 0.15, yPos, &id(small_font), primaryTextColor(), TextAlign::TOP_CENTER,
                stringForNowPlayingMenuState(menuTitles[activeMenuTitleCount - 1]).c_str());
  }
}

void drawNowPlaying() {
  if (drawOptionMenuAndStop()) {
    return;
  }
  if (id(draw_now_playing_menu)) {
    drawNowPlayingSelectMenu();
  }
  int yPos = id(header_height) + id(margin_size) / 4;
  if (speakerGroup->activePlayer->playerState == PowerOffRemotePlayerState) {
    id(my_display)
        .printf(id(my_display).get_width() / 2, yPos, &id(large_font), primaryTextColor(), TextAlign::TOP_CENTER,
                "Power Off");
    return;
  }
  std::string nowPlayingText = "Now Playing,";

  if (speakerGroup->activePlayer->playerType != TVRemotePlayerType) {
    SonosSpeakerComponent *activeSpeaker = static_cast<SonosSpeakerComponent *>(speakerGroup->activePlayer);
    if (activeSpeaker->mediaPlaylist != activeSpeaker->mediaTitle) {
      nowPlayingText += " " + activeSpeaker->mediaPlaylist;
    } else if (activeSpeaker->mediaAlbumName != activeSpeaker->mediaTitle) {
      nowPlayingText += " " + activeSpeaker->mediaAlbumName;
    }
  }
  int xPos = id(my_display).get_width() / 2;
  auto nowPlayingWrappedText =
      getWrappedTitles(id(margin_size), id(medium_font_size), TextAlign::TOP_LEFT, nowPlayingText);
  auto mediaArtistWrappedText =
      getWrappedTitles(xPos, id(large_font_size), TextAlign::TOP_CENTER, speakerGroup->mediaTitleString());
  auto mediaTitleWrappedText =
      getWrappedTitles(xPos, id(medium_font_size), TextAlign::TOP_CENTER, speakerGroup->mediaSubtitleString());
  int lineCount = nowPlayingWrappedText.size() + mediaArtistWrappedText.size() + mediaTitleWrappedText.size();
  int maxLines = 0;
  if (lineCount > id(now_playing_max_lines)) {
    maxLines = 1;
    if (nowPlayingWrappedText.size() > 1) {
      lineCount = 1 + mediaArtistWrappedText.size() + mediaTitleWrappedText.size();
    }
  }
  yPos = drawTextWrapped(id(margin_size), yPos, id(medium_font_size), &id(medium_font), primaryTextColor(),
                         TextAlign::TOP_LEFT, nowPlayingWrappedText, maxLines);
  if (mediaArtistWrappedText.size() == 0 && mediaTitleWrappedText.size() == 0) {
    id(my_display)
        .printf(id(my_display).get_width() / 2, yPos, &id(large_font), primaryTextColor(), TextAlign::TOP_CENTER,
                "Nothing!");
    return;
  }
  if (lineCount > id(now_playing_max_lines)) {
    maxLines = 2;
  } else {
    maxLines = 0;
  }
  yPos = yPos + id(margin_size) / 2;
  if (mediaArtistWrappedText.size() > 0) {
    yPos = drawTextWrapped(xPos, yPos, id(large_font_size), &id(large_font), primaryTextColor(), TextAlign::TOP_CENTER,
                           mediaArtistWrappedText, maxLines);
  }
  if (mediaTitleWrappedText.size() > 0) {
    yPos = yPos + id(margin_size);
    drawTextWrapped(id(my_display).get_width() / 2, yPos, id(medium_font_size), &id(medium_font), primaryTextColor(),
                    TextAlign::TOP_CENTER, mediaTitleWrappedText, maxLines);
  }
  if (optionMenu == volumeOptionMenu) {
    drawVolumeOptionMenu();
  } else {
    drawMediaDuration();
  }
}

int autoClearState = 0;

void drawBootSequenceTitleRainbow(int xPos, int yPos) {
  std::string bootTitle = "homeThing";
  int loopLimit = bootTitle.length();
  int delayTime = 6;
  int animationStartTime = delayTime;
  int animationLength = animationStartTime + loopLimit + delayTime * 2;
  if (animationTick > animationStartTime && animationTick < animationLength) {
    int currentAnimationTick = animationTick - animationStartTime;
    int activeCharacter = currentAnimationTick;
    int maxCharacters = loopLimit;
    std::vector<Color> colors = {id(my_black), id(my_yellow), id(my_red), id(my_pink), id(my_green), id(my_blue)};
    int textWidth = maxCharacters * id(large_font_size) * id(font_size_width_ratio);
    for (int i = 0; i < maxCharacters; i++) {
      int colorIndex = i <= currentAnimationTick ? currentAnimationTick - i : 0;
      if (colorIndex > 0) {
        auto color = colors.size() > colorIndex ? colors[colorIndex] : id(color_accent_primary);
        int characterXPos = xPos - textWidth / 2 + (i * id(large_font_size) * id(font_size_width_ratio));
        id(my_display)
            .printf(characterXPos, yPos, &id(extra_large_font), color, TextAlign::TOP_LEFT, "%c",
                    bootTitle[i]);
      }
    }
  } else if (animationTick >= animationLength) {
    id(my_display)
        .printf(xPos, yPos, &id(extra_large_font), id(color_accent_primary), TextAlign::TOP_CENTER,
                "wifi connecting...");
  }
}

void drawBootSequenceLogo(int xPos, int imageYPos) {
  int animationLength = 6;
  int delayTime = 2;
  int totalDuration = delayTime + animationLength;
  if (animationTick > delayTime && animationTick < totalDuration) {
    int colorValue = (float(animationTick - delayTime) / float(animationLength)) * 255;
    auto color = Color(colorValue, colorValue, colorValue);
    id(my_display).printf(xPos, imageYPos, &id(home_thing_logo), color, TextAlign::TOP_CENTER, "");
  } else if (animationTick >= totalDuration) {
    id(my_display).printf(xPos, imageYPos, &id(home_thing_logo), id(my_white), TextAlign::TOP_CENTER, "");
  }
}

void drawBootSequenceHeader() {
  int animationLength = 8;
  int delayTime = 20;
  int totalDuration = delayTime + animationLength;
  int maxValue = id(header_height);
  if (animationTick > delayTime && animationTick < totalDuration) {
    int yPosOffset = maxValue - (float(animationTick - delayTime) / float(animationLength)) * maxValue;
    drawHeader(yPosOffset);
  } else if (animationTick >= totalDuration) {
    drawHeader(0);
  }
}

float bootSequenceLoadingProgress() {
  if (id(homeassistant_api_id).is_connected()) {
    if (speakerGroup != NULL) {
      int totalPlayers = speakerGroup->totalPlayers();
      int loadedPlayers = speakerGroup->loadedPlayers;
      float progress = 0.8 * (float(loadedPlayers) / float(totalPlayers));
      return 0.25 + progress;
    }
    return 0.2;
  } else if (id(wifi_id).is_connected()) {
    return 0.1;
  } else {
    return 0;
  }
}

void drawBootSequenceLoadingBar(int yPosOffset, float progress) {
  int barMargin = 1;
  int barHeight = id(small_font_size);
  int iconMargin = id(small_font_size) * id(font_size_width_ratio) * 3;
  int totalBarWidth = id(my_display).get_width() - iconMargin * 2;
  int barWidth = (totalBarWidth - 4) * progress;
  int yPos = id(my_display).get_height() - barHeight - id(bottom_bar_margin) + yPosOffset;

  id(my_display).rectangle(iconMargin, yPos, totalBarWidth, barHeight, id(color_accent_primary));
  id(my_display)
      .filled_rectangle(iconMargin + barMargin * 2, yPos + barMargin * 2, barWidth, barHeight - 2 - barMargin * 2,
                        id(color_accent_primary));
}

void drawBootSequenceLoadingBarAnimation() {
  int animationLength = 8;
  int delayTime = 20;
  int totalDuration = delayTime + animationLength;
  int maxValue = id(bottom_bar_margin);

  if (animationTick > delayTime && animationTick < totalDuration) {
    int yPosOffset = maxValue - (float(animationTick - delayTime) / float(animationLength)) * maxValue;
    drawBootSequenceLoadingBar(yPosOffset, bootSequenceLoadingProgress());
  } else if (animationTick >= totalDuration) {
    drawBootSequenceLoadingBar(0, bootSequenceLoadingProgress());
  }
}

void drawBootSequenceTitle(int xPos, int imageYPos) {
  int yPos = imageYPos + id(boot_logo_size) + id(margin_size);
  if (id(homeassistant_api_id).is_connected()) {
    if (speakerGroup != NULL) {
      int totalPlayers = speakerGroup->totalPlayers();
      int loadedPlayers = speakerGroup->loadedPlayers;
      id(my_display)
          .printf(xPos, yPos, &id(extra_large_font), id(color_accent_primary), TextAlign::TOP_CENTER,
                  "%d/%d players loaded", loadedPlayers, totalPlayers);
    } else {
      id(my_display)
          .printf(xPos, yPos, &id(extra_large_font), id(color_accent_primary), TextAlign::TOP_CENTER,
                  "api connected!");
    }
  } else if (id(wifi_id).is_connected()) {
    id(my_display)
        .printf(xPos, yPos, &id(extra_large_font), id(color_accent_primary), TextAlign::TOP_CENTER,
                "api connecting...");
  } else {
    drawBootSequenceTitleRainbow(xPos, yPos);
  }
}

void drawBootSequence() {
  speakerGroup->findActivePlayer();

  int imageYPos = id(header_height) + id(margin_size) * 2;
  int xPos = id(my_display).get_width() / 2;

  drawBootSequenceHeader();
  drawBootSequenceLogo(xPos, imageYPos);
  drawBootSequenceTitle(xPos, imageYPos);
  drawBootSequenceLoadingBarAnimation();
  animationTick++;
  menuDrawing = false;
}

void drawMenu() {
  if (idleTime > 16 && !charging) {
    menuDrawing = false;
    return;
  }
  if (!id(dark_mode) && activeMenuState != bootMenu) {
    id(my_display).fill(id(my_white));
  }
  if (speakerGroup != NULL && speakerGroup->playerSearchFinished == false) {
    drawBootSequence();
    return;
  } else if (activeMenuState == bootMenu) {
    activeMenuState = rootMenu;
  }
  if (autoClearState != 0) {
    id(my_display).set_auto_clear(true);
    autoClearState = 0;
  }
  switch (activeMenuState) {
    case nowPlayingMenu:
      drawNowPlaying();
      break;
    case lightsMenu:
      drawMenu(lightGroup->lightTitleSwitches());
      break;
    case lightsDetailMenu:
      if (lightGroup->getActiveLight() != NULL) {
        drawMenu(lightGroup->getActiveLight()->lightTitleItems());
      }
      break;
    case switchesMenu:
      drawMenu(switchGroup->switchTitleSwitches());
      break;
    case groupMenu: {
      if (speakerGroup->newSpeakerGroupParent != NULL) {
        auto playerSwitches = speakerGroup->groupTitleSwitches();
        drawMenu({playerSwitches.begin(), playerSwitches.end()});
        break;
      }
      auto playerStrings = speakerGroup->groupTitleString();
      drawMenu({playerStrings.begin(), playerStrings.end()});
      break;
    }
    default:
      drawMenu(activeMenu());
      break;
  }
  drawHeader(0);
  menuDrawing = false;
}

void selectMediaPlayers() {
  for (auto &speaker : speakerGroup->speakers) {
    if (speaker->entityId == activeMenuTitle->entityId) {
      speakerGroup->activePlayer = speaker;
      topMenu();
      return;
    }
  }
  for (auto &tv : speakerGroup->tvs) {
    if (tv->entityId == activeMenuTitle->entityId) {
      speakerGroup->activePlayer = tv;
      topMenu();
      return;
    }
  }
}

bool selectRootMenu() {
  MenuStates currentMenu = rootMenuTitles(speakerGroup != NULL, sceneGroup != NULL, sensorGroup != NULL,
                                          lightGroup != NULL, switchGroup != NULL)[menuIndex];
  switch (currentMenu) {
    case sourcesMenu:
      activeMenuState = sourcesMenu;
      break;
    case nowPlayingMenu:
      activeMenuState = nowPlayingMenu;
      break;
    case mediaPlayersMenu:
      activeMenuState = mediaPlayersMenu;
      break;
    case lightsMenu:
      activeMenuState = lightsMenu;
      break;
    case switchesMenu:
      activeMenuState = switchesMenu;
      break;
    case scenesMenu:
      activeMenuState = scenesMenu;
      break;
    case backlightMenu:
      topMenu();
      id(backlight).turn_off();
      return false;
    case sleepMenu:
      id(sleep_toggle).turn_on();
      return false;
    case sensorsMenu:
      activeMenuState = sensorsMenu;
      break;
    case lightsDetailMenu:
    case groupMenu:
    case rootMenu:
    case bootMenu:
      ESP_LOGD("WARNING", "menu is bad  %d", menuIndex);
      return false;
  }
  menuIndex = 0;
  return true;
}

bool selectMenuHold() {
  int menuIndexForSource = menuIndex;
  switch (activeMenuState) {
    case lightsMenu: {
      auto selectedLight = lightGroup->lights[menuIndexForSource];
      if (selectedLight == NULL) {
        break;
      }
      if (selectedLight->supportsBrightness()) {
        // save light and go to light detail
        lightGroup->selectLightAtIndex(menuIndexForSource);
        menuIndex = 0;
        activeMenuState = lightsDetailMenu;
      }
      return true;
    }
    default:
      break;
  }
  return false;
}

bool selectMenu() {
  int menuIndexForSource = menuIndex;
  switch (activeMenuState) {
    case rootMenu:
      return selectRootMenu();
    case nowPlayingMenu:
      activeMenuState = MenuStates::nowPlayingMenu;
      break;
    case sourcesMenu: {
      auto sourceTitleState = std::static_pointer_cast<MenuTitleSource>(activeMenuTitle);
      idleMenu(true);
      speakerGroup->playSource(*sourceTitleState);
      optionMenu = playingNewSourceMenu;
      displayUpdate.updateDisplay(true);
      break;
    }
    case groupMenu: {
      auto playerTitleState = std::static_pointer_cast<MenuTitlePlayer>(activeMenuTitle);
      speakerGroup->selectGroup(*playerTitleState);
      break;
    }
    case lightsMenu: {
      auto selectedLight = lightGroup->lights[menuIndexForSource];
      if (selectedLight == NULL) {
        break;
      }
      selectedLight->toggleLight();
      return true;
    }
    case lightsDetailMenu:
      lightGroup->lightDetailSelected = true;
    case mediaPlayersMenu:
      selectMediaPlayers();
      break;
    case scenesMenu:
      if (sceneGroup->selectScene(menuIndexForSource)) {
        topMenu();
      }
      break;
    case switchesMenu:
      if (switchGroup->selectSwitch(menuIndexForSource)) {
        topMenu();
      }
      break;
    default:
      ESP_LOGW("WARNING", "menu state is bad but its an enum");
      return false;
  }
  return true;
}

bool buttonPressWakeUpDisplay() {
  if (idleTime != -1) {
    idleTime = 0;
  }
  if (speakerGroup != NULL && !speakerGroup->playerSearchFinished) {
    speakerGroup->findActivePlayer();
  }
  if (!id(backlight).state) {
    id(backlight).turn_on();
    displayUpdate.updateDisplay(false);
    return true;
  }
  return false;
}
