#include "esphome.h"
#include "DisplayUpdateInterface.h"
#include "MenuTitle.h"

#ifndef REMOTEPLAYERS
#define REMOTEPLAYERS

std::string playingNewSourceText = "";

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
    if(i < str.size() - 3 && str[i] == '\\' && str[i+1] == 'x' && str[i+2] == 'a') {
      // replace \xa with space
      output += ' ';
      i+=3;
      continue;
    }
    if(i == str.size() - 1 && str[i] == '}') return output;
    if(str[i] != '[' && str[i] != ']' && str[i] != '\'' && str[i] != '"') output += str[i];
  }
  return output;
}

class BasePlayerComponent : public CustomAPIDevice, public Component {
  public:
    BasePlayerComponent(DisplayUpdateInterface& newCallback) : display(newCallback) { }
    std::string playerState = "";
    std::string mediaTitle = "";
    std::string mediaArtist = "";
    std::string friendlyName = "";
    std::string entityId = "";
    std::string playerType = "";
    std::vector<MenuTitle> sources;

  void superSetup(std::string newentityId) {
    entityId = newentityId;
    ESP_LOGD("Player", "Player subbed %s", entityId.c_str());
    subscribe_homeassistant_state(&BasePlayerComponent::playerState_changed, newentityId.c_str());
    subscribe_homeassistant_state(&BasePlayerComponent::player_name_changed, newentityId, "friendly_name");
  }

  void playerState_changed(std::string state) {
    ESP_LOGD("Player", "%s state changed to %s", entityId.c_str(), state.c_str());
    playerState = state.c_str();
    display.updateDisplay(false);
  }

  void player_name_changed(std::string state) {
    ESP_LOGD("Player", "%s name changed to %s", entityId.c_str(), state.c_str());
    friendlyName = state.c_str();
    display.updateDisplay(false);
  }

  void playSource(float index) {
    ESP_LOGD("speaker", "%s playing %f %s", entityId.c_str(), index, sources[index].friendlyName.c_str());
    playingNewSourceText = sources[index].friendlyName;
    call_homeassistant_service("media_player.select_source", {
      {"entity_id", entityId},
      {"source", sources[index].friendlyName.c_str()},
    });
  }

  void playPause() {
    ESP_LOGD("speaker", "%s play pause", entityId.c_str());
    call_homeassistant_service("media_player.media_play_pause", {
      {"entity_id", entityId},
    });
  }

  void nextTrack() {
    ESP_LOGD("speaker", "%s next track", entityId.c_str());
    call_homeassistant_service("media_player.media_next_track", {
      {"entity_id", entityId},
    });
  }

  MenuTitleState menuTitlePlayerState() {
    MenuTitleState titleState = StoppedMenuTitleState;
    if(playerState == "playing") {
      titleState = PlayingMenuTitleState;
    } else if (playerState == "paused") {
      titleState = PausedMenuTitleState;
    }
    return titleState;
  }

  protected:
  DisplayUpdateInterface& display;
};

class SonosSpeakerComponent : public BasePlayerComponent {
 public:
  SonosSpeakerComponent(DisplayUpdateInterface& newCallback) : BasePlayerComponent { newCallback } { }
  double localVolume = -1;
  double speaker_volume = -1;
  std::string shuffle = "";
  std::string muted = "";
  std::vector<std::string> groupMembers;

  void setup(std::string newSpeakerName) {
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
    ESP_LOGD("Player", "%s Player media title changed to %s", entityId.c_str(), state.c_str());
    mediaTitle = state.c_str();
    display.updateDisplay(false);
  }

  void player_media_artist_changed(std::string state) {
    ESP_LOGD("Player", "%s Player artist changed to %s", entityId.c_str(), state.c_str());
    mediaArtist = state.c_str();
    display.updateDisplay(false);
  }

  void speaker_volume_changed(std::string state) {
    ESP_LOGD("speaker", "%s Sonos Speaker volume changed to %s", entityId.c_str(), state.c_str());
    speaker_volume =  atof(state.c_str());
    if(localVolume == -1) {
      localVolume = atof(state.c_str());
    }
    display.updateDisplay(false);
  }

  void speaker_muted_changed(std::string state) {
    ESP_LOGD("speaker", "%s Sonos Speaker muted changed to %s", entityId.c_str(), state.c_str());
    muted = state.c_str();
    display.updateDisplay(false);
  }

  void shuffle_changed(std::string state) {
    ESP_LOGD("speaker", "%s Sonos Speaker shuffle changed to %s", entityId.c_str(), state.c_str());
    shuffle = state.c_str();
    display.updateDisplay(false);
  }

  void group_members_changed(std::string state) {
    std::vector<std::string> out;
    tokenize(state, ",", out);
    groupMembers.clear();
    for (auto &state: out) {
      std::string newGroupedSpeaker = filter(state);
      ESP_LOGD("speaker", "%s group %s", entityId.c_str(), newGroupedSpeaker.c_str());
      if(newGroupedSpeaker != entityId) {
        groupMembers.push_back(filter(state));
      }
    }
    display.updateDisplay(false);
  }

