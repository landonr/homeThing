#include "esphome.h"
#include "DisplayUpdateInterface.h"
#include "MenuTitle.h"

#ifndef REMOTEPLAYERS
#define REMOTEPLAYERS

enum RemotePlayerType {
  TVRemotePlayerType,
  SpeakerRemotePlayerType
};

enum RemotePlayerState {
  NoRemoteState,
  PausedRemoteState,
  PlayingRemoteState,
  StoppedRemoteState,
  PowerOffRemoteState
};

std::string playingNewSourceText = "";

class SpeakerSetup
{
  public:
    SpeakerSetup(
      std::string newEntityId, 
      std::string newFriendlyName
    ) : entityId(newEntityId), friendlyName(newFriendlyName) { }
    std::string entityId;
    std::string friendlyName;
};

class TVSetup
{
  public:
    TVSetup(
      std::string newEntityId, 
      std::string newFriendlyName,
      SpeakerSetup* newSoundBar
    ) : entityId(newEntityId), friendlyName(newFriendlyName), soundBar(newSoundBar) { }
    std::string entityId;
    std::string friendlyName;
    SpeakerSetup* soundBar;
};


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
  protected:
  DisplayUpdateInterface& display;

  public:
    BasePlayerComponent(
      DisplayUpdateInterface& newCallback,
      int newIndex,
      std::string newEntityId,
      std::string newFriendlyName,
      RemotePlayerType newPlayerType
    ) : display(newCallback), index(newIndex), entityId(newEntityId), friendlyName(newFriendlyName), playerType(newPlayerType) { }
    std::string mediaTitle = "";
    std::string mediaArtist = "";
    RemotePlayerState playerState = NoRemoteState;
    std::vector<MenuTitle> sources;
    int index;
    std::string entityId;
    std::string friendlyName;
    RemotePlayerType playerType;

  void superSetup() {
    ESP_LOGI("Player", "Player subbed %s", entityId.c_str());
    subscribe_homeassistant_state(&BasePlayerComponent::playerState_changed, entityId.c_str());
  }

  void playerState_changed(std::string state) {
    ESP_LOGI("Player", "%s state changed to %s", entityId.c_str(), state.c_str());
    if(state.length() == 0) {
      playerState = StoppedRemoteState;
    } if(strcmp(state.c_str(), "playing") == 0) {
      playerState = PlayingRemoteState;
    } else if(strcmp(state.c_str(), "paused") == 0) {
      playerState = PausedRemoteState;
    } else if(strcmp(state.c_str(), "standby") == 0) {
      playerState = PowerOffRemoteState;
      mediaTitle = "";
      mediaArtist = "";
    } else if(strcmp(state.c_str(), "home") == 0) {
      playerState = StoppedRemoteState;
      mediaTitle = "";
      mediaArtist = "";
    } else if(strcmp(state.c_str(), "on") == 0) {
      playerState = StoppedRemoteState;
    } else if(strcmp(state.c_str(), "idle") == 0) {
      playerState = StoppedRemoteState;
    } else {
      playerState = NoRemoteState;
    }
    display.updateDisplay(false);
  }

  void playSource(float index) {
    ESP_LOGI("speaker", "%s playing %f %s", entityId.c_str(), index, sources[index].friendlyName.c_str());
    playingNewSourceText = sources[index].friendlyName;
    call_homeassistant_service("media_player.select_source", {
      {"entity_id", entityId},
      {"source", sources[index].friendlyName.c_str()},
    });
  }

  void playPause() {
    ESP_LOGI("speaker", "%s play pause", entityId.c_str());
    call_homeassistant_service("media_player.media_play_pause", {
      {"entity_id", entityId},
    });
  }

  void nextTrack() {
    ESP_LOGI("speaker", "%s next track", entityId.c_str());
    call_homeassistant_service("media_player.media_next_track", {
      {"entity_id", entityId},
    });
  }

  MenuTitleState menuTitlePlayerState() {
    switch (playerState) {
      case PlayingRemoteState:
        return PlayingMenuTitleState;
      case PausedRemoteState:
        return PausedMenuTitleState;
      case StoppedRemoteState:
        return StoppedMenuTitleState;
      case PowerOffRemoteState:
        return PowerOffMenuTitleState;
      case NoRemoteState:
        return NoMenuTitleState;
    }
    return NoMenuTitleState;
  }
};

