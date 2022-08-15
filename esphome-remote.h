#include "esphome.h"
#include "esphome-remote-players.h"

enum MenuStates {
  rootMenu,
  volumeMenu,
  // shortcuts,
  sourcesMenu,
  groupMenu,
  mediaPlayersMenu,
  lightsMenu,
  tvNowPlayingMenu,
  speakerNowPlayingMenu
};

enum MenuStringType {
  nowPlaying,
  sources,
  // shortcuts,
  mediaPlayers,
  lights,
  backlightString,
  sleepString,
  back
};

enum OptionMenuType {
  noOptionMenu,
  volumeOptionMenu,
  tvOptionMenu,
  speakerOptionMenu,
  playingNewSourceMenu
};

MenuStates activeMenuState = rootMenu;
int scrollTop = 0;
int menuIndex = 0;
int fontSize = 15;
int marginSize = 4;
int rotaryPosition = 0;
OptionMenuType optionMenu = noOptionMenu;

class DisplayUpdateImpl : public DisplayUpdateInterface
{
  public:
    virtual void updateDisplay(bool force) {
    // ESP_LOGD("update", "update");
    if(force) {
      if (!id(backlight).state) {
        id(backlight).turn_on();
      }
      id(my_display).update();
      return;
    }

    if (id(backlight).state) {
      switch(activeMenuState) {
        case sourcesMenu:
          return;
        default:
          id(my_display).update();
          break;
      }
    }
  }
};

class BaseService: public CustomAPIDevice {
  public:
    std::string entityId;
    std::string friendlyName;
    std::string serviceType;
    std::string getEntityId() { return entityId; }
    std::string getFriendlyName() { return friendlyName; }
    void callService() {
      if (serviceType == "script") {
        call_homeassistant_service(entityId.c_str());
      } else {
        call_homeassistant_service("scene.turn_on", {
          {"entity_id", entityId.c_str()}
        });
      }
    }

    void superSetup(std::string newEntityId, std::string newFriendlyName) {
      entityId = newEntityId;
      friendlyName = newFriendlyName;
    }
};

class SceneService: public BaseService {
  public:
    SceneService(std::string newEntityId, std::string newFriendlyName) {
      serviceType = "scene";
      superSetup(newEntityId, newFriendlyName); 
   }
};

class ScriptService: public BaseService {
  public:
    ScriptService(std::string newEntityId, std::string newFriendlyName) {
      serviceType = "script";
      superSetup(newEntityId, newFriendlyName); 
   }
};

class SceneGroupComponent : public CustomAPIDevice, public Component {
 public:
  std::vector<BaseService> services;

  void setup(std::vector<BaseService> newServices) {
    services = newServices;
  }

  std::vector<std::string> sceneTitleStrings() {
    std::vector<std::string> out;
    for (auto &service: services) {
      out.push_back(service.getFriendlyName());
    }
    return out;
  }

  bool selectLights(int index) {
   if(index >= 0 && index < services.size()) {
      BaseService service = services[index];
      ESP_LOGD("Scene", "Service called %s", service.getFriendlyName().c_str());
      service.callService();
      return false;
    }
    return true;
  }
};

auto displayUpdate = DisplayUpdateImpl();
auto *sceneGroup = new SceneGroupComponent();
auto *speakerGroup = new SonosSpeakerGroupComponent(displayUpdate);

std::string menuStringForType(MenuStringType stringType) {
  switch(stringType) {
  case nowPlaying:
    return "Now Playing";
  case sources:
    return "Sources";
  case backlightString:
    return "Backlight";
  case sleepString:
    return "Sleep";
  case mediaPlayers:
    return "Media Players";
  case lights:
    return "Lights";
  default:
    return "DEFAULT";
  }
}

int batteryWidth = 24;

void drawPlayPauseIcon() {
  int yPos = 2;
  int xPos = 4;
  if (speakerGroup->activePlayer->playerState == "playing") {
    id(my_display).image(id(my_display).get_width() - xPos - batteryWidth - 16, 2, &id(image_play));
  } else if (speakerGroup->activePlayer->playerState == "paused") {
    id(my_display).image(id(my_display).get_width() - xPos - batteryWidth - 16, 2, &id(image_pause));
  } else {
    id(my_display).image(id(my_display).get_width() - xPos - batteryWidth - 16, 2, &id(image_stop));
  }
}

