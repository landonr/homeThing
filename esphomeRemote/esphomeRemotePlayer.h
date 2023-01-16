#include "esphome.h"
#include "DisplayUpdateInterface.h"
#include "MenuTitle.h"
#include "FriendlyNameEntity.h"
#include "TextHelpers.h"

#pragma once

enum RemotePlayerType { TVRemotePlayerType, SpeakerRemotePlayerType };

struct RemotePlayerStateUpdatedInterface {
 public:
  virtual void stateUpdated(RemotePlayerState state){};
  virtual ~RemotePlayerStateUpdatedInterface() {}
};

class SpeakerSetup {
 public:
  SpeakerSetup(std::string newEntityId, std::string newFriendlyName)
      : entityId(newEntityId), friendlyName(newFriendlyName) {}
  std::string entityId;
  std::string friendlyName;
};

class TVSetup {
 public:
  TVSetup(std::string newEntityId, std::string newFriendlyName, SpeakerSetup *newSoundBar)
      : entityId(newEntityId), friendlyName(newFriendlyName), soundBar(newSoundBar) {}
  std::string entityId;
  std::string friendlyName;
  SpeakerSetup *soundBar;
};

class BasePlayerComponent : public CustomAPIDevice, public Component {
 protected:
  DisplayUpdateInterface &display;
  RemotePlayerStateUpdatedInterface &stateCallback;

 public:
  BasePlayerComponent(DisplayUpdateInterface &newCallback, RemotePlayerStateUpdatedInterface &newStateCallback,
                      int newIndex, std::string newEntityId, std::string newFriendlyName,
                      RemotePlayerType newPlayerType)
      : display(newCallback),
        stateCallback(newStateCallback),
        index(newIndex),
        entityId(newEntityId),
        friendlyName(newFriendlyName),
        playerType(newPlayerType) {}

  std::string mediaTitle = "";
  std::string mediaArtist = "";
  RemotePlayerMediaSource mediaSource = NoRemotePlayerMediaSource;
  RemotePlayerState playerState = NoRemotePlayerState;
  std::vector<std::shared_ptr<MenuTitleSource>> sources;
  int index;
  std::string entityId;
  std::string friendlyName;
  RemotePlayerType playerType;

  void superSetup() {
    ESP_LOGI("Player", "Player subbed %s", entityId.c_str());
    subscribe_homeassistant_state(&BasePlayerComponent::playerState_changed, entityId.c_str());
  }