class SonosSpeakerComponent : public BasePlayerComponent {
 public:
  SonosSpeakerComponent(
    DisplayUpdateInterface& newCallback,
    int newIndex,
    SpeakerSetup newSpeakerSetup
  ) : BasePlayerComponent { 
    newCallback,
    newIndex,
    newSpeakerSetup.entityId,
    newSpeakerSetup.friendlyName,
    SpeakerRemotePlayerType
  } {
    setup();
  }
  double localVolume = -1;
  double speaker_volume = -1;
  double volumeStep = 0.04;
  bool shuffle = false;
  bool muted = false;
  std::vector<std::string> groupMembers;
  std::string mediaPlaylist = "";
  std::string mediaAlbumName = "";
  int mediaDuration = -1;
  int mediaPosition = -1;

  void setup() {
    ESP_LOGI("speaker", "Sonos Speaker subbed %s", entityId.c_str());
    superSetup();
    subscribe_homeassistant_state(&SonosSpeakerComponent::speaker_volume_changed, entityId, "volume_level");
    subscribe_homeassistant_state(&SonosSpeakerComponent::speaker_muted_changed, entityId, "is_volume_muted");
    subscribe_homeassistant_state(&SonosSpeakerComponent::shuffle_changed, entityId, "shuffle");
    subscribe_homeassistant_state(&SonosSpeakerComponent::group_members_changed, entityId, "group_members");
    subscribe_homeassistant_state(&SonosSpeakerComponent::player_media_title_changed, entityId, "media_title");
    subscribe_homeassistant_state(&SonosSpeakerComponent::player_media_artist_changed, entityId, "media_artist");
    subscribe_homeassistant_state(&SonosSpeakerComponent::playlist_changed, entityId, "media_playlist");
    subscribe_homeassistant_state(&SonosSpeakerComponent::media_album_changed, entityId, "media_album_name");
    subscribe_homeassistant_state(&SonosSpeakerComponent::media_duration_changed, entityId, "media_duration");
    subscribe_homeassistant_state(&SonosSpeakerComponent::media_position_changed, entityId, "media_position");
  }

  void ungroup() {
    ESP_LOGI("speaker", "%s ungroup speaker", entityId.c_str());
    call_homeassistant_service("media_player.unjoin", {
      {"entity_id", entityId},
    });
    display.updateDisplay(false);
  }

  void joinGroup(std::string newSpeakerName) {
    ESP_LOGI("speaker", "%s group speaker to %s", entityId.c_str(), newSpeakerName.c_str());
    call_homeassistant_service("media_player.join", {
      {"entity_id", newSpeakerName.c_str()},
      {"group_members", entityId.c_str()},
    });
    display.updateDisplay(false);
  }

  void toggleShuffle() {
    ESP_LOGI("speaker", "%s toggle shuffle", entityId.c_str());
    call_homeassistant_service("media_player.shuffle_set", {
      {"entity_id", entityId},
      {"shuffle", shuffle ? "false" : "true"},
    });
  }

  void toggleMute() {
    ESP_LOGI("speaker", "%s toggle mute", entityId.c_str());
    call_homeassistant_service("media_player.volume_mute", {
      {"entity_id", entityId},
      {"is_volume_muted", muted ? "false" : "true"},
    });
  }

  void increaseVolume() {
    if(speaker_volume == -1) {
      localVolume = 0;
      return;
    }
    if(localVolume + volumeStep > 1) {
      localVolume = 1;
    } else {
      localVolume = localVolume + volumeStep;
    }
    updateVolumeLevel();
  }

  void decreaseVolume() {
    if(speaker_volume == -1 || localVolume - volumeStep < 0) {
      localVolume = 0;
      return;
    }
    if(localVolume - volumeStep > 1) {
      localVolume = 0;
    } else {
      localVolume = localVolume - volumeStep;
    }
    updateVolumeLevel();
  }

