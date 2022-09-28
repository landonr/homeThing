#include "esphome.h"
#include "esphomeRemotePlayer.h"
#include "esphomeRemoteService.h"
#include "esphomeRemoteLight.h"
#include "esphomeRemoteSensor.h"
#include "MenuGlobals.h"

#ifndef ESPHOMEREMOTE
#define ESPHOMEREMOTE

bool menuDrawing = false;

class DisplayUpdateImpl: public DisplayUpdateInterface {
  public: virtual void updateDisplay(bool force) {
    if(menuDrawing && id(backlight).state) {
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
};

auto displayUpdate = DisplayUpdateImpl();
auto * sceneGroup = new SceneGroupComponent();
auto * sensorGroup = new SensorGroupComponent();
auto * speakerGroup = new SonosSpeakerGroupComponent(displayUpdate);
auto * lightGroup = new LightGroupComponent(displayUpdate);
MenuTitle activeMenuTitle = MenuTitle("", "", NoMenuTitleState);
double marqueePosition = 0;
bool marqueeText = false;

void resetMarquee() {
  marqueePosition = 0;
}

std::string textWrap(std::string text, unsigned per_line) {
  unsigned line_begin = 0;
  while (line_begin < text.size()) {
    const unsigned ideal_end = line_begin + per_line;
    unsigned line_end = ideal_end <= text.size() ? ideal_end : text.size() - 1;

    if (line_end == text.size() - 1)
      ++line_end;
    else if (std::isspace(text[line_end])) {
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
      } else
        text.insert(line_end++, 1, '\n');
    }

    line_begin = line_end;
  }

  return text;
}

int getCharacterLimit(int xPos, int fontSize, TextAlign alignment) {
  int characterLimit = (id(my_display).get_width() - xPos) / (fontSize * id(font_size_width_ratio)) - 1;
  if(xPos == id(my_display).get_width() / 2 && alignment == TextAlign::TOP_CENTER) {
    characterLimit = id(my_display).get_width() / (fontSize * id(font_size_width_ratio)) - 1;
  }
  return characterLimit;
}

MenuTitle menuTitleForType(MenuStates stringType) {
  switch (stringType) {
  case nowPlayingMenu:
    return MenuTitle("Now Playing", "", ArrowMenuTitleState);
  case sourcesMenu:
    return MenuTitle("Sources", "", ArrowMenuTitleState);
  case backlightMenu:
    return MenuTitle("Backlight", "", NoMenuTitleState);
  case sleepMenu:
    return MenuTitle("Sleep", "", NoMenuTitleState);
  case mediaPlayersMenu:
    return MenuTitle("Media Players", "", ArrowMenuTitleState);
  case lightsMenu:
    return MenuTitle("Lights", "", ArrowMenuTitleState);
  case scenesMenu:
    return MenuTitle("Scenes and Actions", "", ArrowMenuTitleState);
  case rootMenu:
    return MenuTitle("Home", "", NoMenuTitleState);
  case groupMenu:
    return MenuTitle("Speaker Group", "", ArrowMenuTitleState);
  case sensorsMenu:
    return MenuTitle("Sensors", "", ArrowMenuTitleState);
  case bootMenu:
    return MenuTitle("Boot", "", NoMenuTitleState);
  }
  return MenuTitle("", "", NoMenuTitleState);
}

void goToScreenFromString(std::string screenName) {
  if(screenName == "nowPlaying") {
    activeMenuState = nowPlayingMenu;
  } else if (screenName == "sensors") {
    activeMenuState = sensorsMenu;
  }
  menuIndex = 0;
  displayUpdate.updateDisplay(true);
}

int getTextWidth(int fontSize, int characterCount) {
  return (fontSize * id(font_size_width_ratio) * characterCount);
}

int getHeaderTextYPos() {
  return ((id(header_height) - id(small_font_size) * 1.2) / 2);
}

int drawPlayPauseIcon(int oldXPos) {
  int yPos = id(header_height) / 2 - id(icon_size) / 2;
  int xPos = oldXPos;
  switch(speakerGroup->activePlayer->menuTitlePlayerState()) {
    case PlayingMenuTitleState:
      id(my_display).image(xPos, yPos, & id(image_play));
      break;
    case PausedMenuTitleState:
      id(my_display).image(xPos, yPos, & id(image_pause));
      break;
    case StoppedMenuTitleState:
      id(my_display).image(xPos, yPos, & id(image_stop));
      break;
    case PowerOffMenuTitleState:
      id(my_display).image(xPos, yPos, & id(image_sleep));
      break;
    default:
      return oldXPos;
  }
  return xPos + id(icon_size) + id(margin_size) / 2;
}

void drawHeaderTitleWithString(std::string title, int xPos) {
  int yPos = getHeaderTextYPos();
  id(my_display).printf(xPos, yPos, & id(small_font), id(my_white), title.c_str());
}

void drawHeaderTitle() {
  int xPos = 2;
  switch (activeMenuState) {
  case rootMenu:
  case backlightMenu:
  case sleepMenu:
  case nowPlayingMenu:
    xPos = drawPlayPauseIcon(xPos);
    drawHeaderTitleWithString(speakerGroup->headerMediaPlayerTitleString(), xPos);
    break;
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
  case sensorsMenu:
    drawHeaderTitleWithString("Sensors", xPos);
    break;
  case bootMenu:
    drawHeaderTitleWithString("Boot", xPos);
    break;
  }
}

int drawVolumeLevel(int oldXPos) {
  if(!id(draw_volume_level)) {
    return oldXPos;
  }
  int xPos = oldXPos - id(margin_size) / 2;
  int yPos = getHeaderTextYPos();
  id(my_display).printf(xPos, yPos, & id(small_font), id(my_white), TextAlign::TOP_RIGHT, "%.0f%%", speakerGroup -> getVolumeLevel());
  return xPos;
}

int drawShuffle(int oldXPos) {
  if (speakerGroup -> activePlayer -> playerType == TVRemotePlayerType) {
    return oldXPos;
  }
  if (speakerGroup -> activePlayer -> playerState != StoppedRemoteState) {
    int xPos = oldXPos - id(icon_size) - id(margin_size) / 2;
    int yPos = id(header_height) / 2 - id(icon_size) / 2;
    if (speakerGroup -> mediaShuffling()) {
      id(my_display).image(xPos, yPos, & id(image_shuffle));
    } else if(id(draw_shuffle_disabled)) {
      id(my_display).image(xPos, yPos, & id(image_shuffle_disabled));
    } else {
      return oldXPos;
    }
    return xPos - id(margin_size) / 2;
  }
  return oldXPos;
}

int drawHeaderTime(int oldXPos) {
  int yPos = getHeaderTextYPos();
  std::string timeString = id(esptime).now().strftime("%I:%M%P");
  if(timeString.length() > 0 && timeString[0] == '0') {
    timeString.erase(0,1);
  }
  int xPos = oldXPos - getTextWidth(id(small_font_size), timeString.length());
  id(my_display).printf(xPos, yPos, &id(small_font), timeString.c_str());
  return xPos - id(margin_size) / 2;
}

int drawBattery(int oldXPos) {
  if(!id(draw_battery_level)) {
    return oldXPos;
  }
  int batteryWidth = 24;
  int batteryHeight = id(header_height) - 5;
  int yPos = 2;
  int capHeight = 6;
  int capWidth = 3;
  int xPos = oldXPos - batteryWidth;
  id(my_display).rectangle(xPos, yPos, batteryWidth, batteryHeight, id(my_gray_dark));
  id(my_display).rectangle(xPos + batteryWidth - 1, yPos + (batteryHeight / 2) - (capHeight / 2), capWidth, capHeight, id(my_gray_dark));
  if (!charging) {
    id(my_display).filled_rectangle(xPos + 1, yPos + 1, (batteryWidth * id(batteryPercent).state * 0.01) - 2, batteryHeight - 2, id(my_green));
  } else {
    id(my_display).filled_rectangle(xPos + 1, yPos + 1, (batteryWidth * id(batteryPercent).state * 0.01) - 2, batteryHeight - 2, id(my_yellow));
  }
  return xPos - id(margin_size);
}

void drawHeader() {
  id(my_display).rectangle(0, id(header_height), id(my_display).get_width(), 1, id(my_blue));
  drawHeaderTitle();
  int xPos = id(my_display).get_width() - id(margin_size) / 2;
  drawVolumeLevel(
    drawHeaderTime(
      drawShuffle(
        drawBattery(xPos)
      )
    )
  );
}

void drawTitle(int menuState, int i, std::string title, int yPos, bool buttonSpace) {
  int xPos = buttonSpace ? id(small_font_size) + id(margin_size) * 2 : id(margin_size);
  int textYPos = yPos + (id(margin_size) / 4);
  if (menuState == i) {
    int characterLimit = getCharacterLimit(xPos, id(medium_font_size), TextAlign::TOP_LEFT);
    if(marqueePosition > title.length() - characterLimit + 4) {
      marqueePosition = -6;
    }
    int marqueePositionMaxed = 0;
    if(title.length() > characterLimit) {
      marqueeText = true;
      marqueePositionMaxed = marqueePosition < title.length() ? marqueePosition : title.length();
      if(marqueePositionMaxed < 0) {
        marqueePositionMaxed = 0;
      }
    } else {
      marqueeText = false;
    }
    std::string marqueeTitle = title.erase(0, marqueePositionMaxed);
    id(my_display).filled_rectangle(0, yPos, id(my_display).get_width(), id(medium_font_size) + id(margin_size), id(my_blue));
    id(my_display).printf(xPos, textYPos, & id(medium_font), id(my_white), TextAlign::TOP_LEFT, "%s", marqueeTitle.c_str());
  } else {
    id(my_display).printf(xPos, textYPos, & id(medium_font), id(my_white), TextAlign::TOP_LEFT, "%s", title.c_str());
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
    id(my_display).filled_rectangle(id(my_display).get_width() - scrollBarWidth, headerHeight, scrollBarWidth, screenHeight, id(my_gray_dark_2));
    id(my_display).filled_rectangle(id(my_display).get_width() - scrollBarWidth + scrollBarMargin, yPos, scrollBarWidth - scrollBarMargin * 2, height - 1, id(my_blue));
  }
}

void drawSwitch(bool switchState, int yPos) {
  int circleSize = id(small_font_size) / 2;
  int xPos = id(margin_size) + circleSize;
  int centerYPos = yPos + (id(medium_font_size) + id(margin_size)) / 2;
  id(my_display).circle(xPos, centerYPos, circleSize, id(my_white));
  if (switchState) {
    id(my_display).filled_circle(xPos, centerYPos, circleSize - 2, id(my_white));
  }
}

void drawArrow(int yPos, int menuTitlesCount) {
  int xPos = id(my_display).get_width() - 8;
  if (menuTitlesCount > maxItems() + 1) {
    xPos = xPos - id(scroll_bar_width);
  }
  id(my_display).line(xPos, yPos + 4, xPos + 3, yPos + (id(medium_font_size) + id(margin_size)) / 2, id(my_white));
  id(my_display).line(xPos, yPos + (id(medium_font_size) + id(margin_size)) - 4, xPos + 3, yPos + (id(medium_font_size) + id(margin_size)) / 2, id(my_white));
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

void drawTitleImage(int characterCount, int yPos, MenuTitleState titleState, bool selected) {
  int adjustedYPos = yPos + (id(medium_font_size) / 4);
  int xPos = ((characterCount + 1) * (id(medium_font_size) * id(font_size_width_ratio))) + 4;
  switch(titleState) {
    case PlayingMenuTitleState:
      if(selected) {
        id(my_display).image(xPos, adjustedYPos, &id(image_play_white));
      } else {
        id(my_display).image(xPos, adjustedYPos, &id(image_play));
      }
      break;
    case PausedMenuTitleState:
      if(selected) {
        id(my_display).image(xPos, adjustedYPos, &id(image_pause_white));
      } else {
        id(my_display).image(xPos, adjustedYPos, &id(image_pause));
      }
      break;
    case StoppedMenuTitleState:
      if(selected) {
        id(my_display).image(xPos, adjustedYPos, &id(image_stop_white));
      } else {
        id(my_display).image(xPos, adjustedYPos, &id(image_stop));
      }
      break;
    case PowerOffMenuTitleState:
      if(selected) {
        id(my_display).image(xPos, adjustedYPos, &id(image_sleep_white));
      } else {
        id(my_display).image(xPos, adjustedYPos, &id(image_sleep));
      }
      break;
    default:
      break;
  }
}

void drawGroupedBar(int yPos, bool extend) {
  int xPos = 8;
  int width = 8;
  int lineHeight = extend ? id(medium_font_size) + id(margin_size) : (id(medium_font_size) + id(margin_size)) / 2;
  id(my_display).line(xPos, yPos, xPos, yPos + lineHeight, id(my_white));
  id(my_display).line(xPos, yPos + (id(medium_font_size) + id(margin_size)) / 2, xPos + width, yPos + (id(medium_font_size) + id(margin_size)) / 2, id(my_white));
}

void drawMenu(std::vector <MenuTitle> menuTitles) {
  activeMenuTitleCount = menuTitles.size();
  if(menuTitles.size() == 0 ) {
    return;
  }
  scrollMenuPosition();
  int menuState = menuIndex;
  MenuTitle menuTitleCopy = menuTitles[menuIndex];
  activeMenuTitle = menuTitleCopy;
  for (int i = scrollTop; i < menuTitles.size(); i++) {
    if (i > scrollTop + maxItems()) {
      break;
    }
    int yPos = ((i - scrollTop) * (id(medium_font_size) + id(margin_size))) + id(header_height);
    drawTitle(menuState, i, menuTitles[i].friendlyName, yPos, menuTitles[i].indentLine());
    switch(menuTitles[i].titleState) {
      case NoMenuTitleState:
        break;
      case OffMenuTitleState:
      case OnMenuTitleState:
        drawSwitch(menuTitles[i].titleState == OnMenuTitleState, yPos);
        break;
      case ArrowMenuTitleState:
        if(menuState == i) {
          drawArrow(yPos, menuTitles.size());
        }
        break;
      case PlayingMenuTitleState:
      case PausedMenuTitleState:
      case StoppedMenuTitleState:
      case PowerOffMenuTitleState:
        drawTitleImage(menuTitles[i].friendlyName.length(), yPos, menuTitles[i].titleState, menuState == i);
        break;
      case GroupedMenuTitleState:
        bool extend = i < menuTitles.size() && menuTitles[i+1].titleState == GroupedMenuTitleState;
        drawGroupedBar(yPos, extend);
        break;
    }
  }
  drawScrollBar(menuTitles.size(), id(header_height));
}

std::vector <MenuTitle> menuTypesToTitles(std::vector <MenuStates> menu) {
  std::vector <MenuTitle> out;
  for (auto & menuItem: menu) {
    out.push_back(menuTitleForType(menuItem));
  }
  return out;
}

std::vector <MenuTitle> activeMenu() {
  int x = menuIndex;
  switch (activeMenuState) {
  case rootMenu:
    return menuTypesToTitles(rootMenuTitles());
  case sourcesMenu:
    return speakerGroup -> activePlayer -> sources;
  case mediaPlayersMenu:
    return speakerGroup -> mediaPlayersTitleString();
  case scenesMenu:
    return sceneGroup -> sceneTitleStrings();
  case sensorsMenu:
    return sensorGroup -> sensorTitles();
  default:
    ESP_LOGW("WARNING", "menu is bad  %d", x);
    std::vector <MenuTitle> out;
    return out;
  }
}

void topMenu() {
  menuIndex = 0;
  speakerGroup->newSpeakerGroupParent = NULL;
  optionMenu = noOptionMenu;
  activeMenuState = MenuStates::rootMenu;
}

void idleMenu(bool force) {
  if(!charging || force) {
    menuIndex = 0;
    speakerGroup->newSpeakerGroupParent = NULL;
    optionMenu = noOptionMenu;
    activeMenuState = MenuStates::nowPlayingMenu;
    if(force) {
      displayUpdate.updateDisplay(true);
    }
  }
}

void activeTick() {
  if(activeMenuState == bootMenu) {
    return;
  }
  if((charging || idleTime < 15) && idleTime > 1 && (marqueeText || charging)) {
    if(marqueeText) {
      marqueePosition+=1.5;
    } else if(marqueePosition != 0) {
      marqueePosition = 0;
    }
    if(marqueePosition >= 0) {
      displayUpdate.updateDisplay(true);
    }
  } else if(marqueePosition != 0) {
    marqueePosition = 0;
    if(marqueeText) {
      displayUpdate.updateDisplay(true);
    }
  }
}

void idleTick() {
  bool updatedMediaPositions = speakerGroup->updateMediaPosition();
  if (idleTime == 3) {
    optionMenu = noOptionMenu;
    displayUpdate.updateDisplay(true);
  } else if (idleTime == 16) {
    if (speakerGroup -> playerSearchFinished) {
      idleMenu(false);
      displayUpdate.updateDisplay(false);
    }
    if(!charging) {
      ESP_LOGD("idle", "turning off display");
      id(backlight).turn_off();
    }
    idleTime ++;
    return;
  } else if(idleTime == 180 && charging) {
    idleMenu(true);
    idleTime++;
    return;
  } else if (idleTime > 6400) {
    if(!charging) {
      ESP_LOGI("idle", "night night");
      id(sleep_toggle).turn_on();
      return;
    }
  }
  if(updatedMediaPositions) {
    switch(activeMenuState) {
      case nowPlayingMenu:
        if(idleTime < 16 || charging) {
          displayUpdate.updateDisplay(true);
        }
        break;
      default:
        break;
    }
  }
  idleTime ++;
}

void drawTVOptionMenu() {
  id(my_display).circle(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.45 + 24, 48, id(my_gray));
  id(my_display).printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.15 + 16, & id(small_font), id(my_white), TextAlign::TOP_CENTER, "Remote Menu");
  id(my_display).printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.75 + 16, & id(small_font), id(my_white), TextAlign::TOP_CENTER, "Pause");
  id(my_display).printf(id(my_display).get_width() * 0.2, (id(my_display).get_height() - 16) * 0.45 + 16, & id(small_font), id(my_white), TextAlign::TOP_CENTER, "Back");
  id(my_display).printf(id(my_display).get_width() * 0.8, (id(my_display).get_height() - 16) * 0.45 + 16, & id(small_font), id(my_white), TextAlign::TOP_CENTER, "Home");
  id(my_display).printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.45 + 16, & id(small_font), id(my_white), TextAlign::TOP_CENTER, "TV Power");
}

