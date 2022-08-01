#include "esphome.h"

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
  group,
  // shortcuts,
  shuffle,
  mediaPlayers,
  lights,
  backlightString,
  sleepString,
  back
};

enum OptionMenuType {
  noOptionMenu,
  volumeOptionMenu,
  tvOptionMenu
};

MenuStates activeMenuState = rootMenu;
int scrollTop = 0;
int menuIndex = 0;
int fontSize = 15;
int marginSize = 4;
int rotaryPosition = 0;
OptionMenuType optionMenu = noOptionMenu;

void tokenize(std::string const &str, std::string delim, std::vector<std::string> &out) {
  size_t start;
  size_t end = 0;

  while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
  {
      end = str.find(delim, start);
      out.push_back( str.substr(start, end - start));
  }
}

std::string filter(std::string str) {
  std::string output;
  output.reserve(str.size()); // optional, avoids buffer reallocations in the loop
  for(size_t i = 0; i < str.size(); ++i) {
    if(i == 0 && str[i] == ' ') continue;
    if(i == str.size() - 1 && str[i] == '}') return output;
    if(str[i] != '[' && str[i] != ']' && str[i] != '\'' && str[i] != '"') output += str[i];
  }
  return output;
}

void updateDisplay(bool force) {
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

class BasePlayerComponent : public CustomAPIDevice, public Component {
  public:
    std::string playerState = "";
    std::string mediaTitle = "";
    std::string mediaArtist = "";
    std::string friendlyName = "";
    std::string playerName = "";
    std::string playerType = "";
    std::vector<std::string> sources;

  void superSetup(std::string newPlayerName) {
    playerName = newPlayerName;
    ESP_LOGD("Player", "Player subbed %s", playerName.c_str());
    subscribe_homeassistant_state(&BasePlayerComponent::playerState_changed, newPlayerName.c_str());
    subscribe_homeassistant_state(&BasePlayerComponent::player_name_changed, newPlayerName, "friendly_name");
  }

  void playerState_changed(std::string state) {
    ESP_LOGD("Player", "%s state changed to %s", playerName.c_str(), state.c_str());
    playerState = state.c_str();
    updateDisplay(false);
  }

  void player_name_changed(std::string state) {
    ESP_LOGD("Player", "%s name changed to %s", playerName.c_str(), state.c_str());
    friendlyName = state.c_str();
    updateDisplay(false);
  }

  void playSource(float index) {
    ESP_LOGD("speaker", "%s playing %f %s", playerName.c_str(), index, sources[index].c_str());
    call_homeassistant_service("media_player.select_source", {
      {"entity_id", playerName},
      {"source", sources[index].c_str()},
    });
  }

  void playPause() {
    ESP_LOGD("speaker", "%s play pause", playerName.c_str());
    call_homeassistant_service("media_player.media_play_pause", {
      {"entity_id", playerName},
    });
  }

  void nextTrack() {
    ESP_LOGD("speaker", "%s next track", playerName.c_str());
    call_homeassistant_service("media_player.media_next_track", {
      {"entity_id", playerName},
    });
  }
};

class SonosSpeakerComponent : public BasePlayerComponent {
 public:
  double localVolume = -1;
  double speaker_volume = -1;
  std::string shuffle = "";
  std::string muted = "";
  std::vector<std::string> groupMembers;

  void setup(std::string newSpeakerName) {
    // This will be called once to set up the component
    // think of it as the setup() call in Arduino
    // pinMode(6, OUTPUT);
    ESP_LOGD("speaker", "Sonos Speaker subbed %s", newSpeakerName.c_str());
    playerType = "Speaker";
    superSetup(newSpeakerName);
    subscribe_homeassistant_state(&SonosSpeakerComponent::speaker_volume_changed, newSpeakerName, "volume_level");
    subscribe_homeassistant_state(&SonosSpeakerComponent::speaker_muted_changed, newSpeakerName, "is_volume_muted");
    subscribe_homeassistant_state(&SonosSpeakerComponent::shuffle_changed, newSpeakerName, "shuffle");
    subscribe_homeassistant_state(&SonosSpeakerComponent::group_members_changed, newSpeakerName, "group_members");
    subscribe_homeassistant_state(&SonosSpeakerComponent::player_media_title_changed, newSpeakerName, "media_title");
    subscribe_homeassistant_state(&SonosSpeakerComponent::player_media_artist_changed, newSpeakerName, "media_artist");
  }

  void player_media_title_changed(std::string state) {
    ESP_LOGD("Player", "%s Player media title changed to %s", playerName.c_str(), state.c_str());
    mediaTitle = state.c_str();
    updateDisplay(false);
  }

  void player_media_artist_changed(std::string state) {
    ESP_LOGD("Player", "%s Player artist changed to %s", playerName.c_str(), state.c_str());
    mediaArtist = state.c_str();
    updateDisplay(false);
  }

  void speaker_volume_changed(std::string state) {
    ESP_LOGD("speaker", "%s Sonos Speaker volume changed to %s", playerName.c_str(), state.c_str());
    speaker_volume =  atof(state.c_str());
    if(localVolume == -1) {
      localVolume = atof(state.c_str());
    }
    updateDisplay(false);
  }

  void speaker_muted_changed(std::string state) {
    ESP_LOGD("speaker", "%s Sonos Speaker muted changed to %s", playerName.c_str(), state.c_str());
    muted = state.c_str();
    updateDisplay(false);
  }

  void shuffle_changed(std::string state) {
    ESP_LOGD("speaker", "%s Sonos Speaker shuffle changed to %s", playerName.c_str(), state.c_str());
    shuffle = state.c_str();
    updateDisplay(false);
  }

  void group_members_changed(std::string state) {
    std::vector<std::string> out;
    tokenize(state, ",", out);
    groupMembers.clear();
    for (auto &state: out) {
      std::string newGroupedSpeaker = filter(state);
      ESP_LOGD("speaker", "%s group %s", playerName.c_str(), newGroupedSpeaker.c_str());
      if(newGroupedSpeaker != playerName) {
        groupMembers.push_back(filter(state));
      }
    }
    updateDisplay(false);
  }

  void ungroup() {
    ESP_LOGD("speaker", "%s ungroup speaker", playerName.c_str());
    call_homeassistant_service("media_player.unjoin", {
      {"entity_id", playerName},
    });
    updateDisplay(false);
  }

  void joinGroup(std::string newSpeakerName) {
    ESP_LOGD("speaker", "%s group speaker to %s", playerName.c_str(), newSpeakerName.c_str());
    call_homeassistant_service("media_player.join", {
      {"entity_id", newSpeakerName.c_str()},
      {"group_members", playerName.c_str()},
    });
    updateDisplay(false);
  }

  void toggleShuffle() {
    ESP_LOGD("speaker", "%s toggle shuffle", playerName.c_str());
    call_homeassistant_service("media_player.shuffle_set", {
      {"entity_id", playerName},
      {"shuffle", shuffle == "on" ? "false" : "true"},
    });
  }

  double volumeStep = 0.04;

  void increaseVolume() {
    if(speaker_volume == -1) {
      return;
    }
    optionMenu = volumeOptionMenu;
    localVolume = localVolume + volumeStep;
    updateVolumeLevel();
  }

  void decreaseVolume() {
    if(speaker_volume == -1) {
      return;
    }
    optionMenu = volumeOptionMenu;
    localVolume = localVolume - volumeStep;
    //updateVolumeLevel();
  }

  void updateVolumeLevel() {
    std::string entityIds = playerName;
    for (auto &speaker: groupMembers) {
      entityIds = entityIds + ", " + speaker;
    }
    ESP_LOGD("speaker", "%s volume update %f", entityIds.c_str(), localVolume);
    call_homeassistant_service("media_player.volume_set", {
      {"entity_id", entityIds},
      {"volume_level", to_string(localVolume)},
    });
  }
};

class TVPlayerComponent : public BasePlayerComponent {
 public:
  SonosSpeakerComponent *speaker;

  void setup(std::string newTVName) {
    ESP_LOGD("PlayerTV", "TV subbed %s", newTVName.c_str());
    playerType = "TV";
    superSetup(newTVName);
    subscribe_homeassistant_state(&TVPlayerComponent::player_media_artist_changed, newTVName, "source");
    subscribe_homeassistant_state(&TVPlayerComponent::player_source_list_changed, newTVName, "source_list");
  }

  void player_media_artist_changed(std::string state) {
    ESP_LOGD("PlayerTV", "%s Player source changed to %s", playerName.c_str(), state.c_str());
    mediaArtist = state.c_str();
    updateDisplay(false);
  }

  void player_source_list_changed(std::string state) {
    ESP_LOGD("PlayerTV", "%s Player source list changed to %s", playerName.c_str(), state.c_str());
    std::vector<std::string> out;
    tokenize(state, ",", out);
    sources.clear();
    for (auto &state: out) {
      std::string source = filter(state);
      sources.push_back(source);
      ESP_LOGD("Player", "%s state %s", playerName.c_str(), source.c_str());
    }
    updateDisplay(false);
  }

  void tvRemoteCommand(std::string command) {
    std::string remoteName = playerName.substr(12).insert(0, "remote");
    ESP_LOGD("PlayerTV", "remote %s, %s", command.c_str(), remoteName.c_str());
    call_homeassistant_service("remote.send_command", {
      {"entity_id", remoteName},
      {"command", command.c_str()},
    });
  }
};

class SonosSpeakerGroupComponent : public CustomAPIDevice, public Component {
 public:
  BasePlayerComponent *activePlayer;
  std::vector<SonosSpeakerComponent*> speakers;
  TVPlayerComponent* tv;
  bool playerSearchFinished = false;

  void findActivePlayer() {
    if (playerSearchFinished) {
      return;
    }
    if (tv->playerState == "playing") {
      playerSearchFinished = true;
      return;
    }
    for (auto &speaker: speakers) {
      if (speaker->playerState == "") {
        return;
      } else if (speaker->playerState == "playing" && speaker->mediaTitle != "") {
        if(speaker->mediaTitle == "TV") {
          ESP_LOGD("activePlayer", "tv");
          activePlayer = tv;
        } else {
          ESP_LOGD("activePlayer", "speaker %s", speaker->mediaTitle.c_str());
          activePlayer = speaker;
        }
        playerSearchFinished = true;
        updateDisplay(true);
      }
    }
    playerSearchFinished = true;
    updateDisplay(true);
  }

  void setup(std::vector<std::string> speakerNames, std::string tvName) {
    bool first = true;
    playerSearchFinished = false;

    auto newTV = new TVPlayerComponent();
    newTV->setup(tvName);
    tv = newTV;
    activePlayer = tv;

    for (auto &name: speakerNames) {
      ESP_LOGD("speakerGroup", "New speaker %s", name.c_str());
      auto newSpeaker = new SonosSpeakerComponent();
      newSpeaker->setup(name);
      speakers.push_back(newSpeaker);
      if(name == "media_player.beam") {
        tv->speaker = newSpeaker;
      }
    }

    subscribe_homeassistant_state(&SonosSpeakerGroupComponent::on_state_changed, "sensor.sonos_favorites", "items");
  }

  std::vector<std::string> groupNames() {
    std::vector<std::string> output;
    for (auto &speaker: speakers) {
      output.push_back(speaker->friendlyName);
    }
    return output;
  }

  SonosSpeakerComponent* speakerForFriendlyName(std::string speakerFriendlyName) {
    for (auto &speaker: speakers) {
      if (speaker->friendlyName == speakerFriendlyName) {
        return speaker;
      }
    }
  }

  void on_state_changed(std::string state) {
    ESP_LOGD("group", "Sonos Favorites changes to %s", state.c_str());
    std::string delim = ", '";
 
    // const std::regex re(R"(('?,?[{ ]?'FV(.*?) ')|'[}])");
    std::vector<std::string> out;
    std::vector<std::string> sources;
    tokenize(state, delim, out);

    for (auto &state: out) {
        std::string source = filter(state);
        std::string outSource = source.substr(source.find(" ") + 1);
        ESP_LOGD("group", "%s", outSource.c_str());
        sources.push_back(outSource);
    }

    for(auto &player: speakers) {
      player->sources = sources;
    }
  }

  void incraseSpeakerVolume() {
    SonosSpeakerComponent* activeSpeaker = static_cast<SonosSpeakerComponent*>(activePlayer);
    if (activeSpeaker != NULL) {
      activeSpeaker->increaseVolume();
    }
  }

  void decreaseSpeakerVolume() {
    SonosSpeakerComponent* activeSpeaker = static_cast<SonosSpeakerComponent*>(activePlayer);
    if (activeSpeaker != NULL) {
      activeSpeaker->decreaseVolume();
    }
  }
};

auto *speakerGroup = new SonosSpeakerGroupComponent();

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

auto *sceneGroup = new SceneGroupComponent();

std::string sourceTitleString(int x) {
  if (x < speakerGroup->activePlayer->sources.size()) {
    return speakerGroup->activePlayer->sources[x];
  } else {
    return "-";
  }
}

std::string playString() {
    if (speakerGroup->activePlayer->playerState == "playing") {
      return "Pause";
    } else {
      return "Play";
    }
}

bool mediaShuffling() {
  BasePlayerComponent* activePlayer = speakerGroup->activePlayer;
  SonosSpeakerComponent* activeSpeaker = static_cast<SonosSpeakerComponent*>(activePlayer);
  if (activeSpeaker != NULL) {
    if (activeSpeaker->shuffle == "on") {
      return true;
    } else {
      return false;
    }
  }
  return false;
}

void toggleShuffle() {
  BasePlayerComponent* activePlayer = speakerGroup->activePlayer;
  SonosSpeakerComponent* activeSpeaker = static_cast<SonosSpeakerComponent*>(activePlayer);
  if (activeSpeaker != NULL) {
    activeSpeaker->toggleShuffle();
  }
}

std::string shuffleString() {
    if (mediaShuffling()) {
      return "Shfl On";
    } else {
      return "Shfl Off";
    }
}

std::string menuStringForType(MenuStringType stringType) {
  switch(stringType) {
  case nowPlaying:
    return "Now Playing";
  case sources:
    return "Sources";
  case group:
    return "Group";
  case shuffle:
    return shuffleString();
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

std::string newSpeakerGroupMasterName = "";

int batteryWidth = 24;

void drawPlayPauseIcon() {
  int yPos = 2;
  int xPos = 4;
  if (speakerGroup->activePlayer->playerState == "playing") {
    id(my_display).image(id(my_display).get_width() - xPos - batteryWidth - 16, 2, &id(image_play));
  } else if (speakerGroup->activePlayer->playerState == "paused") {
    id(my_display).image(id(my_display).get_width() - xPos - batteryWidth - 16, 2, &id(image_pause));
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
  // it.printf(it.get_width() - 4, 0, id(helvetica_8), id(my_green), TextAlign::TOP_RIGHT, "%.1f%%", id(TTGoBatteryPercent).state);
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

double getVolumeLevel() {
  if (speakerGroup->activePlayer->playerType == "TV") {
    if (speakerGroup->tv->speaker->speaker_volume != -1) {
      double volume = speakerGroup->tv->speaker->localVolume * 100;
      return volume;
    }
  } else {
    SonosSpeakerComponent* activeSpeaker = static_cast<SonosSpeakerComponent*>(speakerGroup->activePlayer);
    if (activeSpeaker != NULL) {
      if (activeSpeaker->speaker_volume != -1) {
        double volume = activeSpeaker->localVolume * 100;
        return volume;
      }
    }
  }
  return 0;
}

void drawVolumeLevel() {
  int xPos = id(my_display).get_width() - batteryWidth - 24;
  id(my_display).printf(xPos, 0, &id(helvetica_8), id(my_white), TextAlign::TOP_RIGHT, "%.0f%%", getVolumeLevel());
}

void drawHeader() {
  id(my_display).rectangle(0, 16, id(my_display).get_width(), 1, id(my_blue));
  drawBattery();
  drawCurrentMediaPlayer();
  drawPlayPauseIcon();
  drawVolumeLevel();
}

void drawTitle(int menuState, int i, std::string title, int yPos) {
    if(menuState == i) {
        // ESP_LOGD("custom", "Title %d %s", i, title.c_str());
        id(my_display).filled_rectangle(0, yPos, id(my_display).get_width() - 4, fontSize + marginSize, id(my_blue));
        id(my_display).printf(4, yPos, &id(helvetica_12), id(my_white), TextAlign::TOP_LEFT, "%s", title.c_str());
    } else {
        id(my_display).printf(4, yPos, &id(helvetica_12), id(my_white), TextAlign::TOP_LEFT, "%s", title.c_str());
    }
}

void drawScrollBar(int menuTitlesCount, int maxItems, double headerHeight) {
  if(menuTitlesCount > maxItems + 1) {
    double screenHeight = id(my_display).get_height() - headerHeight;
    double height = maxItems * (screenHeight / menuTitlesCount);
    double yPos = (((screenHeight - height) / (menuTitlesCount - 1)) * menuIndex) + 1 + headerHeight;
    id(my_display).filled_rectangle(id(my_display).get_width() - 3, yPos, 2, height - 1, id(my_green));
  }
}

void drawMenu(std::vector<std::string> menuTitles) {
  double headerHeight = 16;
  int maxItems = ((id(my_display).get_height() - headerHeight) / (fontSize + marginSize)) - 1;
  int menuState = menuIndex;

  if (menuState - maxItems > scrollTop) {
      scrollTop = menuState - maxItems;
      // ESP_LOGD("scroll", "DOWN");
  } else if (menuState - scrollTop < 0) {
      scrollTop = menuState;
      // ESP_LOGD("scroll", "UP");
  }
  // ESP_LOGD("custom", "menuState %d scrollTop %d", menuState, scrollTop);
  for(int i = scrollTop; i < menuTitles.size(); i++) {
      if(i > scrollTop + maxItems) {
          break;
      }
      drawTitle(menuState, i, menuTitles[i], ((i - scrollTop) * (fontSize + marginSize)) + headerHeight);
  }
  drawScrollBar(menuTitles.size(), maxItems, headerHeight);
}

void drawNowPlaying() {
  int yPos = 200;
  if (speakerGroup->activePlayer->mediaTitle != "") {
    id(my_display).printf(id(my_display).get_width() / 2, yPos, &id(helvetica_8), id(my_white), TextAlign::TOP_CENTER, "%s", speakerGroup->activePlayer->mediaTitle.c_str());     
  }
  
  if (speakerGroup->activePlayer->mediaArtist != "") {
    id(my_display).printf(id(my_display).get_width() / 2, yPos + 12 + marginSize, &id(helvetica_8), id(my_white), TextAlign::TOP_CENTER, "%s", speakerGroup->activePlayer->mediaArtist.c_str());     
  }
}

std::vector<std::string> menuToString(std::vector<MenuStringType> menu) {
  std::vector<std::string> out;
  for (auto &menuItem: menu) {
      out.push_back(menuStringForType(menuItem));
  }
  return out;
}

std::vector<std::string> groupTitleString() {
  std::vector<std::string> out;
  for (auto &speaker: speakerGroup->speakers) {
    std::string speakerPlaying = speaker->playerState == "playing" ? " > " : "";
    if (speaker->groupMembers.size() > 0) {
      out.push_back(speaker->friendlyName + speakerPlaying + " +"); 
    } else if (newSpeakerGroupMasterName == speaker->playerName) {
      out.push_back(speaker->friendlyName + speakerPlaying + " +...");
    } else {
      out.push_back(speaker->friendlyName + speakerPlaying);
    }
  }
  return out;
}

std::vector<std::string> mediaPlayersTitleString() {
  std::vector<std::string> out;
  for (auto &speaker: speakerGroup->speakers) {
    std::string speakerPlaying = speaker->playerState == "playing" ? " > " : "";
    out.push_back(speaker->friendlyName + speakerPlaying);
  }
  std::string tvPlaying = speakerGroup->tv->playerState == "playing" ? " > " : "";
  out.push_back(speakerGroup->tv->friendlyName + tvPlaying);
  return out;
}

std::vector<MenuStringType> rootMenuTitles() {
  if (speakerGroup->activePlayer->playerType == "TV") {
    return { nowPlaying, sources, mediaPlayers, lights, sleepString };
  } else {
    return { nowPlaying, sources, group, shuffle, mediaPlayers, lights, sleepString };
  }
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
    case groupMenu:
      return groupTitleString();
    case mediaPlayersMenu:
      return mediaPlayersTitleString();
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
  activeMenuState = MenuStates::rootMenu;
}

void idleMenu() {
  menuIndex = 0;
  optionMenu = noOptionMenu;
  if (speakerGroup->activePlayer->playerType == "TV") {
    activeMenuState = MenuStates::tvNowPlayingMenu;
  } else {
    activeMenuState = MenuStates::speakerNowPlayingMenu;
  }
}

void selectGroup() {
  auto speaker = speakerGroup->speakers[menuIndex];
  if (speaker->groupMembers.size() > 0) {
    speaker->ungroup();
  } else if (newSpeakerGroupMasterName == "") {
    for (auto &speakerGroupSpeaker: speakerGroup->speakers) {
      if (speakerGroupSpeaker->groupMembers.size() > 0) {
        speaker->joinGroup(speakerGroupSpeaker->playerName);
        return;
      }
    }
    newSpeakerGroupMasterName = speaker->playerName;
  } else {
    speaker->joinGroup(newSpeakerGroupMasterName);
    newSpeakerGroupMasterName = "";
  }
}

void idleTick() {
  // ESP_LOGD("idle", "idle time %d", id(idle_time));
  if(id(idle_time) == 5) {
    optionMenu = noOptionMenu;
    updateDisplay(true);
  } else if(id(idle_time) == 20) {
    if(speakerGroup->playerSearchFinished) {
      idleMenu();
      updateDisplay(true);
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

void drawOptionMenu() {
  switch(optionMenu) {
    case tvOptionMenu:
      id(my_display).printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.2 + 16, &id(helvetica_8), id(my_white), TextAlign::TOP_CENTER, "Remote Menu");     
      id(my_display).printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.75 + 16, &id(helvetica_8), id(my_white), TextAlign::TOP_CENTER, "Pause");     
      id(my_display).printf(id(my_display).get_width() * 0.2, (id(my_display).get_height() - 16) * 0.5 + 16, &id(helvetica_8), id(my_white), TextAlign::TOP_CENTER, "Back");     
      id(my_display).printf(id(my_display).get_width() * 0.8, (id(my_display).get_height() - 16) * 0.5 + 16, &id(helvetica_8), id(my_white), TextAlign::TOP_CENTER, "Home");     
      id(my_display).printf(id(my_display).get_width() * 0.5, (id(my_display).get_height() - 16) * 0.45 + 16, &id(helvetica_8), id(my_white), TextAlign::TOP_CENTER, "TV Power");     
      return;
    case volumeOptionMenu:
      id(my_display).rectangle(16, id(my_display).get_height() - 16, id(my_display).get_width() - 32, 10, id(my_blue));
      id(my_display).filled_rectangle(18, id(my_display).get_height() - 14, (id(my_display).get_width() - 36) * (getVolumeLevel() / 100), 6, id(my_blue));
      return;
    case noOptionMenu:
      return;
  }
}

void drawTVNowPlaying() {
  int yPos = 20;
  drawOptionMenu();
  if (speakerGroup->activePlayer->mediaArtist != "") {
    id(my_display).printf(40, yPos + 32 + marginSize, &id(helvetica_24), id(my_white), "%s", speakerGroup->activePlayer->mediaArtist.c_str());     
  }
}

void drawSpeakerNowPlaying() {
  int yPos = 20;
  drawOptionMenu();
  if (speakerGroup->activePlayer->mediaArtist != "") {
    id(my_display).printf(8, yPos + marginSize, &id(helvetica_12), id(my_white), "%s", speakerGroup->activePlayer->mediaTitle.c_str());     
  }
  if (speakerGroup->activePlayer->mediaArtist != "") {
    id(my_display).printf(8, yPos + 16 + marginSize, &id(helvetica_24), id(my_white), "%s", speakerGroup->activePlayer->mediaArtist.c_str());     
  }
}

void drawMenu() {
  if(speakerGroup->playerSearchFinished == false) {
    ESP_LOGD("drawMenu", "draw beep boop");
    // if(id(my_display).get_update_interval() != 1000) {
    //   id(my_display).set_update_interval(1000);
    // }
    id(my_display).printf(40, 40, &id(helvetica_24), id(my_blue), TextAlign::TOP_LEFT, "beep boop");
    speakerGroup->findActivePlayer();
    return;
  // } else if(id(my_display).get_update_interval() != 50) {
  //   id(my_display).set_update_interval(50);
  }
  switch(activeMenuState) {
    case tvNowPlayingMenu:
      drawTVNowPlaying();
      break;
    case speakerNowPlayingMenu:
      drawSpeakerNowPlaying();
      break;
    case sourcesMenu:
      drawMenu(activeMenu());
      break;
    default:
      drawNowPlaying();
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
    case group:
      activeMenuState = groupMenu;
      break;
    case shuffle:
      toggleShuffle();
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
      speakerGroup->activePlayer->playSource(menuIndex);
      idleMenu();
      break;
    case groupMenu:
      selectGroup();
      break;
    case mediaPlayersMenu:
      selectMediaPlayers();
      break;
    case lightsMenu:
      if (sceneGroup->selectLights(menuIndex)) {
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
        updateDisplay(true);
      } else {
        speakerGroup->tv->tvRemoteCommand("select");
      }
      return;
    default:
      break;
  }
  if (selectMenu()) {
    updateDisplay(true);
  }
}

void buttonPressNext() {
  switch(activeMenuState) {
    case tvNowPlayingMenu:
      speakerGroup->tv->speaker->increaseVolume();
      debounceUpdateDisplay();
      return;
    case speakerNowPlayingMenu:
      speakerGroup->incraseSpeakerVolume();
      debounceUpdateDisplay();
      return;
    default:
      break;
  }
  auto activeMenuTitles = activeMenu();
  if (menuIndex < activeMenuTitles.size()-1) {
    menuIndex++;
  } else if (menuIndex == activeMenuTitles.size()-1) {
    // menuIndex = 0;
  } else {
    menuIndex = 0;
  }
  debounceUpdateDisplay();
}

void buttonPressPrevious() {
  switch(activeMenuState) {
    case tvNowPlayingMenu:
      speakerGroup->tv->speaker->decreaseVolume();
      debounceUpdateDisplay();
      return;
    case speakerNowPlayingMenu:
      speakerGroup->decreaseSpeakerVolume();
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
        updateDisplay(true);
      } else {
        speakerGroup->tv->tvRemoteCommand("up");
      }
      break;
    default:
      topMenu();
      updateDisplay(true);
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
        updateDisplay(true);
      } else {
        speakerGroup->tv->tvRemoteCommand("down");
      }
      break;
    case speakerNowPlayingMenu:
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
        updateDisplay(true);
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
        updateDisplay(true);
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
      updateDisplay(true);
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
      updateDisplay(true);
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
      updateDisplay(true);
      break;
    default:
      topMenu();
      updateDisplay(true);
      break;
  }
}