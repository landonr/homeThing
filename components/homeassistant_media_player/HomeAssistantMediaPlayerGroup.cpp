#include "HomeAssistantMediaPlayerGroup.h"
#include <algorithm>

namespace esphome {
namespace homeassistant_media_player {

static const char* const TAG = "homeassistant.media_player_group";

void HomeAssistantMediaPlayerGroup::setup() {
  ESP_LOGI(TAG, "Subscribe states");

  subscribe_homeassistant_state(
      &HomeAssistantMediaPlayerGroup::playlists_changed,
      "sensor.playlists_sensor", "playlists");
  subscribe_homeassistant_state(
      &HomeAssistantMediaPlayerGroup::sonos_favorites_changed,
      "sensor.sonos_favorites", "items");
}

void HomeAssistantMediaPlayerGroup::register_media_player(
    homeassistant_media_player::HomeAssistantBaseMediaPlayer*
        new_media_player) {
  media_players_.push_back(new_media_player);
  new_media_player->add_on_state_callback([this, new_media_player]() {
    if (this->display != NULL) {
      this->display->updateDisplay(false);
    }
    this->state_updated(new_media_player->playerState);
  });
}

void HomeAssistantMediaPlayerGroup::selectFirstActivePlayer() {
  if (playerSearchFinished || loadedPlayers < 1) {
    return;
  }
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
    if (media_player->get_player_type() ==
        homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType) {
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
  if (activePlayer->get_player_type() ==
      homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType) {
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
  if (activePlayer->get_player_type() ==
      homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType) {
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
    if (activeSpeaker->is_muted()) {
      return "Unmute";
    } else {
      return "Mute";
    }
  }
  return "";
}

double HomeAssistantMediaPlayerGroup::getVolumeLevel() {
  if (activePlayer->get_player_type() ==
      homeassistant_media_player::RemotePlayerType::TVRemotePlayerType) {
    HomeAssistantRokuMediaPlayer* activeTV =
        static_cast<HomeAssistantRokuMediaPlayer*>(activePlayer);
    if (activeTV != NULL) {
      if (activeTV->speaker != NULL) {
        HomeAssistantSonosMediaPlayer* tvSpeaker =
            static_cast<HomeAssistantSonosMediaPlayer*>(activeTV->speaker);
        if (tvSpeaker->volume != -1) {
          double volume = tvSpeaker->volume * 100;
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
      if (activeSpeaker->volume != -1) {
        double volume = activeSpeaker->volume * 100;
        return volume;
      }
    }
  }
  return 0;
}

void HomeAssistantMediaPlayerGroup::selectGroup(
    HomeAssistantBaseMediaPlayer* selected_group_parent, int menuIndex) {
  if (newSpeakerGroupParent == nullptr) {
    newSpeakerGroupParent = selected_group_parent;
    return;
  } else if (menuIndex == 0) {
    newSpeakerGroupParent = nullptr;
    return;
  }

  std::vector<HomeAssistantSonosMediaPlayer*> speakerList;
  for (auto& media_player : media_players_) {
    if (media_player->entity_id_ == newSpeakerGroupParent->entity_id_ ||
        media_player->get_player_type() != SpeakerRemotePlayerType) {
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
    if (media_player->get_player_type() != SpeakerRemotePlayerType) {
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
    switch (activePlayer->get_player_type()) {
      case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
        updateDisplay = false;
        break;
      case homeassistant_media_player::RemotePlayerType::
          SpeakerRemotePlayerType:
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
    case homeassistant_media_player::RemotePlayerState::PausedRemotePlayerState:
      return "Play";
    case homeassistant_media_player::RemotePlayerState::
        PlayingRemotePlayerState:
      return "Pause";
    case homeassistant_media_player::RemotePlayerState::
        StoppedRemotePlayerState:
    case homeassistant_media_player::RemotePlayerState::NoRemotePlayerState:
      return "Stopped";
    case homeassistant_media_player::RemotePlayerState::
        PowerOffRemotePlayerState:
      return "Off";
    case homeassistant_media_player::RemotePlayerState::
        UnavailableRemotePlayerState:
      return "Unavailable";
  }
  return "";
}

std::string HomeAssistantMediaPlayerGroup::mediaTitleString() {
  switch (activePlayer->get_player_type()) {
    case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
      return activePlayer->mediaTitleString();
    case homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType:
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
  switch (activePlayer->get_player_type()) {
    case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
      return activePlayer->mediaSubtitleString();
    case homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType:
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

std::vector<std::shared_ptr<MediaPlayerSource>>
HomeAssistantMediaPlayerGroup::activePlayerSources() {
  return activePlayer->sources;
}

// private

void HomeAssistantMediaPlayerGroup::syncActivePlayer(RemotePlayerState state) {
  if (loadedPlayers < totalPlayers()) {
    return;
  }
  ESP_LOGI(TAG, "Syncing active player %d", state);
  playerSearchFinished = false;
  findActivePlayer(true);
}

void HomeAssistantMediaPlayerGroup::state_updated(RemotePlayerState state) {
  ESP_LOGI(TAG, "state update callback %d %d", activePlayer == NULL,
           sync_active_player);
  if (activePlayer != NULL || !sync_active_player) {
    return;
  }

  ESP_LOGD(TAG,
           "Trying to sync active player, state: %d activePlayerNull: %d, "
           "sync_active_player: %d",
           state, activePlayer == NULL, sync_active_player == true);
  switch (state) {
    case homeassistant_media_player::RemotePlayerState::NoRemotePlayerState:
    case homeassistant_media_player::RemotePlayerState::PausedRemotePlayerState:
    case homeassistant_media_player::RemotePlayerState::
        UnavailableRemotePlayerState:
      ESP_LOGD(TAG, "Trying to sync active player - 1");
      syncActivePlayer(state);
      return;
    case homeassistant_media_player::RemotePlayerState::
        PlayingRemotePlayerState:
      if (activePlayer == NULL) {
        ESP_LOGD(TAG, "Trying to sync active player - 2");
        syncActivePlayer(state);
      } else if (activePlayer->playerState < state) {
        ESP_LOGD(TAG, "Trying to sync active player - 3 %d",
                 activePlayer->playerState);
        syncActivePlayer(state);
      }
      return;
    case homeassistant_media_player::RemotePlayerState::
        PowerOffRemotePlayerState:
    case homeassistant_media_player::RemotePlayerState::
        StoppedRemotePlayerState:
      ESP_LOGD(TAG, "Trying to sync active player - 4");
      syncActivePlayer(state);
  }
}

void HomeAssistantMediaPlayerGroup::playSource(MediaPlayerSource source) {
  activePlayer->clearSource();
  playingNewSourceText = source.title_;
  if (activePlayer->get_player_type() ==
      homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType) {
    HomeAssistantSonosMediaPlayer* activeSpeaker =
        static_cast<HomeAssistantSonosMediaPlayer*>(activePlayer);
    activeSpeaker->mediaPlaylist = source.title_;
  }
  activePlayer->playSource(source);
}

void HomeAssistantMediaPlayerGroup::sonos_favorites_changed(std::string state) {
  ESP_LOGI(TAG, "Sonos Favorites changes to %s", state.c_str());
  auto sources = parseJsonKeyValue(state);
  for (auto& player : media_players_) {
    player->sources.assign(sources.begin(), sources.end());
  }
  sonosFavorites = sources;
}

void HomeAssistantMediaPlayerGroup::playlists_changed(std::string state) {
  stripUnicode(&state);
  ESP_LOGI(TAG, "Spotify playlists changes to %s", state.c_str());
  auto sources = parseJsonSource(state, "name", "uri");
  for (auto& player : media_players_) {
    player->sources.assign(sources.begin(), sources.end());
  }
  spotifyPlaylists = sources;
}
}  // namespace homeassistant_media_player
}  // namespace esphome