void drawSpeakerOptionMenu() {
  id(my_display).circle(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.45 + 24, 48, id(my_gray));
  id(my_display).printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.15 + 16, & id(small_font), id(my_white), TextAlign::TOP_CENTER, speakerGroup -> shuffleString().c_str());
  id(my_display).printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.75 + 16, & id(small_font), id(my_white), TextAlign::TOP_CENTER, "Group");
  id(my_display).printf(id(my_display).get_width() * 0.8, (id(my_display).get_height() - 16) * 0.45 + 16, & id(small_font), id(my_white), TextAlign::TOP_CENTER, speakerGroup -> muteString().c_str());
}

void drawVolumeOptionMenu() {
  int barMargin = 1;
  int barHeight = id(small_font_size);
  int iconMargin = id(small_font_size) * id(font_size_width_ratio) * 3;
  int totalBarWidth = id(my_display).get_width() - iconMargin * 2;
  int barWidth = (totalBarWidth - 4) * (speakerGroup -> getVolumeLevel() / 100);
  int yPos = id(my_display).get_height() - barHeight - id(bottom_bar_margin);

  id(my_display).image(iconMargin / 2 - id(icon_size) / 2, yPos + 1, & id(image_volume_low));
  id(my_display).image(id(my_display).get_width() - iconMargin / 2 - id(icon_size) / 2, yPos + 1, & id(image_volume_high));

  id(my_display).rectangle(iconMargin, yPos, totalBarWidth, barHeight, id(my_blue));
  id(my_display).filled_rectangle(iconMargin + barMargin * 2, yPos + barMargin * 2, barWidth, barHeight - 2 - barMargin * 2, id(my_blue));
}