  void playSource(MenuTitleSource source) {
    ESP_LOGI("Player", "Player play source %s %s %s %d", entityId.c_str(), source.entityId.c_str(),
             source.sourceTypeString().c_str(), source.sourceType);
    switch (source.sourceType) {
      case MusicRemotePlayerSourceType:
        playMedia(source);
        break;
      case FavoriteItemIDRemotePlayerSourceType:
      case SourceRemotePlayerSourceType:
        selectSource(source);
        break;
    }
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

  std::string mediaTitleString() {
    switch (playerType) {
      case TVRemotePlayerType:
        return playerSourceStateString(mediaSource);
      case SpeakerRemotePlayerType:
        return mediaArtist;
    }
    return "";
  }

  std::string mediaSubtitleString() {
    switch (playerType) {
      case TVRemotePlayerType:
        return "";
      case SpeakerRemotePlayerType:
        return mediaTitle;
    }
    return "";
  }

  virtual void clearSource() {}

  void clearMedia() {
    clearSource();
    mediaTitle = "";
    mediaArtist = "";
  }

 private:
  void selectSource(MenuTitleSource source) {
    ESP_LOGI("player", "%s select source %s", entityId.c_str(), source.friendlyName.c_str());
    call_homeassistant_service("media_player.select_source", {
                                                                 {"entity_id", entityId},
                                                                 {"source", source.friendlyName.c_str()},
                                                             });
  }

  void playMedia(MenuTitleSource source) {
    ESP_LOGI("player", "%s play media %s %s", entityId.c_str(), source.friendlyName.c_str(), source.entityId.c_str());
    call_homeassistant_service("media_player.play_media", {
                                                              {"entity_id", entityId},
                                                              {"media_content_id", source.entityId.c_str()},
                                                              {"media_content_type", source.sourceTypeString().c_str()},
                                                          });
  }

  void playerState_changed(std::string state) {
    ESP_LOGI("Player", "%s state changed to %s", entityId.c_str(), state.c_str());
    if (state.length() == 0) {
      playerState = StoppedRemotePlayerState;
    } else if (strcmp(state.c_str(), "playing") == 0) {
      if (playerState == PlayingRemotePlayerState) {
        clearSource();
      }
      playerState = PlayingRemotePlayerState;
    } else if (strcmp(state.c_str(), "paused") == 0) {
      playerState = PausedRemotePlayerState;
    } else if (strcmp(state.c_str(), "standby") == 0) {
      playerState = PowerOffRemotePlayerState;
      clearMedia();
    } else if (strcmp(state.c_str(), "off") == 0) {
      playerState = PowerOffRemotePlayerState;
      clearMedia();
    } else if (strcmp(state.c_str(), "home") == 0 || strcmp(state.c_str(), "Roku") == 0) {
      playerState = StoppedRemotePlayerState;
      clearMedia();
    } else if (strcmp(state.c_str(), "on") == 0) {
      playerState = StoppedRemotePlayerState;
    } else if (strcmp(state.c_str(), "idle") == 0) {
      clearMedia();
      playerState = StoppedRemotePlayerState;
    } else if (strcmp(state.c_str(), "unavailable") == 0) {
      playerState = UnavailableRemotePlayerState;
    } else {
      playerState = NoRemotePlayerState;
    }
    stateCallback.stateUpdated(playerState);
    display.updateDisplay(false);
  }
};

class SonosSpeakerComponent : public BasePlayerComponent {
 public:
  SonosSpeakerComponent(DisplayUpdateInterface &newCallback, RemotePlayerStateUpdatedInterface &newStateCallback,
                        int newIndex, SpeakerSetup newSpeakerSetup)
      : BasePlayerComponent{newCallback,
                            newStateCallback,
                            newIndex,
                            newSpeakerSetup.entityId,
                            newSpeakerSetup.friendlyName,
                            SpeakerRemotePlayerType} {
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
  BasePlayerComponent *tv = NULL;

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
    subscribe_homeassistant_state(&SonosSpeakerComponent::media_source_changed, entityId, "media_content_id");
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
    if (speaker_volume == -1) {
      localVolume = 0;
      return;
    }
    if (localVolume + volumeStep > 1) {
      localVolume = 1;
    } else {
      localVolume = localVolume + volumeStep;
    }
    updateVolumeLevel();
  }

  void decreaseVolume() {
    if (speaker_volume == -1 || localVolume - volumeStep < 0) {
      localVolume = 0;
      return;
    }
    if (localVolume - volumeStep > 1) {
      localVolume = 0;
    } else {
      localVolume = localVolume - volumeStep;
    }
    updateVolumeLevel();
  }