void drawCurrentMediaPlayer() {
  if (speakerGroup->activePlayer->playerName != "") {
    std::string playerName = speakerGroup->activePlayer->friendlyName;
    if(speakerGroup->activePlayer->playerType != "TV") {
      SonosSpeakerComponent* activeSpeaker = static_cast<SonosSpeakerComponent*>(speakerGroup->activePlayer);
      if (activeSpeaker != NULL) {
        if(activeSpeaker->groupMembers.size() > 0) {
          playerName = playerName + " + " + to_string(activeSpeaker->groupMembers.size());
        }
      }
    }
    id(my_display).printf(2, 0, &id(helvetica_8), id(my_white), "%s", playerName.c_str());     
  }
}

void drawBattery() {
  int batteryHeight = 12;
  int yPos = 2;
  int xPos = 4;
  if (id(TTGoBatteryPercent).state < 100) {
    id(my_display).rectangle(id(my_display).get_width() - xPos - batteryWidth, yPos, batteryWidth, batteryHeight, id(my_gray));
    id(my_display).filled_rectangle(id(my_display).get_width() - xPos - batteryWidth + 1, yPos + 1, (batteryWidth * id(TTGoBatteryPercent).state * 0.01) - 2, batteryHeight - 2, id(my_green));
  } else {
    id(my_display).rectangle(id(my_display).get_width() - xPos - batteryWidth, yPos, batteryWidth, batteryHeight, id(my_gray));
    id(my_display).filled_rectangle(id(my_display).get_width() - xPos - batteryWidth + 1, yPos + 1, batteryWidth - 2, batteryHeight - 2, id(my_yellow));
  }
}

void drawVolumeLevel() {
  int xPos = id(my_display).get_width() - batteryWidth - 24;
  id(my_display).printf(xPos, 0, &id(helvetica_8), id(my_white), TextAlign::TOP_RIGHT, "%.0f%%", speakerGroup->getVolumeLevel());
}

void drawHeader() {
  id(my_display).rectangle(0, 16, id(my_display).get_width(), 1, id(my_blue));
  drawBattery();
  drawCurrentMediaPlayer();
  drawPlayPauseIcon();
  drawVolumeLevel();
}

void drawTitle(int menuState, int i, std::string title, int yPos, bool buttonSpace) {
  int xPos = buttonSpace ? 20 : 4;
  if(menuState == i) {
      id(my_display).filled_rectangle(0, yPos, id(my_display).get_width() - 4, fontSize + marginSize, id(my_blue));
      id(my_display).printf(xPos, yPos, &id(helvetica_12), id(my_white), TextAlign::TOP_LEFT, "%s", title.c_str());
  } else {
      id(my_display).printf(xPos, yPos, &id(helvetica_12), id(my_white), TextAlign::TOP_LEFT, "%s", title.c_str());
  }
}

double headerHeight = 16;

int maxItems() {
  int maxItems = ((id(my_display).get_height() - headerHeight) / (fontSize + marginSize)) - 1;
  return maxItems;
}


void drawScrollBar(int menuTitlesCount, double headerHeight) {
  if(menuTitlesCount > maxItems() + 1) {
    double screenHeight = id(my_display).get_height() - headerHeight;
    double height = maxItems() * (screenHeight / menuTitlesCount);
    double yPos = (((screenHeight - height) / (menuTitlesCount - 1)) * menuIndex) + 1 + headerHeight;
    id(my_display).filled_rectangle(id(my_display).get_width() - 3, yPos, 2, height - 1, id(my_blue));
  }
}