  void ungroup() {
    ESP_LOGD("speaker", "%s ungroup speaker", entityId.c_str());
    call_homeassistant_service("media_player.unjoin", {
      {"entity_id", entityId},
    });
    display.updateDisplay(false);
  }

  void joinGroup(std::string newSpeakerName) {
    ESP_LOGD("speaker", "%s group speaker to %s", entityId.c_str(), newSpeakerName.c_str());
    call_homeassistant_service("media_player.join", {
      {"entity_id", newSpeakerName.c_str()},
      {"group_members", entityId.c_str()},
    });
    display.updateDisplay(false);
  }

  void toggleShuffle() {
    ESP_LOGD("speaker", "%s toggle shuffle", entityId.c_str());
    call_homeassistant_service("media_player.shuffle_set", {
      {"entity_id", entityId},
      {"shuffle", shuffle == "on" ? "false" : "true"},
    });
  }

  double volumeStep = 0.04;

  void increaseVolume() {
    if(speaker_volume == -1) {
      return;
    }
    localVolume = localVolume + volumeStep;
    updateVolumeLevel();
  }

  void decreaseVolume() {
    if(speaker_volume == -1) {
      return;
    }
    localVolume = localVolume - volumeStep;
    updateVolumeLevel();
  }

  void updateVolumeLevel() {
    std::string entityIds = entityId;
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
  TVPlayerComponent(DisplayUpdateInterface& newCallback) : BasePlayerComponent { newCallback } { }
  SonosSpeakerComponent *speaker;

  void setup(std::string newTVName) {
    ESP_LOGD("PlayerTV", "TV subbed %s", newTVName.c_str());
    playerType = "TV";
    superSetup(newTVName);
    subscribe_homeassistant_state(&TVPlayerComponent::player_media_artist_changed, newTVName, "source");
    subscribe_homeassistant_state(&TVPlayerComponent::player_source_list_changed, newTVName, "source_list");
  }

  void player_media_artist_changed(std::string state) {
    ESP_LOGD("PlayerTV", "%s Player source changed to %s", entityId.c_str(), state.c_str());
    mediaArtist = state.c_str();
    display.updateDisplay(false);
  }

  void player_source_list_changed(std::string state) {
    ESP_LOGD("PlayerTV", "%s Player source list changed to %s", entityId.c_str(), state.c_str());
    std::vector<std::string> out;
    tokenize(state, ",", out);
    sources.clear();
    for (auto &state: out) {
      std::string source = filter(state);
      sources.push_back(MenuTitle(source, "", NoMenuTitleState));
      ESP_LOGD("Player", "%s state %s", entityId.c_str(), source.c_str());
    }
    display.updateDisplay(false);
  }

  void tvRemoteCommand(std::string command) {
    std::string remoteName = entityId.substr(12).insert(0, "remote");
    ESP_LOGD("PlayerTV", "remote %s, %s", command.c_str(), remoteName.c_str());
    call_homeassistant_service("remote.send_command", {
      {"entity_id", remoteName},
      {"command", command.c_str()},
    });
  }
};

class SonosSpeakerGroupComponent : public CustomAPIDevice, public Component {
 public:
  SonosSpeakerGroupComponent(DisplayUpdateInterface& newCallback) : display(newCallback) { }

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
        display.updateDisplay(true);
      }
    }
    playerSearchFinished = true;
    display.updateDisplay(true);
  }

  void setup(std::vector<std::string> speakerNames, std::string tvName) {
    bool first = true;
    playerSearchFinished = false;

    auto newTV = new TVPlayerComponent(display);
    newTV->setup(tvName);
    tv = newTV;
    activePlayer = tv;

    for (auto &name: speakerNames) {
      ESP_LOGD("speakerGroup", "New speaker %s", name.c_str());
      auto newSpeaker = new SonosSpeakerComponent(display);
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

  std::string friendlyNameForentityId(std::string speakerentityId) {
    for (auto &speaker: speakers) {
      if (speaker->entityId == speakerentityId) {
        return speaker->friendlyName;
      }
    }
    return "";
  }

  void on_state_changed(std::string state) {
    ESP_LOGD("group", "Sonos Favorites changes to %s", state.c_str());
    std::string delim = ", '";
 
    std::vector<std::string> out;
    tokenize(state, delim, out);

    std::vector<MenuTitle> sources;
    for (auto &state: out) {
      std::string source = filter(state);
      std::string outSource = source.substr(source.find(" ") + 1);
      ESP_LOGD("group", "%s", outSource.c_str());
      sources.push_back(MenuTitle(outSource, "", NoMenuTitleState));
    }

    for(auto &player: speakers) {
      player->sources = sources;
    }
  }

  void increaseSpeakerVolume() {
    if(activePlayer->playerType != "TV") {
      SonosSpeakerComponent* activeSpeaker = static_cast<SonosSpeakerComponent*>(activePlayer);
      if (activeSpeaker != NULL) {
        activeSpeaker->increaseVolume();
      }
    } else {
      tv->speaker->increaseVolume();
    }
  }

  void decreaseSpeakerVolume() {
    if(activePlayer->playerType != "TV") {
      SonosSpeakerComponent* activeSpeaker = static_cast<SonosSpeakerComponent*>(activePlayer);
      if (activeSpeaker != NULL) {
        activeSpeaker->decreaseVolume();
      }
    } else {
      tv->speaker->decreaseVolume();
    }
  }

  std::string playString() {
      if (activePlayer->playerState == "playing") {
        return "Pause";
      } else {
        return "Play";
      }
  }

  bool mediaShuffling() {
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

  MenuTitle *newSpeakerGroupParent = NULL;

  double getVolumeLevel() {
    if (activePlayer->playerType == "TV") {
      if (tv->speaker->speaker_volume != -1) {
        double volume = tv->speaker->localVolume * 100;
        return volume;
      }
    } else {
      SonosSpeakerComponent* activeSpeaker = static_cast<SonosSpeakerComponent*>(activePlayer);
      if (activeSpeaker != NULL) {
        if (activeSpeaker->speaker_volume != -1) {
          double volume = activeSpeaker->localVolume * 100;
          return volume;
        }
      }
    }
    return 0;
  }

  std::vector<MenuTitle> groupTitleSwitches() {
    std::vector<MenuTitle> out;
    std::vector<std::string> groupedMembers;
    out.push_back(MenuTitle("Group " + newSpeakerGroupParent->friendlyName, newSpeakerGroupParent->entityId, ArrowMenuTitleState));

    for (auto &speaker: speakers) {
      if (newSpeakerGroupParent->entityId == speaker->entityId) {
        continue;
      } else {
        if (std::find(speaker->groupMembers.begin(), speaker->groupMembers.end(), newSpeakerGroupParent->entityId) != speaker->groupMembers.end()) {
          out.push_back(MenuTitle(speaker->friendlyName, speaker->entityId, OnMenuTitleState));
        } else {
          out.push_back(MenuTitle(speaker->friendlyName, speaker->entityId, OffMenuTitleState));
        }
      }
    }
    return out;
  }

  std::vector<MenuTitle> groupTitleString() {
    std::vector<MenuTitle> out;
    std::vector<std::string> groupedMembers;
    for (auto &speaker: speakers) {
      if (std::find(groupedMembers.begin(), groupedMembers.end(), speaker->entityId) != groupedMembers.end()) {
        // skip grouped members that were already found
        continue;
      }
      if (speaker->groupMembers.size() > 0) {
        out.push_back(MenuTitle(speaker->friendlyName, speaker->entityId, speaker->menuTitlePlayerState()));
        for (auto &groupedSpeaker: speaker->groupMembers) {
          groupedMembers.push_back(groupedSpeaker);
          std::string groupedSpeakerName = friendlyNameForentityId(groupedSpeaker);
          out.push_back(MenuTitle(groupedSpeakerName, groupedSpeaker, GroupedMenuTitleState));
        }
      } else {
        out.push_back(MenuTitle(speaker->friendlyName, speaker->entityId, speaker->menuTitlePlayerState()));
      }
    }
    return out;
  }

  std::vector<MenuTitle> mediaPlayersTitleString() {
    std::vector<MenuTitle> out;
    std::vector<MenuTitle> groupedTVSpeakers;
    for (auto &speaker: speakers) {
      if(speaker->mediaTitle != "TV") {
        out.push_back(MenuTitle(speaker->friendlyName, speaker->entityId, speaker->menuTitlePlayerState()));
      } else {
        groupedTVSpeakers.push_back(MenuTitle(speaker->friendlyName, speaker->entityId, GroupedMenuTitleState));
      }
    }
    out.push_back(MenuTitle(tv->friendlyName, tv->entityId, tv->menuTitlePlayerState()));
    for (auto &groupedTVSpeaker: groupedTVSpeakers) {
      out.push_back(groupedTVSpeaker);
    }
    return out;
  }

  void selectGroup(MenuTitle *selectedMenuTitle) {
    if (newSpeakerGroupParent == NULL) {
      MenuTitle *selectedMenuTitleCopy = new MenuTitle(selectedMenuTitle->friendlyName, selectedMenuTitle->entityId, selectedMenuTitle->titleState);
      newSpeakerGroupParent = selectedMenuTitleCopy;
      return;
    } else if (menuIndex == 0) {
      newSpeakerGroupParent = NULL;
      return;
    }

    std::vector<SonosSpeakerComponent*> speakerList;
    for (auto &speaker: speakers) {
      if(speaker->entityId == newSpeakerGroupParent->entityId) {
        continue;
      }
      speakerList.push_back(speaker);
    }
    auto speaker = speakerList[menuIndex-1];

    if (speaker->groupMembers.size() > 0) {
      if (std::find(speaker->groupMembers.begin(), speaker->groupMembers.end(), newSpeakerGroupParent->entityId) != speaker->groupMembers.end()) {
        speaker->ungroup();
      }
    } else {
      speaker->joinGroup(newSpeakerGroupParent->entityId);
    }
  }

  private:
  DisplayUpdateInterface& display;
};

#endif