std::string secondsToString(int seconds) {
  if(seconds == -1) {
    return "00:00";
  }
  return seconds % 60 < 10 ? "0" + to_string(seconds % 60) : to_string(seconds % 60);
}

void drawMediaDuration() {
  if (speakerGroup -> activePlayer -> playerType != TVRemotePlayerType) {
    SonosSpeakerComponent * activeSpeaker = static_cast <SonosSpeakerComponent*>(speakerGroup->activePlayer);
    int mediaDuration = activeSpeaker->mediaDuration;
    int mediaPosition = activeSpeaker->mediaPosition;
    if(mediaDuration <= 0 && mediaPosition <= 0) {
      return;
    }
    int barMargin = 1;
    int barHeight = id(small_font_size);
    int textWidth = (id(small_font_size) * id(font_size_width_ratio) * 5) + id(margin_size) / 2;
    int totalBarWidth = id(my_display).get_width() - textWidth * 2;
    int barWidth = 0;
    if(mediaDuration > 0 && mediaPosition > 0) {
      barWidth = (totalBarWidth - 4) * ((double)mediaPosition / (double)mediaDuration);
    }

    int yPos = id(my_display).get_height() - barHeight - id(bottom_bar_margin);
    id(my_display).rectangle(textWidth, yPos, totalBarWidth, barHeight, id(my_white));
    id(my_display).filled_rectangle(textWidth + barMargin * 2, yPos + barMargin * 2, barWidth, barHeight - 2 - barMargin * 2, id(my_white));

    int textYPos = yPos - id(small_font_size) * 0.1;
    std::string mediaDurationSeconds = secondsToString(mediaDuration);
    std::string mediaPositionSeconds = secondsToString(mediaPosition);
    id(my_display).printf(id(margin_size), textYPos, & id(small_font), id(my_white), TextAlign::TOP_LEFT, "%d:%s", mediaPosition / 60, mediaPositionSeconds.c_str());
    id(my_display).printf(id(my_display).get_width() - id(margin_size), textYPos, & id(small_font), id(my_white), TextAlign::TOP_RIGHT, "%d:%s", mediaDuration / 60, mediaDurationSeconds.c_str());
  }
}