void drawSwitch(bool switchState, int yPos) {
  id(my_display).circle(8, yPos + (fontSize + marginSize) / 2, 7, id(my_white));
  if(switchState) {
    id(my_display).filled_circle(8, yPos + (fontSize + marginSize) / 2, 5, id(my_white));
  }
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

int activeMenuTitleCount = 0;

void drawMenu(std::vector<std::string> menuTitles) {
  activeMenuTitleCount = menuTitles.size();
  scrollMenuPosition();
  int menuState = menuIndex;
  for(int i = scrollTop; i < menuTitles.size(); i++) {
      if(i > scrollTop + maxItems()) {
          break;
      }
      drawTitle(menuState, i, menuTitles[i], ((i - scrollTop) * (fontSize + marginSize)) + headerHeight, 0);
  }
  drawScrollBar(menuTitles.size(), headerHeight);
}

void drawSwitchMenu(std::vector<MenuTitleSwitch> menuTitleSwitches) {
  activeMenuTitleCount = menuTitleSwitches.size();
  scrollMenuPosition();
  int menuState = menuIndex;
  for(int i = scrollTop; i < menuTitleSwitches.size(); i++) {
      if(i > scrollTop + maxItems()) {
          break;
      }
      int yPos = ((i - scrollTop) * (fontSize + marginSize)) + headerHeight;
      drawTitle(menuState, i, menuTitleSwitches[i].friendlyName, yPos, menuTitleSwitches[i].toggleState != 0);
      if(menuTitleSwitches[i].toggleState != 0) {
        drawSwitch(menuTitleSwitches[i].toggleState == 2, yPos);
      }
  }
  drawScrollBar(menuTitleSwitches.size(), headerHeight);
}

std::vector<std::string> menuToString(std::vector<MenuStringType> menu) {
  std::vector<std::string> out;
  for (auto &menuItem: menu) {
      out.push_back(menuStringForType(menuItem));
  }
  return out;
}

std::vector<MenuStringType> rootMenuTitles() {
  return { nowPlaying, sources, mediaPlayers, lights, sleepString };
}

std::vector<std::string> activeMenu() {
  int x = menuIndex;
  switch(activeMenuState) {
    case rootMenu:
      return menuToString(rootMenuTitles());
    // case shortcuts:
    //   return shortcutStrings;
    case sourcesMenu:
      return speakerGroup->activePlayer->sources;
    case mediaPlayersMenu:
      return speakerGroup->mediaPlayersTitleString();
    case lightsMenu:
      return sceneGroup->sceneTitleStrings();
    default:
      ESP_LOGD("WARNING", "menu is bad  %d", x);
      std::vector<std::string> out;
      return out;
  }
}

void topMenu() {
  menuIndex = 0;
  speakerGroup->newSpeakerGroupParent = NULL;
  optionMenu = noOptionMenu;
  activeMenuState = MenuStates::rootMenu;
}

void idleMenu() {
  menuIndex = 0;
  speakerGroup->newSpeakerGroupParent = NULL;
  optionMenu = noOptionMenu;
  if (speakerGroup->activePlayer->playerType == "TV") {
    activeMenuState = MenuStates::tvNowPlayingMenu;
  } else {
    activeMenuState = MenuStates::speakerNowPlayingMenu;
  }
}

void idleTick() {
  if(id(idle_time) == 5) {
    optionMenu = noOptionMenu;
    displayUpdate.updateDisplay(true);
  } else if(id(idle_time) == 20) {
    if(speakerGroup->playerSearchFinished) {
      idleMenu();
      displayUpdate.updateDisplay(true);
    }
    ESP_LOGD("idle", "turning off display");
    id(backlight).turn_off();
  } else if(id(idle_time) > 3600) {
    ESP_LOGD("idle", "night night");
    id(tt_sleep).turn_on();
    return;
  }
  id(idle_time)++;
}

void drawTVOptionMenu() {
  id(my_display).circle(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.45 + 24, 48, id(my_gray));
  id(my_display).printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.15 + 16, &id(helvetica_8), id(my_white), TextAlign::TOP_CENTER, "Remote Menu");
  id(my_display).printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.75 + 16, &id(helvetica_8), id(my_white), TextAlign::TOP_CENTER, "Pause");
  id(my_display).printf(id(my_display).get_width() * 0.2, (id(my_display).get_height() - 16) * 0.45 + 16, &id(helvetica_8), id(my_white), TextAlign::TOP_CENTER, "Back");
  id(my_display).printf(id(my_display).get_width() * 0.8, (id(my_display).get_height() - 16) * 0.45 + 16, &id(helvetica_8), id(my_white), TextAlign::TOP_CENTER, "Home");
  id(my_display).printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.45 + 16, &id(helvetica_8), id(my_white), TextAlign::TOP_CENTER, "TV Power");
}