  void updateVolumeLevel() {
    std::string entityIds = entityId;
    for (auto &speaker: groupMembers) {
      if(speaker != entityId) {
        entityIds = entityIds + ", " + speaker;
      }
    }
    if(muted) {
      // unmute all speakers 
      call_homeassistant_service("media_player.volume_mute", {
        {"entity_id", entityIds},
        {"is_volume_muted", "false"},
      });
    }
    ESP_LOGI("speaker", "%s volume update %f", entityIds.c_str(), localVolume);
    call_homeassistant_service("media_player.volume_set", {
      {"entity_id", entityIds},
      {"volume_level", to_string(localVolume)},
    });
  }

private:
  void player_media_title_changed(std::string state) {
    ESP_LOGI("Player", "%s Player media title changed to %s", entityId.c_str(), state.c_str());
    if(strcmp(state.c_str(), mediaTitle.c_str()) != 0) {
      mediaPosition = 0;
    }
    if(strcmp("TV", state.c_str()) != 0) {
      mediaTitle = state.c_str();
    } else {
      mediaTitle = "TV";
      mediaArtist = "";
      mediaPlaylist = "";
      mediaPosition = -1;
    }
    mediaDuration = -1;
    display.updateDisplay(false);
  }

  void player_media_artist_changed(std::string state) {
    ESP_LOGI("Player", "%s Player artist changed to %s", entityId.c_str(), state.c_str());
    mediaArtist = state.c_str();
    display.updateDisplay(false);
  }

  void speaker_volume_changed(std::string state) {
    ESP_LOGI("speaker", "%s Sonos Speaker volume changed to %s", entityId.c_str(), state.c_str());
    speaker_volume =  atof(state.c_str());
    if(localVolume == -1) {
      localVolume = atof(state.c_str());
    }
    display.updateDisplay(false);
  }

  void speaker_muted_changed(std::string state) {
    ESP_LOGI("speaker", "%s Sonos Speaker muted changed to %s", entityId.c_str(), state.c_str());
    muted = strcmp(state.c_str(), "on") == 0;
    display.updateDisplay(false);
  }

  void shuffle_changed(std::string state) {
    ESP_LOGI("speaker", "%s Sonos Speaker shuffle changed to %s", entityId.c_str(), state.c_str());
    shuffle = strcmp(state.c_str(), "on") == 0;
    display.updateDisplay(false);
  }

  void playlist_changed(std::string state) {
    ESP_LOGI("speaker", "%s Sonos Speaker playlist changed to %s", entityId.c_str(), state.c_str());
    mediaPlaylist = state.c_str();
    display.updateDisplay(false);
  }

  void media_album_changed(std::string state) {
    ESP_LOGI("speaker", "%s Sonos Speaker album changed to %s", entityId.c_str(), state.c_str());
    mediaAlbumName = state.c_str();
    display.updateDisplay(false);
  }

  void group_members_changed(std::string state) {
    ESP_LOGI("speaker", "%s Sonos Speaker group members changed to %s", entityId.c_str(), state.c_str());
    std::vector<std::string> out;
    tokenize(state, ",", out);
    groupMembers.clear();
    for (auto &state: out) {
      std::string newGroupedSpeaker = filter(state);
      groupMembers.push_back(filter(state));
    }
    display.updateDisplay(false);
  }

  void media_duration_changed(std::string state) {
    ESP_LOGI("speaker", "%s Sonos Speaker media duration changed to %s", entityId.c_str(), state.c_str());
    mediaDuration = atof(state.c_str());
    display.updateDisplay(false);
  }

  void media_position_changed(std::string state) {
    ESP_LOGI("speaker", "%s Sonos Speaker media position changed to %s", entityId.c_str(), state.c_str());
    mediaPosition = atof(state.c_str());
    display.updateDisplay(false);
  }
};