  void updateVolumeLevel() {
    std::string entityIds = entityId;
    for (auto &speaker : groupMembers) {
      if (speaker != entityId) {
        entityIds = entityIds + ", " + speaker;
      }
    }
    if (muted) {
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

  virtual void clearSource() {
    BasePlayerComponent::clearSource();
    mediaPlaylist = "";
    mediaAlbumName = "";
  }

 private:
  void player_media_title_changed(std::string state) {
    ESP_LOGI("Player", "%s Player media title changed to %s", entityId.c_str(), state.c_str());
    if (strcmp(state.c_str(), mediaTitle.c_str()) != 0) {
      mediaPosition = 0;
    }
    if (strcmp("TV", state.c_str()) != 0) {
      mediaTitle = state.c_str();
    } else {
      mediaTitle = "";
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
    speaker_volume = atof(state.c_str());
    if (localVolume == -1) {
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
    TextHelpers::tokenize(state, ",", out);
    groupMembers.clear();
    for (auto &state : out) {
      std::string newGroupedSpeaker = TextHelpers::filter(state);
      groupMembers.push_back(newGroupedSpeaker);
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

  void media_source_changed(std::string state) {
    ESP_LOGI("speaker", "%s Speaker source changed to %s", entityId.c_str(), state.c_str());
    if (state.find("spdif") != std::string::npos) {
      mediaSource = TVRemotePlayerMediaSource;
    } else if (state.find("spotify") != std::string::npos) {
      mediaSource = SpotifyRemotePlayerMediaSource;
    } else {
      mediaSource = NoRemotePlayerMediaSource;
    }
    display.updateDisplay(false);
  }
};

class TVPlayerComponent : public BasePlayerComponent {
 public:
  TVPlayerComponent(DisplayUpdateInterface &newCallback, RemotePlayerStateUpdatedInterface &newStateCallback,
                    int newIndex, TVSetup newTVSetup, SonosSpeakerComponent *newSoundBar)
      : BasePlayerComponent{newCallback,         newStateCallback,        newIndex,
                            newTVSetup.entityId, newTVSetup.friendlyName, TVRemotePlayerType},
        speaker(newSoundBar) {
    setup();
  }
  SonosSpeakerComponent *speaker;

  void setup() {
    ESP_LOGI("PlayerTV", "TV subbed %s", entityId.c_str());
    superSetup();
    subscribe_homeassistant_state(&TVPlayerComponent::player_source_changed, entityId, "source");
    subscribe_homeassistant_state(&TVPlayerComponent::player_source_list_changed, entityId, "source_list");
  }

  void player_source_changed(std::string state) {
    ESP_LOGI("PlayerTV", "%s Player source changed to %s", friendlyName.c_str(), state.c_str());
    if (state.find("YouTube") != std::string::npos) {
      mediaSource = YouTubeRemotePlayerMediaSource;
    } else if (state.find("Netflix") != std::string::npos) {
      mediaSource = NetflixRemotePlayerMediaSource;
    } else if (state.find("Plex") != std::string::npos) {
      mediaSource = PlexRemotePlayerMediaSource;
    } else if (state.find("Roku") != std::string::npos) {
      mediaSource = HomeRemotePlayerMediaSource;
    } else {
      mediaSource = NoRemotePlayerMediaSource;
    }
    display.updateDisplay(false);
  }

  void player_source_list_changed(std::string state) {
    ESP_LOGI("PlayerTV", "%s Player source list changed to %s", friendlyName.c_str(), state.c_str());
    auto newSources = TextHelpers::parseJsonArray(TextHelpers::replaceAll(state, "\\xa0", " "), "source");
    sources.assign(newSources.begin(), newSources.end());
  }

  void tvRemoteCommand(std::string command) {
    std::string remoteName = entityId.substr(12).insert(0, "remote");
    ESP_LOGI("PlayerTV", "remote %s, %s", command.c_str(), remoteName.c_str());
    call_homeassistant_service("remote.send_command", {
                                                          {"entity_id", remoteName},
                                                          {"command", command.c_str()},
                                                      });
  }

  void increaseVolume() { tvRemoteCommand("volume_up"); }

  void decreaseVolume() { tvRemoteCommand("volume_down"); }
};

class SonosSpeakerGroupComponent : public CustomAPIDevice, public Component, public RemotePlayerStateUpdatedInterface {
 public:
  explicit SonosSpeakerGroupComponent(DisplayUpdateInterface &newCallback) : display(newCallback) {}

  BasePlayerComponent *activePlayer = NULL;
  std::vector<SonosSpeakerComponent *> speakers;
  std::vector<TVPlayerComponent *> tvs;
  bool playerSearchFinished = false;
  std::string playingNewSourceText = "";
  int loadedPlayers = 0;

  int totalPlayers() { return speakers.size() + tvs.size(); }

  void findActivePlayer(bool background = false) {
    if (playerSearchFinished) {
      return;
    }
    BasePlayerComponent *newActivePlayer = NULL;
    int tempLoadedPlayers = 0;
    for (auto &tv : tvs) {
      if (tv->playerState == NoRemotePlayerState) {
        return;
      } else if (newActivePlayer != NULL) {
        if (newActivePlayer->playerState < tv->playerState) {
          newActivePlayer = tv;
        }
      } else {
        newActivePlayer = tv;
      }
      tempLoadedPlayers++;
      loadedPlayers = max(tempLoadedPlayers, loadedPlayers);
    }
    for (auto &speaker : speakers) {
      if (speaker->playerState == NoRemotePlayerState) {
        return;
      } else if (speaker->tv != NULL && speaker->mediaSource == TVRemotePlayerMediaSource &&
                 newActivePlayer->playerState < speaker->tv->playerState) {
        newActivePlayer = speaker->tv;
      } else if (newActivePlayer != NULL) {
        if (newActivePlayer->playerState < speaker->playerState && speaker->mediaSource != TVRemotePlayerMediaSource) {
          newActivePlayer = speaker;
        }
      } else {
        newActivePlayer = speaker;
      }
      tempLoadedPlayers++;
      loadedPlayers = max(tempLoadedPlayers, loadedPlayers);
    }
    if (newActivePlayer != NULL) {
      ESP_LOGI("FIND_ACTIVE_PLAYER", "setting active player %s", newActivePlayer->entityId.c_str());
      setActivePlayer(newActivePlayer);
      playerSearchFinished = true;
      display.setUpdateInterval(60000);
      if (!background) {
        display.updateDisplay(true);
      }
    }
  }

  void setActivePlayer(BasePlayerComponent *newActivePlayer) {
    ESP_LOGI("ACTIVE_PLAYER", "New active player %s", newActivePlayer->entityId.c_str());
    activePlayer = newActivePlayer;
  }

  void setup(std::vector<TVSetup> newTVSetups, std::vector<SpeakerSetup> newSpeakerSetups) {
    bool first = true;
    playerSearchFinished = false;

    int tvIndex = 0;
    int speakerIndex = 0;
    for (auto &newTVSetup : newTVSetups) {
      SonosSpeakerComponent *newSoundBar = NULL;
      if (newTVSetup.soundBar != NULL) {
        auto newSpeaker =
            new SonosSpeakerComponent(display, *this, newTVSetups.size() + speakerIndex, *newTVSetup.soundBar);
        speakers.push_back(newSpeaker);
        speakerIndex++;
        newSoundBar = newSpeaker;
      }
      auto newTV = new TVPlayerComponent(display, *this, tvIndex, newTVSetup, newSoundBar);
      if (newSoundBar != NULL) {
        newSoundBar->tv = newTV;
      }
      tvs.push_back(newTV);
      tvIndex++;
    }
    for (auto &newSpeakerSetup : newSpeakerSetups) {
      auto newSpeaker = new SonosSpeakerComponent(display, *this, newTVSetups.size() + speakerIndex, newSpeakerSetup);
      speakers.push_back(newSpeaker);
      speakerIndex++;
    }
    subscribe_homeassistant_state(&SonosSpeakerGroupComponent::playlists_changed, "sensor.playlists_sensor",
                                  "playlists");
    subscribe_homeassistant_state(&SonosSpeakerGroupComponent::sonos_favorites_changed, "sensor.sonos_favorites",
                                  "items");
  }

  std::vector<std::string> groupNames() {
    std::vector<std::string> output;
    for (auto &speaker : speakers) {
      output.push_back(speaker->friendlyName);
    }
    return output;
  }

  std::string friendlyNameForEntityId(std::string speakerentityId) {
    for (auto &speaker : speakers) {
      if (speaker->entityId == speakerentityId) {
        return speaker->friendlyName;
      }
    }
    return "";
  }

  void stripUnicode(std::string &str) {
    str.erase(remove_if(str.begin(), str.end(), [](char c) { return !(c >= 0 && c < 128); }), str.end());
  }

  void increaseSpeakerVolume() {
    if (activePlayer->playerType != TVRemotePlayerType) {
      SonosSpeakerComponent *activeSpeaker = static_cast<SonosSpeakerComponent *>(activePlayer);
      if (activeSpeaker != NULL) {
        activeSpeaker->increaseVolume();
      }
    } else {
      TVPlayerComponent *activeTV = static_cast<TVPlayerComponent *>(activePlayer);
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
    if (activePlayer->playerType != TVRemotePlayerType) {
      SonosSpeakerComponent *activeSpeaker = static_cast<SonosSpeakerComponent *>(activePlayer);
      if (activeSpeaker != NULL) {
        activeSpeaker->decreaseVolume();
      }
    } else {
      TVPlayerComponent *activeTV = static_cast<TVPlayerComponent *>(activePlayer);
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
    SonosSpeakerComponent *activeSpeaker = static_cast<SonosSpeakerComponent *>(activePlayer);
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
    SonosSpeakerComponent *activeSpeaker = static_cast<SonosSpeakerComponent *>(activePlayer);
    if (activeSpeaker != NULL) {
      activeSpeaker->toggleShuffle();
    }
  }

  void toggleMute() {
    SonosSpeakerComponent *activeSpeaker = static_cast<SonosSpeakerComponent *>(activePlayer);
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
    SonosSpeakerComponent *activeSpeaker = static_cast<SonosSpeakerComponent *>(activePlayer);
    if (activeSpeaker != NULL) {
      if (activeSpeaker->muted) {
        return "Unmute";
      } else {
        return "Mute";
      }
    }
    return "";
  }

  MenuTitlePlayer *newSpeakerGroupParent = NULL;

  double getVolumeLevel() {
    if (activePlayer->playerType == TVRemotePlayerType) {
      TVPlayerComponent *activeTV = static_cast<TVPlayerComponent *>(activePlayer);
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
      SonosSpeakerComponent *activeSpeaker = static_cast<SonosSpeakerComponent *>(activePlayer);
      if (activeSpeaker != NULL) {
        if (activeSpeaker->speaker_volume != -1) {
          double volume = activeSpeaker->localVolume * 100;
          return volume;
        }
      }
    }
    return 0;
  }

  std::vector<MenuTitlePlayer *> groupTitleSwitches() {
    std::vector<MenuTitlePlayer *> out;
    std::vector<std::string> groupedMembers;
    out.push_back(new MenuTitlePlayer("Group " + newSpeakerGroupParent->friendlyName, newSpeakerGroupParent->entityId,
                                      NoMenuTitleLeftIcon, ArrowMenuTitleRightIcon, newSpeakerGroupParent->mediaSource,
                                      newSpeakerGroupParent->playerState));

    for (auto &speaker : speakers) {
      if (newSpeakerGroupParent->entityId == speaker->entityId) {
        continue;
      } else {
        if (std::find(speaker->groupMembers.begin(), speaker->groupMembers.end(), newSpeakerGroupParent->entityId) !=
            speaker->groupMembers.end()) {
          out.push_back(new MenuTitlePlayer(speaker->friendlyName, speaker->entityId, OnMenuTitleLeftIcon,
                                            NoMenuTitleRightIcon, speaker->mediaSource, speaker->playerState));
        } else {
          out.push_back(new MenuTitlePlayer(speaker->friendlyName, speaker->entityId, OffMenuTitleLeftIcon,
                                            NoMenuTitleRightIcon, speaker->mediaSource, speaker->playerState));
        }
      }
    }
    return out;
  }

  std::vector<MenuTitlePlayer *> groupTitleString() {
    std::vector<MenuTitlePlayer *> out;
    std::vector<std::string> groupedMembers;
    for (auto &speaker : speakers) {
      if (std::find(groupedMembers.begin(), groupedMembers.end(), speaker->entityId) != groupedMembers.end()) {
        // skip grouped members that were already found
        continue;
      }
      if (speaker->groupMembers.size() > 1) {
        if (speaker->groupMembers[0] != speaker->entityId) {
          ESP_LOGD("group title string", "%s not parent %s", speaker->groupMembers[0].c_str(),
                   speaker->entityId.c_str());
          // speaker isn't the group parent
          continue;
        }
        out.push_back(new MenuTitlePlayer(speaker->friendlyName, speaker->entityId, NoMenuTitleLeftIcon,
                                          ArrowMenuTitleRightIcon, speaker->mediaSource, speaker->playerState));
        for (auto &groupedSpeaker : speaker->groupMembers) {
          if (groupedSpeaker != speaker->entityId) {
            groupedMembers.push_back(groupedSpeaker);
            std::string groupedSpeakerName = friendlyNameForEntityId(groupedSpeaker);
            out.push_back(new MenuTitlePlayer(groupedSpeakerName, groupedSpeaker, GroupedMenuTitleLeftIcon,
                                              ArrowMenuTitleRightIcon, speaker->mediaSource, speaker->playerState));
          }
        }
      } else {
        out.push_back(new MenuTitlePlayer(speaker->friendlyName, speaker->entityId, NoMenuTitleLeftIcon,
                                          ArrowMenuTitleRightIcon, speaker->mediaSource, speaker->playerState));
      }
    }
    return out;
  }

  std::vector<MenuTitlePlayer *> mediaPlayersTitleString() {
    std::vector<MenuTitlePlayer *> out;
    for (auto &speaker : speakers) {
      if (speaker->mediaSource != TVRemotePlayerMediaSource) {
        out.push_back(new MenuTitlePlayer(speaker->friendlyName, speaker->entityId, NoMenuTitleLeftIcon,
                                          NoMenuTitleRightIcon, speaker->mediaSource, speaker->playerState));
      }
    }
    for (auto &tv : tvs) {
      out.push_back(new MenuTitlePlayer(tv->friendlyName, tv->entityId, NoMenuTitleLeftIcon, NoMenuTitleRightIcon,
                                        tv->mediaSource, tv->playerState));
      if (tv->speaker != NULL && tv->speaker->mediaSource == TVRemotePlayerMediaSource) {
        out.push_back(new MenuTitlePlayer(tv->speaker->friendlyName, tv->speaker->entityId, GroupedMenuTitleLeftIcon,
                                          NoMenuTitleRightIcon, tv->speaker->mediaSource, tv->speaker->playerState));
      }
    }
    return out;
  }

  MenuTitlePlayer headerMediaPlayerTitle() {
    std::string friendlyName = activePlayer->friendlyName;
    if (activePlayer->playerType != TVRemotePlayerType) {
      SonosSpeakerComponent *activeSpeaker = static_cast<SonosSpeakerComponent *>(activePlayer);
      if (activeSpeaker != NULL) {
        if (activeSpeaker->groupMembers.size() > 1) {
          friendlyName = friendlyName + " + " + to_string(activeSpeaker->groupMembers.size() - 1);
        }
      }
    }
    return MenuTitlePlayer(friendlyName, activePlayer->entityId, NoMenuTitleLeftIcon, NoMenuTitleRightIcon,
                           activePlayer->mediaSource, activePlayer->playerState);
  }

  void selectGroup(MenuTitlePlayer selectedMenuTitle) {
    if (newSpeakerGroupParent == NULL) {
      MenuTitlePlayer *selectedMenuTitleCopy = new MenuTitlePlayer(
          selectedMenuTitle.friendlyName, selectedMenuTitle.entityId, selectedMenuTitle.leftIconState,
          selectedMenuTitle.rightIconState, selectedMenuTitle.mediaSource, selectedMenuTitle.playerState);
      newSpeakerGroupParent = selectedMenuTitleCopy;
      return;
    } else if (menuIndex == 0) {
      newSpeakerGroupParent = NULL;
      return;
    }

    std::vector<SonosSpeakerComponent *> speakerList;
    for (auto &speaker : speakers) {
      if (speaker->entityId == newSpeakerGroupParent->entityId) {
        continue;
      }
      speakerList.push_back(speaker);
    }
    auto speaker = speakerList[menuIndex - 1];

    if (speaker->groupMembers.size() > 1) {
      if (std::find(speaker->groupMembers.begin(), speaker->groupMembers.end(), newSpeakerGroupParent->entityId) !=
          speaker->groupMembers.end()) {
        speaker->ungroup();
      }
    } else {
      speaker->joinGroup(newSpeakerGroupParent->entityId);
    }
  }

  bool updateMediaPosition() {
    if (!playerSearchFinished) {
      return false;
    }
    bool updateDisplay = false;
    for (auto &speaker : speakers) {
      if (speaker->playerState == PlayingRemotePlayerState && speaker->mediaDuration >= 0 &&
          speaker->mediaPosition >= 0 && speaker->mediaPosition < speaker->mediaDuration) {
        speaker->mediaPosition++;
        updateDisplay = true;
      }
    }
    if (activePlayer != NULL) {
      switch (activePlayer->playerType) {
        case TVRemotePlayerType:
          updateDisplay = false;
          break;
        case SpeakerRemotePlayerType:
          break;
      }
    }
    return updateDisplay;
  }

  void selectNextMediaPlayer() {
    if (activePlayer != NULL) {
      if (activePlayer->index < tvs.size() - 1) {
        setActivePlayer(tvs[activePlayer->index + 1]);
      } else if (activePlayer->index - tvs.size() + 1 < speakers.size()) {
        setActivePlayer(speakers[activePlayer->index - tvs.size() + 1]);
      } else if (tvs.size() > 0) {
        setActivePlayer(tvs[0]);
      }
    }
  }

  std::string playTitleString() {
    switch (activePlayer->playerState) {
      case PausedRemotePlayerState:
        return "Play";
      case PlayingRemotePlayerState:
        return "Pause";
      case StoppedRemotePlayerState:
      case NoRemotePlayerState:
        return "Stopped";
      case PowerOffRemotePlayerState:
        return "Off";
      case UnavailableRemotePlayerState:
        return "Unavailable";
    }
    return "";
  }

  std::string mediaTitleString() {
    switch (activePlayer->playerType) {
      case TVRemotePlayerType:
        return activePlayer->mediaTitleString();
      case SpeakerRemotePlayerType:
        if (activePlayer->mediaSource == TVRemotePlayerMediaSource) {
          SonosSpeakerComponent *activeSpeaker = static_cast<SonosSpeakerComponent *>(activePlayer);
          if (activeSpeaker != NULL && activeSpeaker->tv != NULL) {
            return activeSpeaker->tv->mediaTitleString();
          }
        }
        return activePlayer->mediaTitleString();
    }
    return "";
  }

  std::string mediaSubtitleString() {
    switch (activePlayer->playerType) {
      case TVRemotePlayerType:
        return activePlayer->mediaSubtitleString();
      case SpeakerRemotePlayerType:
        if (activePlayer->mediaSource == TVRemotePlayerMediaSource) {
          return playerSourceStateString(activePlayer->mediaSource);
        }
        return activePlayer->mediaSubtitleString();
    }
    return "";
  }

  void sendActivePlayerRemoteCommand(std::string command) {
    TVPlayerComponent *activeTV = static_cast<TVPlayerComponent *>(activePlayer);
    if (activeTV != NULL) {
      activeTV->tvRemoteCommand(command);
    }
  }

  std::vector<std::shared_ptr<MenuTitleSource>> activePlayerSourceMenu() {
    std::vector<std::shared_ptr<MenuTitleSource>> out;
    switch (activePlayer->playerType) {
      case TVRemotePlayerType:
        return activePlayer->sources;
      case SpeakerRemotePlayerType: {
        SonosSpeakerComponent *activeSpeaker = static_cast<SonosSpeakerComponent *>(activePlayer);
        if (activeSpeaker != NULL) {
          if (activeSpeaker->tv != NULL && activeSpeaker->tv->friendlyName.size() > 0) {
            auto tvSource = std::make_shared<MenuTitleSource>(activeSpeaker->tv->friendlyName, "", NoMenuTitleRightIcon,
                                                              SourceRemotePlayerSourceType);
            out.push_back(tvSource);
          }
          out.insert(out.end(), spotifyPlaylists.begin(), spotifyPlaylists.end());
          out.insert(out.end(), sonosFavorites.begin(), sonosFavorites.end());
        }
        return out;
      }
    }
    return out;
  }

  void syncActivePlayer(RemotePlayerState state) {
    ESP_LOGI("SYNC", "Syncing active player %d", state);
    playerSearchFinished = false;
    findActivePlayer(true);
  }

  virtual void stateUpdated(RemotePlayerState state) {
    ESP_LOGD("SYNC", "state update callback %d %d", activePlayer == NULL, id(sync_active_player));
    if (activePlayer == NULL || id(sync_active_player) == true) {
      ESP_LOGD("SYNC", "Trying to sync active player, state: %d activePlayerNull: %d, sync_active_player: %d", state,
               activePlayer == NULL, id(sync_active_player) == true);
      switch (state) {
        case NoRemotePlayerState:
        case PausedRemotePlayerState:
        case UnavailableRemotePlayerState:
          ESP_LOGD("SYNC", "Trying to sync active player - 1");
          syncActivePlayer(state);
          return;
        case PlayingRemotePlayerState:
          if (activePlayer == NULL) {
            ESP_LOGD("SYNC", "Trying to sync active player - 2");
            syncActivePlayer(state);
          } else if (activePlayer->playerState < state) {
            ESP_LOGD("SYNC", "Trying to sync active player - 3 %d", activePlayer->playerState);
            syncActivePlayer(state);
          }
          return;
        case PowerOffRemotePlayerState:
        case StoppedRemotePlayerState:
          ESP_LOGD("SYNC", "Trying to sync active player - 4");
          syncActivePlayer(state);
      }
    }
  }

  void playSource(MenuTitleSource source) {
    activePlayer->clearSource();
    playingNewSourceText = source.friendlyName;
    if (activePlayer->playerType == SpeakerRemotePlayerType) {
      SonosSpeakerComponent *activeSpeaker = static_cast<SonosSpeakerComponent *>(activePlayer);
      activeSpeaker->mediaPlaylist = source.friendlyName;
    }
    activePlayer->playSource(source);
  }

 private:
  DisplayUpdateInterface &display;
  std::vector<std::shared_ptr<MenuTitleSource>> sonosFavorites;
  std::vector<std::shared_ptr<MenuTitleSource>> spotifyPlaylists;

  void sonos_favorites_changed(std::string state) {
    ESP_LOGI("group", "Sonos Favorites changes to %s", state.c_str());
    auto sources = TextHelpers::parseJsonKeyValue(state);
    for (auto &player : speakers) {
      player->sources.assign(sources.begin(), sources.end());
    }
    sonosFavorites = sources;
  }

  void playlists_changed(std::string state) {
    stripUnicode(state);
    ESP_LOGI("group", "Spotify playlists changes to %s", state.c_str());
    auto sources = TextHelpers::parseJsonSource(state, "name", "uri");
    for (auto &player : speakers) {
      player->sources.assign(sources.begin(), sources.end());
    }
    spotifyPlaylists = sources;
  }
};