void drawSpeakerOptionMenu() {
  id(my_display).circle(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.45 + 24, 48, id(my_gray));
  id(my_display).printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.15 + 16, &id(helvetica_8), id(my_white), TextAlign::TOP_CENTER, speakerGroup->shuffleString().c_str());
  id(my_display).printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.75 + 16, &id(helvetica_8), id(my_white), TextAlign::TOP_CENTER, "Group");
  // id(my_display).printf(id(my_display).get_width() * 0.2, (id(my_display).get_height() - 16) * 0.5 + 16, &id(helvetica_8), id(my_white), TextAlign::TOP_CENTER, "Back");
  // id(my_display).printf(id(my_display).get_width() * 0.8, (id(my_display).get_height() - 16) * 0.5 + 16, &id(helvetica_8), id(my_white), TextAlign::TOP_CENTER, "Home");
  // id(my_display).printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.45 + 16, &id(helvetica_8), id(my_white), TextAlign::TOP_CENTER, "TV Power");
}

bool drawOptionMenuAndStop() {
  ESP_LOGD("option", "start");
  switch(optionMenu) {
    case tvOptionMenu:
    ESP_LOGD("option", "tv");
      drawTVOptionMenu();
      return true;
    case speakerOptionMenu:
    ESP_LOGD("option", "speaker");
      drawSpeakerOptionMenu();
      return true;
    case volumeOptionMenu:
    ESP_LOGD("option", "volume");
      id(my_display).rectangle(16, id(my_display).get_height() - 16, id(my_display).get_width() - 32, 10, id(my_blue));
      id(my_display).filled_rectangle(18, id(my_display).get_height() - 14, (id(my_display).get_width() - 36) * (speakerGroup->getVolumeLevel() / 100), 6, id(my_blue));
      return false;
    case noOptionMenu:
      return false;
    case playingNewSourceMenu:
    ESP_LOGD("option", "new source");
      id(my_display).printf(8, 20 + marginSize, &id(helvetica_12), id(my_white), "Playing");
      id(my_display).printf(8, 36 + marginSize, &id(helvetica_24), id(my_white), "%s", playingNewSourceText.c_str());
      return true;
  }
  return true;
}

void drawSpeakerNowPlaying() {
  int yPos = 24;
  if(drawOptionMenuAndStop()) {
    return;
  }
  id(my_display).printf(8, yPos, &id(helvetica_12), id(my_white), TextAlign::TOP_LEFT, "Now Playing,");
  if(speakerGroup->activePlayer->mediaArtist == "" && speakerGroup->activePlayer->mediaTitle == "") {
    id(my_display).printf(8, yPos + 24 + marginSize, &id(helvetica_24), id(my_white), "Nothing!");
    return; 
  }
  if (speakerGroup->activePlayer->mediaArtist != "") {
    id(my_display).printf(8, yPos + 24 + marginSize, &id(helvetica_24), id(my_white), "%s", speakerGroup->activePlayer->mediaArtist.c_str());
  }
  if (speakerGroup->activePlayer->mediaTitle != "") {
    id(my_display).printf(8, yPos + 56 + marginSize, &id(helvetica_12), id(my_white), "%s", speakerGroup->activePlayer->mediaTitle.c_str());
  }
}

void drawMenu() {
  if(speakerGroup->playerSearchFinished == false) {
    ESP_LOGD("drawMenu", "draw beep boop");
    id(my_display).printf(40, 40, &id(helvetica_24), id(my_blue), TextAlign::TOP_LEFT, "beep boop");
    speakerGroup->findActivePlayer();
    return;
  }
  switch(activeMenuState) {
    case tvNowPlayingMenu:
      drawSpeakerNowPlaying();
      break;
    case speakerNowPlayingMenu:
      drawSpeakerNowPlaying();
      break;
    case sourcesMenu:
      drawMenu(activeMenu());
      break;
    case groupMenu:
      if(speakerGroup->newSpeakerGroupParent != NULL) {
        drawSwitchMenu(speakerGroup->groupTitleSwitches());
        break;
      }
      drawMenu(speakerGroup->groupTitleString());
      break;
    default:
      drawMenu(activeMenu());
      break;
  }
  drawHeader();
}