class TVPlayerComponent : public BasePlayerComponent {
 public:
  TVPlayerComponent(
    DisplayUpdateInterface& newCallback,
    int newIndex,
    TVSetup newTVSetup,
    SonosSpeakerComponent* newSoundBar
  ) : BasePlayerComponent { 
    newCallback,
    newIndex,
    newTVSetup.entityId,
    newTVSetup.friendlyName,
    TVRemotePlayerType
  }, speaker(newSoundBar) {
    setup();
  }
  SonosSpeakerComponent *speaker;

  void setup() {
    ESP_LOGI("PlayerTV", "TV subbed %s", entityId.c_str());
    superSetup();
    subscribe_homeassistant_state(&TVPlayerComponent::player_media_artist_changed, entityId, "source");
    subscribe_homeassistant_state(&TVPlayerComponent::player_source_list_changed, entityId, "source_list");
  }

  void player_media_artist_changed(std::string state) {
    ESP_LOGI("PlayerTV", "%s Player source changed to %s", entityId.c_str(), state.c_str());
    mediaArtist = state.c_str();
    display.updateDisplay(false);
  }

  void player_source_list_changed(std::string state) {
    ESP_LOGI("PlayerTV", "%s Player source list changed to %s", entityId.c_str(), state.c_str());
    std::vector<std::string> out;
    tokenize(state, ",", out);
    sources.clear();
    for (auto &state: out) {
      std::string source = filter(state);
      sources.push_back(MenuTitle(source, "", NoMenuTitleState));
    }
  }

  void tvRemoteCommand(std::string command) {
    std::string remoteName = entityId.substr(12).insert(0, "remote");
    ESP_LOGI("PlayerTV", "remote %s, %s", command.c_str(), remoteName.c_str());
    call_homeassistant_service("remote.send_command", {
      {"entity_id", remoteName},
      {"command", command.c_str()},
    });
  }

  void increaseVolume() {
    tvRemoteCommand("volume_up");
  }

  void decreaseVolume() {
    tvRemoteCommand("volume_down");
  }
};

class SonosSpeakerGroupComponent : public CustomAPIDevice, public Component {
 public:
  SonosSpeakerGroupComponent(DisplayUpdateInterface& newCallback) : display(newCallback) { }

  BasePlayerComponent *activePlayer;
  std::vector<SonosSpeakerComponent*> speakers;
  std::vector<TVPlayerComponent*> tvs;
  bool playerSearchFinished = false;

  void findActivePlayer() {
    if (playerSearchFinished) {
      return;
    }
    for (auto &tv: tvs) {
      if (tv->playerState == PlayingRemoteState) {
        activePlayer = tv;
        playerSearchFinished = true;
        display.updateDisplay(true);
        return;
      }
    }
    for (auto &speaker: speakers) {
      if (speaker->playerState == NoRemoteState) {
        return;
      } else if (speaker->playerState == PlayingRemoteState && speaker->mediaTitle != "") {
        if(speaker->mediaTitle == "TV") {
          for (auto &tv: tvs) {
            if (tv->speaker == speaker) {
              activePlayer = tv;
              playerSearchFinished = true;
              display.updateDisplay(true);
              return;
            }
          }
        } else {
          activePlayer = speaker;
          playerSearchFinished = true;
          display.updateDisplay(true);
          return;
        }
      }
    }
    if(tvs.size() > 0) {
      activePlayer = tvs[0];
      playerSearchFinished = true;
      display.updateDisplay(true);
    } else if(speakers.size() > 0) {
      activePlayer = speakers[0];
      playerSearchFinished = true;
      display.updateDisplay(true);
    }
  }