std::vector <std::string> getWrappedTitles(int xPos, int fontSize, TextAlign alignment, std::string text) {
  std::vector <std::string> output;
  if(text.size() == 0) {
    return output;
  }
  std::string wrappedTitles = textWrap(text, getCharacterLimit(xPos, fontSize, alignment));
  tokenize(wrappedTitles, "\n", output);
  return output;
}

int drawTextWrapped(int xPos, int yPos, int fontSize, Font * font, Color color, TextAlign alignment, std::vector <std::string> wrappedTitles, int maxLines) {
  int characterLimit = getCharacterLimit(xPos, fontSize, alignment);
  int max = maxLines != 0 && maxLines < wrappedTitles.size() ? maxLines : wrappedTitles.size();
  for (int i = 0; i < max; i++) {
    if(
      maxLines != 0 &&
      maxLines - 1 == i && 
      i == max-1
    ) {
      std::string title = wrappedTitles[i];
      if(wrappedTitles[i].size() > characterLimit - 2) {
        title.erase(title.length()-3);
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
    id(my_display).printf(id(my_display).get_width() / 2, 20 + id(margin_size), & id(medium_font), id(my_white), TextAlign::TOP_CENTER, "Playing...");
    auto playingNewSourceWrappedText = getWrappedTitles(id(my_display).get_width() / 2, id(large_font_size), TextAlign::TOP_CENTER, playingNewSourceText);
    drawTextWrapped(id(my_display).get_width() / 2, 40, 24, & id(large_font), id(my_white), TextAlign::TOP_CENTER, playingNewSourceWrappedText, 0);
    return true;
  }
  return true;
}

std::string stringForNowPlayingMenuState(NowPlayingMenuState state) {
  switch(state) {
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
  case shuffleNowPlayingMenuState:
    if(speakerGroup -> mediaShuffling()) {
      return "Shfl on";
    } else {
      return "Shfl off";
    }
  }
  return "";
}

std::vector <NowPlayingMenuState> getNowPlayingMenuStates() {
  if (speakerGroup -> activePlayer -> playerType == TVRemotePlayerType) {
    return TVNowPlayingMenuStates();
  }
  return speakerNowPlayingMenuStates();
}

void drawNowPlayingSelectMenu() {
  auto menuTitles = getNowPlayingMenuStates();
  activeMenuTitleCount = menuTitles.size();
  int yPos = id(my_display).get_height() - id(margin_size) - id(large_font_size);
  if(activeMenuTitleCount < 1) {
    return;
  }
  id(my_display).printf(id(my_display).get_width() * 0.5, yPos, & id(large_font), id(my_white), TextAlign::TOP_CENTER, stringForNowPlayingMenuState(menuTitles[menuIndex]).c_str());
  if(menuIndex + 1 < activeMenuTitleCount) {
    id(my_display).printf(id(my_display).get_width() * 0.85, yPos, & id(small_font), id(my_white), TextAlign::TOP_CENTER, stringForNowPlayingMenuState(menuTitles[menuIndex + 1]).c_str());
  } else {
    id(my_display).printf(id(my_display).get_width() * 0.85, yPos, & id(small_font), id(my_white), TextAlign::TOP_CENTER, stringForNowPlayingMenuState(menuTitles[0]).c_str());
  }
  if(menuIndex - 1 >= 0) {
    id(my_display).printf(id(my_display).get_width() * 0.15, yPos, & id(small_font), id(my_white), TextAlign::TOP_CENTER, stringForNowPlayingMenuState(menuTitles[menuIndex - 1]).c_str());
  } else {
    id(my_display).printf(id(my_display).get_width() * 0.15, yPos, & id(small_font), id(my_white), TextAlign::TOP_CENTER, stringForNowPlayingMenuState(menuTitles[activeMenuTitleCount - 1]).c_str());
  }
}

void drawNowPlaying() {
  if (drawOptionMenuAndStop()) {
    return;
  }
  if (id(draw_now_playing_menu)){
    drawNowPlayingSelectMenu();
  }
  int yPos = id(header_height) + id(margin_size) / 4;
  if(speakerGroup->activePlayer->playerState == PowerOffRemoteState) {
    id(my_display).printf(id(my_display).get_width() / 2, yPos, & id(large_font), id(my_white), TextAlign::TOP_CENTER, "Power Off");
    return;
  }
  std::string nowPlayingText = "Now Playing,";

  if (speakerGroup -> activePlayer -> playerType != TVRemotePlayerType) {
    SonosSpeakerComponent * activeSpeaker = static_cast <SonosSpeakerComponent*>(speakerGroup->activePlayer);
    if(activeSpeaker->mediaPlaylist != activeSpeaker->mediaTitle) {
      nowPlayingText += " " + activeSpeaker->mediaPlaylist;
    } else if(activeSpeaker->mediaAlbumName != activeSpeaker->mediaTitle) {
      nowPlayingText += " " + activeSpeaker->mediaAlbumName;
    }
  }
  int xPos = id(my_display).get_width() / 2;
  auto nowPlayingWrappedText = getWrappedTitles(id(margin_size), id(medium_font_size), TextAlign::TOP_LEFT, nowPlayingText);
  auto mediaArtistWrappedText = getWrappedTitles(xPos, id(large_font_size), TextAlign::TOP_CENTER, speakerGroup -> getMediaArtistString());
  auto mediaTitleWrappedText = getWrappedTitles(xPos, id(medium_font_size), TextAlign::TOP_CENTER, speakerGroup -> getMediaTitleString());
  int lineCount = nowPlayingWrappedText.size() + mediaArtistWrappedText.size() + mediaTitleWrappedText.size();
  int maxLines = 0;
  if(lineCount > id(now_playing_max_lines)) {
    maxLines = 1;
    if(nowPlayingWrappedText.size() > 1) {
      lineCount = 1 + mediaArtistWrappedText.size() + mediaTitleWrappedText.size();
    }
  }
  yPos = drawTextWrapped(id(margin_size), yPos, id(medium_font_size), & id(medium_font), id(my_white), TextAlign::TOP_LEFT, nowPlayingWrappedText, maxLines);
  if (speakerGroup -> activePlayer -> mediaArtist == "" && speakerGroup -> activePlayer -> mediaTitle == "") {
    id(my_display).printf(id(my_display).get_width() / 2, yPos, & id(large_font), id(my_white), TextAlign::TOP_CENTER, "Nothing!");
    return;
  }
  if(lineCount > id(now_playing_max_lines)) {
    maxLines = 2;
  } else {
    maxLines = 0;
  }
  yPos = yPos + id(margin_size) / 2;
  if (mediaArtistWrappedText.size() > 0) {
    yPos = drawTextWrapped(xPos, yPos, id(large_font_size), & id(large_font), id(my_white), TextAlign::TOP_CENTER, mediaArtistWrappedText, maxLines);
  }
  if (mediaTitleWrappedText.size() > 0) {
    yPos = yPos + id(margin_size);
    drawTextWrapped(id(my_display).get_width() / 2, yPos, id(medium_font_size), & id(medium_font), id(my_white), TextAlign::TOP_CENTER, mediaTitleWrappedText, maxLines);
  }
  if (optionMenu == volumeOptionMenu) {
    drawVolumeOptionMenu();
  } else {
    drawMediaDuration();
  }
}

int autoClearState = 0;

void drawBootSequence() {
  if(autoClearState == 0) {
    id(my_display).set_auto_clear(false);
    autoClearState = abs((int)esp_random()) + 1; // add 1 in case its 0
  }

  int imageXPos = esp_random() % (id(my_display).get_width() - id(icon_size));
  int imageYPos = esp_random() % (id(my_display).get_height() - id(icon_size));
  id(my_display).image(imageXPos, imageYPos, & id(image_sleep));
  std::vector<Color> colors = { id(my_green), id(my_blue), id(my_yellow), id(my_red) };

  for(int i = 0; i < 3; i++) {
    int xPos = autoClearState % (id(my_display).get_width() / 3);
    int yPos = autoClearState % (id(my_display).get_height() - id(large_font_size) * 2);
    auto wrappedBootText = getWrappedTitles(xPos, id(large_font_size), TextAlign::TOP_LEFT, id(boot_device_name));
    drawTextWrapped(xPos, yPos, id(large_font_size), & id(large_font), colors[esp_random() % colors.size()], TextAlign::TOP_LEFT, wrappedBootText, 0);
    autoClearState++;
  }
  speakerGroup -> findActivePlayer();
  menuDrawing = false;
}

void drawMenu() {
  if(idleTime > 16 && !charging) {
    menuDrawing = false;
    return;
  }
  if (speakerGroup -> playerSearchFinished == false) {
    drawBootSequence();
    return;
  } else if(activeMenuState == bootMenu) {
    activeMenuState = rootMenu;
  }
  if(autoClearState != 0) {
    id(my_display).set_auto_clear(true);
    autoClearState = 0;
  }
  switch (activeMenuState) {
  case nowPlayingMenu:
    drawNowPlaying();
    break;
  case lightsMenu:
    drawMenu(lightGroup -> lightTitleSwitches());
    break;
  case groupMenu:
    if (speakerGroup -> newSpeakerGroupParent != NULL) {
      drawMenu(speakerGroup -> groupTitleSwitches());
      break;
    }
    drawMenu(speakerGroup -> groupTitleString());
    break;
  default:
    drawMenu(activeMenu());
    break;
  }
  drawHeader();
  menuDrawing = false;
}

void selectMediaPlayers() {
  if(activeMenuTitle.entityId == speakerGroup->tv->entityId) {
    speakerGroup->activePlayer = speakerGroup->tv;
  } else {
    for (auto & speaker: speakerGroup->speakers) {
      if(speaker->entityId == activeMenuTitle.entityId) {
        speakerGroup->activePlayer = speaker;
        break;
      }
    }
  }
  topMenu();
}

bool selectRootMenu() {
  MenuStates currentMenu = rootMenuTitles()[menuIndex];
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
  case groupMenu:
  case rootMenu:
  case bootMenu:
    ESP_LOGD("WARNING", "menu is bad  %d", menuIndex);
    return false;
  }
  menuIndex = 0;
  return true;
}

bool selectMenu() {
  int menuIndexForSource = menuIndex;
  switch (activeMenuState) {
  case rootMenu:
    return selectRootMenu();
  case nowPlayingMenu:
    activeMenuState = MenuStates::nowPlayingMenu;
    break;
  case sourcesMenu:
    idleMenu(true);
    speakerGroup -> activePlayer -> playSource(menuIndexForSource);
    optionMenu = playingNewSourceMenu;
    displayUpdate.updateDisplay(true);
    break;
  case groupMenu:
    speakerGroup -> selectGroup(&activeMenuTitle);
    break;
  case mediaPlayersMenu:
    selectMediaPlayers();
    break;
  case scenesMenu:
    if (sceneGroup -> selectScene(menuIndexForSource)) {
      topMenu();
    }
    break;
  case lightsMenu:
    if (lightGroup -> selectLight(menuIndexForSource)) {
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
  if (!speakerGroup -> playerSearchFinished) {
    speakerGroup -> findActivePlayer();
  }
  if (!id(backlight).state) {
    id(backlight).turn_on();
    displayUpdate.updateDisplay(false);
    return true;
  }
  return false;
}

#endif