void selectMediaPlayers() {
  if (menuIndex >= 0 && menuIndex < speakerGroup->speakers.size()) {
    speakerGroup->activePlayer = speakerGroup->speakers[menuIndex];
  } else if (menuIndex == speakerGroup->speakers.size()) {
    speakerGroup->activePlayer = speakerGroup->tv;
  }
  ESP_LOGD("media player", "selected  %s %d %d", speakerGroup->activePlayer->friendlyName.c_str(), menuIndex, speakerGroup->speakers.size());
  topMenu();
}

bool selectRootMenu() {
  MenuStringType currentMenu = rootMenuTitles()[menuIndex];
  ESP_LOGD("menu", "menu  %s", menuStringForType(currentMenu).c_str());
  switch(currentMenu) {
    case sources:
      activeMenuState = sourcesMenu;
      break;
    case nowPlaying:
      if(speakerGroup->activePlayer->playerType == "TV") {
        activeMenuState = tvNowPlayingMenu;
      } else {
        activeMenuState = speakerNowPlayingMenu;
      }
      break;
    case mediaPlayers:
      activeMenuState = mediaPlayersMenu;
      break;
    case lights:
      activeMenuState = lightsMenu;
      break;
    case backlightString:
      topMenu();
      id(backlight).turn_off();
      return false;
    case sleepString:
      id(tt_sleep).turn_on();
      return false;
    default:
      ESP_LOGD("WARNING", "menu is bad  %d", menuIndex);
      return false;
  }
  menuIndex = 0;
  return true;
}

bool selectMenu() {
  int menuIndexForSource = menuIndex;
  switch(activeMenuState) {
    case rootMenu:
      return selectRootMenu();
    case speakerNowPlayingMenu:
      activeMenuState = MenuStates::speakerNowPlayingMenu;
      break;
    case tvNowPlayingMenu:
      activeMenuState = MenuStates::tvNowPlayingMenu;
      break;
    case sourcesMenu:
      idleMenu();
      speakerGroup->activePlayer->playSource(menuIndexForSource);
      optionMenu = playingNewSourceMenu;
      displayUpdate.updateDisplay(true);
      break;
    case groupMenu:
      speakerGroup->selectGroup(menuIndex);
      break;
    case mediaPlayersMenu:
      selectMediaPlayers();
      break;
    case lightsMenu:
      if (sceneGroup->selectLights(menuIndexForSource)) {
        topMenu();
      }
      break;
    default:
      ESP_LOGD("WARNING", "menu state is bad but its an enum");
      return false;
  }
  return true;
}

void debounceUpdateDisplay() {
  if(id(display_update_tick).state != id(rotary).state) {
    id(display_update_tick).publish_state(id(rotary).state);
  }
}

bool buttonPressWakeUpDisplay() {
  if(id(idle_time) != -1) {
    id(idle_time) = 0;
  }
  if(!speakerGroup->playerSearchFinished) {
    speakerGroup->findActivePlayer();
  }
  if (!id(backlight).state) {
    id(backlight).turn_on();
    return true;
  }
  return false;
}

void buttonPressSelect() {
  if(buttonPressWakeUpDisplay()) { return; }
  switch(activeMenuState) {
    case tvNowPlayingMenu:
      if(optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
        speakerGroup->tv->tvRemoteCommand("power");
        displayUpdate.updateDisplay(true);
      } else {
        speakerGroup->tv->tvRemoteCommand("select");
      }
      return;
    default:
      break;
  }
  if (selectMenu()) {
    displayUpdate.updateDisplay(true);
  }
}

void buttonPressNext() {
  switch(activeMenuState) {
    case tvNowPlayingMenu:
    case speakerNowPlayingMenu:
      speakerGroup->increaseSpeakerVolume();
      optionMenu = volumeOptionMenu;
      debounceUpdateDisplay();
      return;
    default:
      break;
  }
  if (menuIndex < activeMenuTitleCount-1) {
    menuIndex++;
  } else if (menuIndex == activeMenuTitleCount-1) {
    // menuIndex = 0;
  } else {
    menuIndex = 0;
  }
  debounceUpdateDisplay();
}