  void setup(std::vector<TVSetup> newTVSetups, std::vector<SpeakerSetup> newSpeakerSetups) {
    bool first = true;
    playerSearchFinished = false;

    int tvIndex = 0;
    int speakerIndex = 0;
    for (auto &newTVSetup: newTVSetups) {
      SonosSpeakerComponent* newSoundBar = NULL;
      if(newTVSetup.soundBar != NULL) {
        auto newSpeaker = new SonosSpeakerComponent(
          display,
          newTVSetups.size() + speakerIndex,
          *newTVSetup.soundBar
        );
        speakers.push_back(newSpeaker);
        speakerIndex++;
      }
      auto newTV = new TVPlayerComponent(
        display,
        tvIndex,
        newTVSetup,
        newSoundBar
      );
      tvs.push_back(newTV);
      tvIndex++;
    }
    for (auto &newSpeakerSetup: newSpeakerSetups) {
      auto newSpeaker = new SonosSpeakerComponent(
        display,
        newTVSetups.size() + speakerIndex,
        newSpeakerSetup
      );
      speakers.push_back(newSpeaker);
      speakerIndex++;
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

  std::string friendlyNameForEntityId(std::string speakerentityId) {
    for (auto &speaker: speakers) {
      if (speaker->entityId == speakerentityId) {
        return speaker->friendlyName;
      }
    }
    return "";
  }

  void on_state_changed(std::string state) {
    ESP_LOGI("group", "Sonos Favorites changes to %s", state.c_str());
    std::string delim = ", '";
 
    std::vector<std::string> out;
    tokenize(state, delim, out);

    std::vector<MenuTitle> sources;
    for (auto &state: out) {
      std::string source = filter(state);
      std::string outSource = source.substr(source.find(" ") + 1);
      sources.push_back(MenuTitle(outSource, "", NoMenuTitleState));
    }

    for(auto &player: speakers) {
      player->sources = sources;
    }
  }

  void increaseSpeakerVolume() {
    if(activePlayer->playerType != TVRemotePlayerType) {
      SonosSpeakerComponent* activeSpeaker = static_cast<SonosSpeakerComponent*>(activePlayer);
      if (activeSpeaker != NULL) {
        activeSpeaker->increaseVolume();
      }
    } else {
      TVPlayerComponent* activeTV = static_cast<TVPlayerComponent*>(activePlayer);
      if (activeTV != NULL) {
        if (activeTV->speaker != NULL) {
          activeTV->speaker->increaseVolume();
        } else {
          activeTV->increaseVolume();
        }
      }
    }
  }

  void decreaseSpeakerVolume() {
    if(activePlayer->playerType != TVRemotePlayerType) {
      SonosSpeakerComponent* activeSpeaker = static_cast<SonosSpeakerComponent*>(activePlayer);
      if (activeSpeaker != NULL) {
        activeSpeaker->decreaseVolume();
      }
    } else {
      TVPlayerComponent* activeTV = static_cast<TVPlayerComponent*>(activePlayer);
      if (activeTV != NULL) {
        if (activeTV->speaker != NULL) {
          activeTV->speaker->decreaseVolume();
        } else {
          activeTV->decreaseVolume();
        }
      }
    }
  }

  bool mediaShuffling() {
    SonosSpeakerComponent* activeSpeaker = static_cast<SonosSpeakerComponent*>(activePlayer);
    if (activeSpeaker != NULL) {
      if (activeSpeaker->shuffle) {
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

  void toggleMute() {
    SonosSpeakerComponent* activeSpeaker = static_cast<SonosSpeakerComponent*>(activePlayer);
    if (activeSpeaker != NULL) {
      activeSpeaker->toggleMute();
    }
  }

  std::string shuffleString() {
      if (mediaShuffling()) {
        return "Shfl On";
      } else {
        return "Shfl Off";
      }
  }

  std::string muteString() {
    SonosSpeakerComponent* activeSpeaker = static_cast<SonosSpeakerComponent*>(activePlayer);
    if (activeSpeaker != NULL) {
      if (activeSpeaker->muted) {
        return "Unmute";
      } else {
        return "Mute";
      }
    }
    return "";
  }

  MenuTitle *newSpeakerGroupParent = NULL;

  double getVolumeLevel() {
    if (activePlayer->playerType == TVRemotePlayerType) {
      TVPlayerComponent* activeTV = static_cast<TVPlayerComponent*>(activePlayer);
      if (activeTV != NULL) {
        if (activeTV->speaker != NULL) {
          if (activeTV->speaker->speaker_volume != -1) {
            double volume = activeTV->speaker->localVolume * 100;
            return volume;
          }
        } else {
          return -1;
        }
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

  std::string getMediaTitleString() {
    if(activePlayer->mediaTitle != "TV") {
      return activePlayer->mediaTitle;
    } else {
      return "TV";
    }
  }

  std::string getMediaArtistString() {
    return activePlayer->mediaArtist;
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
      if (speaker->groupMembers.size() > 1) {
        if(speaker->groupMembers[0] != speaker->entityId) {
          ESP_LOGI("group title string", "%s not parent %s", speaker->groupMembers[0].c_str(), speaker->entityId.c_str());
          // speaker isn't the group parent
          continue;
        }

        out.push_back(MenuTitle(speaker->friendlyName, speaker->entityId, speaker->menuTitlePlayerState()));
        for (auto &groupedSpeaker: speaker->groupMembers) {
          if(groupedSpeaker != speaker->entityId) {
            groupedMembers.push_back(groupedSpeaker);
            std::string groupedSpeakerName = friendlyNameForEntityId(groupedSpeaker);
            out.push_back(MenuTitle(groupedSpeakerName, groupedSpeaker, GroupedMenuTitleState));
          }
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
    for (auto &tv: tvs) {
      out.push_back(MenuTitle(tv->friendlyName, tv->entityId, tv->menuTitlePlayerState()));
    }
    for (auto &groupedTVSpeaker: groupedTVSpeakers) {
      out.push_back(groupedTVSpeaker);
    }
    return out;
  }

  std::string headerMediaPlayerTitleString() {
    if (activePlayer -> entityId != "") {
      std::string entityId = activePlayer -> friendlyName;
      if (activePlayer -> playerType != TVRemotePlayerType) {
        SonosSpeakerComponent * activeSpeaker = static_cast < SonosSpeakerComponent * > (activePlayer);
        if (activeSpeaker != NULL) {
          if (activeSpeaker -> groupMembers.size() > 1) {
            entityId = entityId + " + " + to_string(activeSpeaker -> groupMembers.size() - 1);
          }
        }
      }
      return entityId;
    }
    return "";
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

    if (speaker->groupMembers.size() > 1) {
      if (std::find(speaker->groupMembers.begin(), speaker->groupMembers.end(), newSpeakerGroupParent->entityId) != speaker->groupMembers.end()) {
        speaker->ungroup();
      }
    } else {
      speaker->joinGroup(newSpeakerGroupParent->entityId);
    }
  }

  bool updateMediaPosition() {
    bool updateDisplay = false;
    for (auto &speaker: speakers) {
      if (
        speaker->playerState == PlayingRemoteState &&
        speaker->mediaDuration >= 0 && 
        speaker->mediaPosition >= 0 && 
        speaker->mediaPosition < speaker->mediaDuration
      ) {
        speaker->mediaPosition++;
        updateDisplay = true;
      }
    }
    switch (activePlayer->playerType) {
    case TVRemotePlayerType:
      updateDisplay = false;
      break;
    case SpeakerRemotePlayerType:
      break;
    }
    return updateDisplay;
  }

  void selectNextMediaPlayer() {
    if(activePlayer != NULL) {
      if(activePlayer->index < tvs.size() - 1) {
        activePlayer = tvs[activePlayer->index + 1];
      } else if(activePlayer->index - tvs.size() + 1 < speakers.size()) {
        activePlayer = speakers[activePlayer->index - tvs.size() + 1];
      } else if(tvs.size() > 0) {
        activePlayer = tvs[0];
      }
    }
  }

  std::string playTitleString() {
    switch(activePlayer->playerState) {
      case PausedRemoteState:
        return "Play";
      case PlayingRemoteState:
        return "Pause";
      case StoppedRemoteState:
      case NoRemoteState:
        return "Stopped";
      case PowerOffRemoteState:
        return "Off";
    }
    return "";
  }

  void sendActivePlayerRemoteCommand(std::string command) {
    TVPlayerComponent* activeTV = static_cast<TVPlayerComponent*>(activePlayer);
    if (activeTV != NULL) {
      activeTV->tvRemoteCommand(command);
    }
  }

  private:
  DisplayUpdateInterface& display;
};

#endif