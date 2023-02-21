#include "HomeAssistantMediaPlayerGroup.h"
#include <algorithm>

namespace esphome {
namespace homeassistant_media_player {

static const char* const TAG = "homeassistant.media_player_roku";

void HomeAssistantMediaPlayerGroup::selectFirstActivePlayer() {
  if (playerSearchFinished || loadedPlayers < 1) {
    return;
  }
  // for (auto& tv : tvs) {
  //   if (tv->playerState != NoRemotePlayerState) {
  //     playerSearchFinished = true;
  //     setActivePlayer(tv);
  //     display->updateDisplay(true);
  //     return;
  //   }
  // }
  for (auto& speaker : media_players_) {
    if (speaker->playerState != NoRemotePlayerState) {
      playerSearchFinished = true;
      setActivePlayer(speaker);
      display->updateDisplay(true);
      return;
    }
  }
}

void HomeAssistantMediaPlayerGroup::findActivePlayer(bool background) {
  if (playerSearchFinished) {
    return;
  }
  HomeAssistantBaseMediaPlayer* newActivePlayer = NULL;
  int tempLoadedPlayers = 0;
  for (auto& media_player : media_players_) {
    if (media_player->playerType != TVRemotePlayerType) {
      HomeAssistantSonosMediaPlayer* speaker =
          static_cast<HomeAssistantSonosMediaPlayer*>(media_player);
      if (speaker->playerState == NoRemotePlayerState) {
        return;
      } else if (speaker->tv != NULL &&
                 speaker->mediaSource == TVRemotePlayerMediaSource &&
                 newActivePlayer->playerState < speaker->tv->playerState) {
        newActivePlayer = speaker->tv;
      } else if (newActivePlayer != NULL) {
        if (newActivePlayer->playerState < speaker->playerState &&
            speaker->mediaSource != TVRemotePlayerMediaSource) {
          newActivePlayer = speaker;
        }
      } else {
        newActivePlayer = speaker;
      }
      tempLoadedPlayers++;
      loadedPlayers = max(tempLoadedPlayers, loadedPlayers);
    } else {
      HomeAssistantRokuMediaPlayer* tv =
          static_cast<HomeAssistantRokuMediaPlayer*>(media_player);
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
  }
  if (newActivePlayer != NULL) {
    ESP_LOGI(TAG, "setting active player %s",
             newActivePlayer->entity_id_.c_str());
    setActivePlayer(newActivePlayer);
    playerSearchFinished = true;
    if (!background) {
      display->updateDisplay(true);
    }
  }
}

void HomeAssistantMediaPlayerGroup::setActivePlayer(
    HomeAssistantBaseMediaPlayer* newActivePlayer) {
  ESP_LOGI(TAG, "New active player %s", newActivePlayer->entity_id_.c_str());
  activePlayer = newActivePlayer;
}

void HomeAssistantMediaPlayerGroup::setup() {
  // bool first = true;
  // playerSearchFinished = false;

  // int tvIndex = 0;
  // int speakerIndex = 0;
  // for (auto newTVSetup : newTVSetups) {
  //   HomeAssistantSonosMediaPlayer* newSoundBar = NULL;
  //   if (newTVSetup.soundBar != NULL) {
  //     auto newSpeaker = new HomeAssistantSonosMediaPlayer(
  //         display, this, newTVSetups.size() + speakerIndex,
  //         *newTVSetup.soundBar);
  //     speakers.push_back(newSpeaker);
  //     speakerIndex++;
  //     newSoundBar = newSpeaker;
  //   }
  //   auto newTV = new HomeAssistantRokuMediaPlayer(display, this, tvIndex,
  //                                                 newTVSetup, newSoundBar);
  //   if (newSoundBar != NULL) {
  //     newSoundBar->tv = newTV;
  //   }
  //   tvs.push_back(newTV);
  //   tvIndex++;
  // }
  // for (auto newSpeakerSetup : newSpeakerSetups) {
  //   auto newSpeaker = new HomeAssistantSonosMediaPlayer(
  //       display, this, newTVSetups.size() + speakerIndex, newSpeakerSetup);
  //   speakers.push_back(newSpeaker);
  //   speakerIndex++;
  // }
  // subscribe_homeassistant_state(&HomeAssistantMediaPlayerGroup::playlists_changed,
  //                               "sensor.playlists_sensor", "playlists");
  // subscribe_homeassistant_state(
  //     &HomeAssistantMediaPlayerGroup::sonos_favorites_changed,
  //     "sensor.sonos_favorites", "items");
}

void HomeAssistantMediaPlayerGroup::register_media_player(
    homeassistant_media_player::HomeAssistantBaseMediaPlayer*
        new_media_player) {
  media_players_.push_back(new_media_player);
  new_media_player->add_on_state_callback([this, new_media_player]() {
    if (this->display != NULL) {
      this->display->updateDisplay(false);
    }
  });
}

std::vector<std::string> HomeAssistantMediaPlayerGroup::groupNames() {
  std::vector<std::string> output;
  for (auto& speaker : media_players_) {
    output.push_back(speaker->get_name());
  }
  return output;
}

std::string HomeAssistantMediaPlayerGroup::friendlyNameForEntityId(
    std::string speakerentityId) {
  for (auto& speaker : media_players_) {
    if (speaker->entity_id_ == speakerentityId) {
      return speaker->get_name();
    }
  }
  return "";
}

void HomeAssistantMediaPlayerGroup::stripUnicode(std::string* str) {
  str->erase(remove_if(str->begin(), str->end(),
                       [](char c) { return !(c >= 0 && c < 128); }),
             str->end());
}

void HomeAssistantMediaPlayerGroup::increaseSpeakerVolume() {
  if (activePlayer->playerType != TVRemotePlayerType) {
    HomeAssistantSonosMediaPlayer* activeSpeaker =
        static_cast<HomeAssistantSonosMediaPlayer*>(activePlayer);
    if (activeSpeaker != NULL) {
      activeSpeaker->increaseVolume();
    }
  } else {
    HomeAssistantRokuMediaPlayer* activeTV =
        static_cast<HomeAssistantRokuMediaPlayer*>(activePlayer);
    if (activeTV != NULL) {
      if (activeTV->speaker != NULL) {
        HomeAssistantSonosMediaPlayer* tvSpeaker =
            static_cast<HomeAssistantSonosMediaPlayer*>(activeTV->speaker);
        tvSpeaker->increaseVolume();
      } else {
        activeTV->increaseVolume();
      }
    }
  }
}

void HomeAssistantMediaPlayerGroup::decreaseSpeakerVolume() {
  if (activePlayer->playerType != TVRemotePlayerType) {
    HomeAssistantSonosMediaPlayer* activeSpeaker =
        static_cast<HomeAssistantSonosMediaPlayer*>(activePlayer);
    if (activeSpeaker != NULL) {
      activeSpeaker->decreaseVolume();
    }
  } else {
    HomeAssistantRokuMediaPlayer* activeTV =
        static_cast<HomeAssistantRokuMediaPlayer*>(activePlayer);
    if (activeTV != NULL) {
      if (activeTV->speaker != NULL) {
        HomeAssistantSonosMediaPlayer* tvSpeaker =
            static_cast<HomeAssistantSonosMediaPlayer*>(activeTV->speaker);
        tvSpeaker->decreaseVolume();
      } else {
        activeTV->decreaseVolume();
      }
    }
  }
}

bool HomeAssistantMediaPlayerGroup::mediaShuffling() {
  HomeAssistantSonosMediaPlayer* activeSpeaker =
      static_cast<HomeAssistantSonosMediaPlayer*>(activePlayer);
  if (activeSpeaker != NULL) {
    if (activeSpeaker->shuffle) {
      return true;
    } else {
      return false;
    }
  }
  return false;
}

void HomeAssistantMediaPlayerGroup::toggleShuffle() {
  HomeAssistantSonosMediaPlayer* activeSpeaker =
      static_cast<HomeAssistantSonosMediaPlayer*>(activePlayer);
  if (activeSpeaker != NULL) {
    activeSpeaker->toggleShuffle();
  }
}

void HomeAssistantMediaPlayerGroup::toggleMute() {
  HomeAssistantSonosMediaPlayer* activeSpeaker =
      static_cast<HomeAssistantSonosMediaPlayer*>(activePlayer);
  if (activeSpeaker != NULL) {
    activeSpeaker->toggleMute();
  }
}

std::string HomeAssistantMediaPlayerGroup::shuffleString() {
  if (mediaShuffling()) {
    return "Shfl On";
  } else {
    return "Shfl Off";
  }
}

std::string HomeAssistantMediaPlayerGroup::muteString() {
  HomeAssistantSonosMediaPlayer* activeSpeaker =
      static_cast<HomeAssistantSonosMediaPlayer*>(activePlayer);
  if (activeSpeaker != NULL) {
    if (activeSpeaker->muted) {
      return "Unmute";
    } else {
      return "Mute";
    }
  }
  return "";
}

double HomeAssistantMediaPlayerGroup::getVolumeLevel() {
  if (activePlayer->playerType == TVRemotePlayerType) {
    HomeAssistantRokuMediaPlayer* activeTV =
        static_cast<HomeAssistantRokuMediaPlayer*>(activePlayer);
    if (activeTV != NULL) {
      if (activeTV->speaker != NULL) {
        HomeAssistantSonosMediaPlayer* tvSpeaker =
            static_cast<HomeAssistantSonosMediaPlayer*>(activeTV->speaker);
        if (tvSpeaker->speaker_volume != -1) {
          double volume = tvSpeaker->localVolume * 100;
          return volume;
        }
      } else {
        return -1;
      }
    }
  } else {
    HomeAssistantSonosMediaPlayer* activeSpeaker =
        static_cast<HomeAssistantSonosMediaPlayer*>(activePlayer);
    if (activeSpeaker != NULL) {
      if (activeSpeaker->speaker_volume != -1) {
        double volume = activeSpeaker->localVolume * 100;
        return volume;
      }
    }
  }
  return 0;
}

std::vector<MenuTitlePlayer*>
HomeAssistantMediaPlayerGroup::groupTitleSwitches() {
  std::vector<MenuTitlePlayer*> out;
  std::vector<std::string> groupedMembers;
  out.push_back(new MenuTitlePlayer(
      "Group " + newSpeakerGroupParent->get_name(),
      newSpeakerGroupParent->entity_id_, NoMenuTitleLeftIcon,
      ArrowMenuTitleRightIcon, newSpeakerGroupParent->mediaSource,
      newSpeakerGroupParent->playerState));

  for (auto& media_player : media_players_) {
    if (newSpeakerGroupParent->entity_id_ == media_player->entity_id_ ||
        media_player->playerType != SpeakerRemotePlayerType) {
      continue;
    } else {
      HomeAssistantSonosMediaPlayer* speaker =
          static_cast<HomeAssistantSonosMediaPlayer*>(media_player);
      if (std::find(speaker->groupMembers.begin(), speaker->groupMembers.end(),
                    newSpeakerGroupParent->entity_id_) !=
          speaker->groupMembers.end()) {
        out.push_back(new MenuTitlePlayer(
            speaker->get_name(), speaker->entity_id_, OnMenuTitleLeftIcon,
            NoMenuTitleRightIcon, speaker->mediaSource, speaker->playerState));
      } else {
        out.push_back(new MenuTitlePlayer(
            speaker->get_name(), speaker->entity_id_, OffMenuTitleLeftIcon,
            NoMenuTitleRightIcon, speaker->mediaSource, speaker->playerState));
      }
    }
  }
  return out;
}

std::vector<MenuTitlePlayer*>
HomeAssistantMediaPlayerGroup::groupTitleString() {
  std::vector<MenuTitlePlayer*> out;
  std::vector<std::string> groupedMembers;
  for (auto& media_player : media_players_) {
    if (std::find(groupedMembers.begin(), groupedMembers.end(),
                  media_player->entity_id_) != groupedMembers.end() ||
        media_player->playerType != SpeakerRemotePlayerType) {
      // skip grouped members that were already found
      continue;
    }
    HomeAssistantSonosMediaPlayer* speaker =
        static_cast<HomeAssistantSonosMediaPlayer*>(media_player);
    if (speaker->groupMembers.size() > 1) {
      if (speaker->groupMembers[0] != speaker->entity_id_) {
        ESP_LOGD(TAG, "%s not parent %s", speaker->groupMembers[0].c_str(),
                 speaker->entity_id_.c_str());
        // speaker isn't the group parent
        continue;
      }
      out.push_back(new MenuTitlePlayer(
          speaker->get_name(), speaker->entity_id_, NoMenuTitleLeftIcon,
          ArrowMenuTitleRightIcon, speaker->mediaSource, speaker->playerState));
      for (auto& groupedSpeaker : speaker->groupMembers) {
        if (groupedSpeaker != speaker->entity_id_) {
          groupedMembers.push_back(groupedSpeaker);
          std::string groupedSpeakerName =
              friendlyNameForEntityId(groupedSpeaker);
          out.push_back(new MenuTitlePlayer(
              groupedSpeakerName, groupedSpeaker, GroupedMenuTitleLeftIcon,
              ArrowMenuTitleRightIcon, speaker->mediaSource,
              speaker->playerState));
        }
      }
    } else {
      out.push_back(new MenuTitlePlayer(
          speaker->get_name(), speaker->entity_id_, NoMenuTitleLeftIcon,
          ArrowMenuTitleRightIcon, speaker->mediaSource, speaker->playerState));
    }
  }
  return out;
}

std::vector<MenuTitlePlayer*>
HomeAssistantMediaPlayerGroup::mediaPlayersTitleString() {
  std::vector<MenuTitlePlayer*> out;
  for (auto& speaker : media_players_) {
    if (speaker->mediaSource != TVRemotePlayerMediaSource) {
      out.push_back(new MenuTitlePlayer(
          speaker->get_name(), speaker->entity_id_, NoMenuTitleLeftIcon,
          NoMenuTitleRightIcon, speaker->mediaSource, speaker->playerState));
    }
  }
  // for (auto& tv : tvs) {
  //   out.push_back(new MenuTitlePlayer(tv->get_name(), tv->entity_id_,
  //                                     NoMenuTitleLeftIcon, NoMenuTitleRightIcon,
  //                                     tv->mediaSource, tv->playerState));
  //   if (tv->speaker != NULL &&
  //       tv->speaker->mediaSource == TVRemotePlayerMediaSource) {
  //     out.push_back(new MenuTitlePlayer(
  //         tv->speaker->get_name(), tv->speaker->entity_id_,
  //         GroupedMenuTitleLeftIcon, NoMenuTitleRightIcon,
  //         tv->speaker->mediaSource, tv->speaker->playerState));
  //   }
  // }
  return out;
}

MenuTitlePlayer HomeAssistantMediaPlayerGroup::headerMediaPlayerTitle() {
  std::string friendlyName = activePlayer->get_name();
  if (activePlayer->playerType != TVRemotePlayerType) {
    HomeAssistantSonosMediaPlayer* activeSpeaker =
        static_cast<HomeAssistantSonosMediaPlayer*>(activePlayer);
    if (activeSpeaker != NULL) {
      if (activeSpeaker->groupMembers.size() > 1) {
        friendlyName = friendlyName + " + " +
                       to_string(activeSpeaker->groupMembers.size() - 1);
      }
    }
  }
  return MenuTitlePlayer(friendlyName, activePlayer->entity_id_,
                         NoMenuTitleLeftIcon, NoMenuTitleRightIcon,
                         activePlayer->mediaSource, activePlayer->playerState);
}

void HomeAssistantMediaPlayerGroup::selectGroup(
    MenuTitlePlayer selectedMenuTitle, int menuIndex) {
  if (newSpeakerGroupParent == NULL) {
    MenuTitlePlayer* selectedMenuTitleCopy = new MenuTitlePlayer(
        selectedMenuTitle.get_name(), selectedMenuTitle.entity_id_,
        selectedMenuTitle.leftIconState, selectedMenuTitle.rightIconState,
        selectedMenuTitle.mediaSource, selectedMenuTitle.playerState);
    newSpeakerGroupParent = selectedMenuTitleCopy;
    return;
  } else if (menuIndex == 0) {
    newSpeakerGroupParent = NULL;
    return;
  }

  std::vector<HomeAssistantSonosMediaPlayer*> speakerList;
  for (auto& media_player : media_players_) {
    if (media_player->entity_id_ == newSpeakerGroupParent->entity_id_ ||
        media_player->playerType != SpeakerRemotePlayerType) {
      continue;
    }
    HomeAssistantSonosMediaPlayer* speaker =
        static_cast<HomeAssistantSonosMediaPlayer*>(media_player);
    speakerList.push_back(speaker);
  }
  auto speaker = speakerList[menuIndex - 1];

  if (speaker->groupMembers.size() > 1) {
    if (std::find(speaker->groupMembers.begin(), speaker->groupMembers.end(),
                  newSpeakerGroupParent->entity_id_) !=
        speaker->groupMembers.end()) {
      speaker->ungroup();
    }
  } else {
    speaker->joinGroup(newSpeakerGroupParent->entity_id_);
  }
}

bool HomeAssistantMediaPlayerGroup::updateMediaPosition() {
  if (!playerSearchFinished) {
    return false;
  }
  bool updateDisplay = false;
  for (auto& media_player : media_players_) {
    if (media_player->playerType != SpeakerRemotePlayerType) {
      continue;
    }
    HomeAssistantSonosMediaPlayer* speaker =
        static_cast<HomeAssistantSonosMediaPlayer*>(media_player);
    if (speaker->playerState == PlayingRemotePlayerState &&
        speaker->mediaDuration >= 0 && speaker->mediaPosition >= 0 &&
        speaker->mediaPosition < speaker->mediaDuration) {
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

void HomeAssistantMediaPlayerGroup::selectNextMediaPlayer() {
  if (activePlayer != NULL) {
    // if (activePlayer->index < tvs.size() - 1) {
    //   setActivePlayer(tvs[activePlayer->index + 1]);
    // } else if (activePlayer->index - tvs.size() + 1 < speakers.size()) {
    //   setActivePlayer(speakers[activePlayer->index - tvs.size() + 1]);
    // } else if (tvs.size() > 0) {
    //   setActivePlayer(tvs[0]);
    // }
  }
}

std::string HomeAssistantMediaPlayerGroup::playTitleString() {
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

std::string HomeAssistantMediaPlayerGroup::mediaTitleString() {
  switch (activePlayer->playerType) {
    case TVRemotePlayerType:
      return activePlayer->mediaTitleString();
    case SpeakerRemotePlayerType:
      if (activePlayer->mediaSource == TVRemotePlayerMediaSource) {
        HomeAssistantSonosMediaPlayer* activeSpeaker =
            static_cast<HomeAssistantSonosMediaPlayer*>(activePlayer);
        if (activeSpeaker != NULL && activeSpeaker->tv != NULL) {
          return activeSpeaker->tv->mediaTitleString();
        }
      }
      return activePlayer->mediaTitleString();
  }
  return "";
}

std::string HomeAssistantMediaPlayerGroup::mediaSubtitleString() {
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

void HomeAssistantMediaPlayerGroup::sendActivePlayerRemoteCommand(
    std::string command) {
  HomeAssistantRokuMediaPlayer* activeTV =
      static_cast<HomeAssistantRokuMediaPlayer*>(activePlayer);
  if (activeTV != NULL) {
    activeTV->tvRemoteCommand(command);
  }
}

std::vector<std::shared_ptr<MenuTitleSource>>
HomeAssistantMediaPlayerGroup::activePlayerSourceMenu() {
  std::vector<std::shared_ptr<MenuTitleSource>> out;
  switch (activePlayer->playerType) {
    case TVRemotePlayerType:
      return activePlayer->sources;
    case SpeakerRemotePlayerType: {
      HomeAssistantSonosMediaPlayer* activeSpeaker =
          static_cast<HomeAssistantSonosMediaPlayer*>(activePlayer);
      if (activeSpeaker != NULL) {
        if (activeSpeaker->tv != NULL &&
            activeSpeaker->tv->get_name().size() > 0) {
          auto tvSource = std::make_shared<MenuTitleSource>(
              activeSpeaker->tv->get_name(), "", NoMenuTitleRightIcon,
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

void HomeAssistantMediaPlayerGroup::syncActivePlayer(RemotePlayerState state) {
  if (loadedPlayers < totalPlayers()) {
    return;
  }
  ESP_LOGI(TAG, "Syncing active player %d", state);
  playerSearchFinished = false;
  findActivePlayer(true);
}

void HomeAssistantMediaPlayerGroup::stateUpdated(RemotePlayerState state) {
  ESP_LOGD(TAG, "state update callback %d %d", activePlayer == NULL,
           sync_active_player);
  if (activePlayer != NULL || !sync_active_player) {
    return;
  }

  ESP_LOGD(TAG,
           "Trying to sync active player, state: %d activePlayerNull: %d, "
           "sync_active_player: %d",
           state, activePlayer == NULL, sync_active_player == true);
  switch (state) {
    case NoRemotePlayerState:
    case PausedRemotePlayerState:
    case UnavailableRemotePlayerState:
      ESP_LOGD(TAG, "Trying to sync active player - 1");
      syncActivePlayer(state);
      return;
    case PlayingRemotePlayerState:
      if (activePlayer == NULL) {
        ESP_LOGD(TAG, "Trying to sync active player - 2");
        syncActivePlayer(state);
      } else if (activePlayer->playerState < state) {
        ESP_LOGD(TAG, "Trying to sync active player - 3 %d",
                 activePlayer->playerState);
        syncActivePlayer(state);
      }
      return;
    case PowerOffRemotePlayerState:
    case StoppedRemotePlayerState:
      ESP_LOGD(TAG, "Trying to sync active player - 4");
      syncActivePlayer(state);
  }
}

void HomeAssistantMediaPlayerGroup::playSource(MenuTitleSource source) {
  activePlayer->clearSource();
  playingNewSourceText = source.get_name();
  if (activePlayer->playerType == SpeakerRemotePlayerType) {
    HomeAssistantSonosMediaPlayer* activeSpeaker =
        static_cast<HomeAssistantSonosMediaPlayer*>(activePlayer);
    activeSpeaker->mediaPlaylist = source.get_name();
  }
  activePlayer->playSource(source);
}

void HomeAssistantMediaPlayerGroup::sonos_favorites_changed(std::string state) {
  ESP_LOGI(TAG, "Sonos Favorites changes to %s", state.c_str());
  auto sources = TextHelpers::parseJsonKeyValue(state);
  for (auto& player : media_players_) {
    player->sources.assign(sources.begin(), sources.end());
  }
  sonosFavorites = sources;
}

void HomeAssistantMediaPlayerGroup::playlists_changed(std::string state) {
  stripUnicode(&state);
  ESP_LOGI(TAG, "Spotify playlists changes to %s", state.c_str());
  auto sources = TextHelpers::parseJsonSource(state, "name", "uri");
  for (auto& player : media_players_) {
    player->sources.assign(sources.begin(), sources.end());
  }
  spotifyPlaylists = sources;
}
}  // namespace homeassistant_media_player
}  // namespace esphome