void buttonPressPrevious() {
  switch(activeMenuState) {
    case tvNowPlayingMenu:
    case speakerNowPlayingMenu:
      speakerGroup->decreaseSpeakerVolume();
      optionMenu = volumeOptionMenu;
      debounceUpdateDisplay();
      return;
    default:
      break;
  }
  if (menuIndex > 0) {
    menuIndex--;
  } else {
    // topMenu();
  }
  debounceUpdateDisplay();
}

void buttonPressUp() {
  if(buttonPressWakeUpDisplay()) { return; }
  switch(activeMenuState) {
    case tvNowPlayingMenu:
      if(optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
        topMenu();
        displayUpdate.updateDisplay(true);
      } else {
        speakerGroup->tv->tvRemoteCommand("up");
      }
      break;
    default:
      if(optionMenu == speakerOptionMenu) {
        speakerGroup->toggleShuffle();
        optionMenu = noOptionMenu;
        displayUpdate.updateDisplay(true);
        return;
      }
      topMenu();
      displayUpdate.updateDisplay(true);
      break;
  }
}

void buttonPressDown() {
  if(buttonPressWakeUpDisplay()) { return; }
  switch(activeMenuState) {
    case tvNowPlayingMenu:
      if(optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
        speakerGroup->tv->tvRemoteCommand("play");
        displayUpdate.updateDisplay(true);
      } else {
        speakerGroup->tv->tvRemoteCommand("down");
      }
      break;
    case speakerNowPlayingMenu:
      if(optionMenu == speakerOptionMenu) {
        optionMenu = noOptionMenu;
        activeMenuState = groupMenu;
        displayUpdate.updateDisplay(true);
        return;
      }
      speakerGroup->activePlayer->playPause();
      return;
    default:
      break;
  }
}

void buttonPressLeft() {
  if(buttonPressWakeUpDisplay()) { return; }
  switch(activeMenuState) {
    case tvNowPlayingMenu:
      if(optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
        speakerGroup->tv->tvRemoteCommand("back");
        displayUpdate.updateDisplay(true);
      } else {
        speakerGroup->tv->tvRemoteCommand("left");
      }
      break;
    case speakerNowPlayingMenu:
      // speakerGroup->activePlayer->back();
      return;
    default:
      break;
  }
}

void buttonPressRight() {
  if(buttonPressWakeUpDisplay()) { return; }
  switch(activeMenuState) {
    case tvNowPlayingMenu:
      if(optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
        speakerGroup->tv->tvRemoteCommand("menu");
        displayUpdate.updateDisplay(true);
      } else {
        speakerGroup->tv->tvRemoteCommand("right");
      }
      break;
    case speakerNowPlayingMenu:
      speakerGroup->activePlayer->nextTrack();
      return;
    default:
      break;
  }
}

void buttonReleaseScreenLeft() {
  if(buttonPressWakeUpDisplay()) { return; }
  switch(activeMenuState) {
    case tvNowPlayingMenu:
      displayUpdate.updateDisplay(true);
      break;
    default:
      break;
  }
}

void buttonPressScreenLeft() {
  if(buttonPressWakeUpDisplay()) { return; }
  switch(activeMenuState) {
    case tvNowPlayingMenu:
      if(optionMenu == tvOptionMenu) {
        optionMenu = noOptionMenu;
      } else {
        optionMenu = tvOptionMenu;
      }
      displayUpdate.updateDisplay(true);
      break;
    case speakerNowPlayingMenu:
      if(optionMenu == speakerOptionMenu) {
        optionMenu = noOptionMenu;
      } else {
        optionMenu = speakerOptionMenu;
      }
      displayUpdate.updateDisplay(true);
      break;
    default:
      break;
  }
}

void buttonPressScreenRight() {
  if(buttonPressWakeUpDisplay()) { return; }
  optionMenu = noOptionMenu;
  switch(activeMenuState) {
    case tvNowPlayingMenu:
      topMenu();
      displayUpdate.updateDisplay(true);
      break;
    default:
      topMenu();
      displayUpdate.updateDisplay(true);
      break;
  }
}