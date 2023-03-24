#include "HomeAssistantMediaPlayerGroup.h"
#include <algorithm>

namespace esphome {
namespace homeassistant_media_player {

static const char* const TAG = "homeassistant.media_player_group";

void HomeAssistantMediaPlayerGroup::setup() {
  ESP_LOGI(TAG, "Subscribe states");
}

void HomeAssistantMediaPlayerGroup::register_media_player(
    HomeAssistantBaseMediaPlayer* new_media_player) {
  new_media_player->index = media_players_.size();
  media_players_.push_back(new_media_player);

  if (new_media_player->get_player_type() ==
      homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType) {
    HomeAssistantSpeakerMediaPlayer* sonos_speaker =
        static_cast<HomeAssistantSpeakerMediaPlayer*>(new_media_player);
    if (!sonos_active && sonos_speaker) {
      sonos_active = true;
      subscribe_homeassistant_state(
          &HomeAssistantMediaPlayerGroup::sonos_favorites_changed,
          "sensor.sonos_favorites", "items");
    }

    if (!spotify_active && sonos_speaker) {
      spotify_active = true;
      subscribe_homeassistant_state(
          &HomeAssistantMediaPlayerGroup::playlists_changed,
          "sensor.playlists_sensor", "playlists");
    }
  }

  new_media_player->add_on_state_callback([this, new_media_player]() {
    this->state_updated(new_media_player->playerState);
  });
}

bool HomeAssistantMediaPlayerGroup::selectMediaPlayers(
    HomeAssistantBaseMediaPlayer* selected_media_player) {
  ESP_LOGI(TAG, "selecting media player %s",
           selected_media_player->entity_id_.c_str());
  for (auto& media_player : media_players_) {
    ESP_LOGI(TAG, "selecting mediar player? %s",
             media_player->entity_id_.c_str());

    if (media_player->entity_id_ == selected_media_player->entity_id_) {
      setActivePlayer(media_player);
      return true;
    }
  }
  return false;
}

void HomeAssistantMediaPlayerGroup::selectFirstActivePlayer() {
  if (playerSearchFinished || loadedPlayers < 1) {
    return;
  }
  for (auto& speaker : media_players_) {
    if (speaker->playerState != NoRemotePlayerState) {
      playerSearchFinished = true;
      setActivePlayer(speaker);
      // display->updateDisplay(true);
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
      HomeAssistantSpeakerMediaPlayer* speaker =
          static_cast<HomeAssistantSpeakerMediaPlayer*>(media_player);
      if (speaker->playerState == NoRemotePlayerState) {
        ESP_LOGD(TAG, "findActivePlayer: didnt load player %s",
                 media_player->entity_id_.c_str());
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
      ESP_LOGD(TAG, "findActivePlayer: loaded player %s",
               media_player->entity_id_.c_str());
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
      ESP_LOGD(TAG, "findActivePlayer: loaded player %s",
               media_player->entity_id_.c_str());
      loadedPlayers = max(tempLoadedPlayers, loadedPlayers);
    }
  }
  if (newActivePlayer != NULL) {
    ESP_LOGI(TAG, "setting active player %s",
             newActivePlayer->entity_id_.c_str());
    setActivePlayer(newActivePlayer);
    playerSearchFinished = true;
    if (!background) {
      // display->updateDisplay(true);
    }
  }
}

void HomeAssistantMediaPlayerGroup::setActivePlayer(
    HomeAssistantBaseMediaPlayer* newActivePlayer) {
  ESP_LOGI(TAG, "New active player %s", newActivePlayer->entity_id_.c_str());
  active_player_ = newActivePlayer;
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
  if (active_player_->get_player_type() ==
      homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType) {
    HomeAssistantSpeakerMediaPlayer* activeSpeaker =
        static_cast<HomeAssistantSpeakerMediaPlayer*>(active_player_);
    if (activeSpeaker != NULL) {
      activeSpeaker->increaseVolume();
    }
  } else {
    HomeAssistantRokuMediaPlayer* activeTV =
        static_cast<HomeAssistantRokuMediaPlayer*>(active_player_);
    if (activeTV != NULL) {
      if (activeTV->speaker != NULL) {
        HomeAssistantSpeakerMediaPlayer* tvSpeaker =
            static_cast<HomeAssistantSpeakerMediaPlayer*>(activeTV->speaker);
        tvSpeaker->increaseVolume();
      } else {
        activeTV->increaseVolume();
      }
    }
  }
}

void HomeAssistantMediaPlayerGroup::decreaseSpeakerVolume() {
  if (active_player_->get_player_type() ==
      homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType) {
    HomeAssistantSpeakerMediaPlayer* activeSpeaker =
        static_cast<HomeAssistantSpeakerMediaPlayer*>(active_player_);
    if (activeSpeaker != NULL) {
      activeSpeaker->decreaseVolume();
    }
  } else {
    HomeAssistantRokuMediaPlayer* activeTV =
        static_cast<HomeAssistantRokuMediaPlayer*>(active_player_);
    if (activeTV != NULL) {
      if (activeTV->speaker != NULL) {
        HomeAssistantSpeakerMediaPlayer* tvSpeaker =
            static_cast<HomeAssistantSpeakerMediaPlayer*>(activeTV->speaker);
        tvSpeaker->decreaseVolume();
      } else {
        activeTV->decreaseVolume();
      }
    }
  }
}

bool HomeAssistantMediaPlayerGroup::mediaShuffling() {
  HomeAssistantSpeakerMediaPlayer* activeSpeaker =
      static_cast<HomeAssistantSpeakerMediaPlayer*>(active_player_);
  if (activeSpeaker != NULL) {
    if (activeSpeaker->is_shuffling()) {
      return true;
    } else {
      return false;
    }
  }
  return false;
}

void HomeAssistantMediaPlayerGroup::toggle_shuffle() {
  HomeAssistantSpeakerMediaPlayer* activeSpeaker =
      static_cast<HomeAssistantSpeakerMediaPlayer*>(active_player_);
  if (activeSpeaker != NULL) {
    activeSpeaker->toggle_shuffle();
  }
}

void HomeAssistantMediaPlayerGroup::toggle_mute() {
  HomeAssistantSpeakerMediaPlayer* activeSpeaker =
      static_cast<HomeAssistantSpeakerMediaPlayer*>(active_player_);
  if (activeSpeaker != NULL) {
    activeSpeaker->toggle_mute();
  }
}

std::string HomeAssistantMediaPlayerGroup::shuffle_string() {
  if (mediaShuffling()) {
    return "Shfl On";
  } else {
    return "Shfl Off";
  }
}

std::string HomeAssistantMediaPlayerGroup::muteString() {
  HomeAssistantSpeakerMediaPlayer* activeSpeaker =
      static_cast<HomeAssistantSpeakerMediaPlayer*>(active_player_);
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
  if (active_player_ == nullptr) {
    return 0;
  }
  if (active_player_->get_player_type() ==
      homeassistant_media_player::RemotePlayerType::TVRemotePlayerType) {
    HomeAssistantRokuMediaPlayer* activeTV =
        static_cast<HomeAssistantRokuMediaPlayer*>(active_player_);
    if (activeTV != NULL) {
      if (activeTV->speaker != NULL) {
        HomeAssistantSpeakerMediaPlayer* tvSpeaker =
            static_cast<HomeAssistantSpeakerMediaPlayer*>(activeTV->speaker);
        if (tvSpeaker->volume != -1) {
          double volume = tvSpeaker->volume * 100;
          return volume;
        }
      } else {
        return -1;
      }
    }
  } else {
    HomeAssistantSpeakerMediaPlayer* activeSpeaker =
        static_cast<HomeAssistantSpeakerMediaPlayer*>(active_player_);
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

  std::vector<HomeAssistantSpeakerMediaPlayer*> speakerList;
  for (auto& media_player : media_players_) {
    if (media_player->entity_id_ == newSpeakerGroupParent->entity_id_ ||
        media_player->get_player_type() != SpeakerRemotePlayerType) {
      continue;
    }
    HomeAssistantSpeakerMediaPlayer* speaker =
        static_cast<HomeAssistantSpeakerMediaPlayer*>(media_player);
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
    HomeAssistantSpeakerMediaPlayer* speaker =
        static_cast<HomeAssistantSpeakerMediaPlayer*>(media_player);
    if (speaker->playerState == PlayingRemotePlayerState &&
        speaker->mediaDuration >= 0 && speaker->mediaPosition >= 0 &&
        speaker->mediaPosition < speaker->mediaDuration) {
      speaker->mediaPosition++;
      updateDisplay = true;
    }
  }
  if (active_player_ != NULL) {
    switch (active_player_->get_player_type()) {
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
  if (active_player_ != NULL) {
    ESP_LOGI(TAG, "Syncing active player %d, %d", active_player_->index,
             media_players_.size());
    if (active_player_->index < media_players_.size() - 1) {
      ESP_LOGI(TAG, "32Syncing active player %d", active_player_->index);
      setActivePlayer(media_players_[active_player_->index + 1]);
    } else if (media_players_.size() > 0) {
      ESP_LOGI(TAG, "323 Syncing active player %d", active_player_->index);
      setActivePlayer(media_players_[0]);
    } else {
      ESP_LOGI(TAG, "132Syncing active player %d", active_player_->index);
    }
  }
}

std::string HomeAssistantMediaPlayerGroup::playTitleString() {
  switch (active_player_->playerState) {
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
  switch (active_player_->get_player_type()) {
    case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
      return active_player_->mediaTitleString();
    case homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType:
      if (active_player_->mediaSource == TVRemotePlayerMediaSource) {
        HomeAssistantSpeakerMediaPlayer* activeSpeaker =
            static_cast<HomeAssistantSpeakerMediaPlayer*>(active_player_);
        if (activeSpeaker != NULL && activeSpeaker->tv != NULL) {
          return activeSpeaker->tv->mediaTitleString();
        }
      }
      return active_player_->mediaTitleString();
  }
  return "";
}

std::string HomeAssistantMediaPlayerGroup::mediaSubtitleString() {
  switch (active_player_->get_player_type()) {
    case homeassistant_media_player::RemotePlayerType::TVRemotePlayerType:
      return active_player_->mediaSubtitleString();
    case homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType:
      if (active_player_->mediaSource == TVRemotePlayerMediaSource) {
        return playerSourceStateString(active_player_->mediaSource);
      }
      return active_player_->mediaSubtitleString();
  }
  return "";
}

void HomeAssistantMediaPlayerGroup::sendActivePlayerRemoteCommand(
    std::string command) {
  HomeAssistantRokuMediaPlayer* activeTV =
      static_cast<HomeAssistantRokuMediaPlayer*>(active_player_);
  if (activeTV != NULL) {
    activeTV->tvRemoteCommand(command);
  }
}

void HomeAssistantMediaPlayerGroup::call_feature(
    MediaPlayerSupportedFeature feature) {
  switch (feature) {
    case TURN_ON:
    case TURN_OFF:
      sendActivePlayerRemoteCommand("power");
      break;
    case SHUFFLE_SET:
      toggle_shuffle();
      break;
    case VOLUME_MUTE:
      toggle_mute();
      break;
    case TV_BACK:
      sendActivePlayerRemoteCommand("back");
      break;
    case TV_HOME:
      sendActivePlayerRemoteCommand("home");
      break;
    case MENU_HOME:
    case PAUSE:
    case VOLUME_SET:
    case SEEK:
    case SELECT_SOURCE:
    case CLEAR_PLAYLIST:
    case GROUPING:
    case SELECT_SOUND_MODE:
    case BROWSE_MEDIA:
    case REPEAT_SET:
    case PLAY_MEDIA:
    case VOLUME_STEP:
    case STOP:
    case PLAY:
    case PREVIOUS_TRACK:
    case NEXT_TRACK:
    default:
      break;
  }
}

std::vector<std::shared_ptr<MediaPlayerSource>>
HomeAssistantMediaPlayerGroup::activePlayerSources() {
  return active_player_->sources;
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
  ESP_LOGD(TAG, "state update callback %d %d", activePlayer == NULL,
           sync_active_player);
  if (active_player_ != NULL) {
    return;
  } else if (!sync_active_player) {
    publish_state(0);
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
      break;
    case homeassistant_media_player::RemotePlayerState::
        PlayingRemotePlayerState:
      if (active_player_ == NULL) {
        ESP_LOGD(TAG, "Trying to sync active player - 2");
        syncActivePlayer(state);
      } else if (active_player_->playerState < state) {
        ESP_LOGD(TAG, "Trying to sync active player - 3 %d",
                 active_player_->playerState);
        syncActivePlayer(state);
      }
      break;
    case homeassistant_media_player::RemotePlayerState::
        PowerOffRemotePlayerState:
    case homeassistant_media_player::RemotePlayerState::
        StoppedRemotePlayerState:
      ESP_LOGD(TAG, "Trying to sync active player - 4");
      syncActivePlayer(state);
  }
  publish_state(0);
}

void HomeAssistantMediaPlayerGroup::playSource(MediaPlayerSource source) {
  active_player_->clearSource();
  playingNewSourceText = source.title_;
  if (active_player_->get_player_type() ==
      homeassistant_media_player::RemotePlayerType::SpeakerRemotePlayerType) {
    HomeAssistantSpeakerMediaPlayer* activeSpeaker =
        static_cast<HomeAssistantSpeakerMediaPlayer*>(active_player_);
    activeSpeaker->playlist_title = source.title_;
  }
  active_player_->playSource(source);
}

// should use same json and just set the source type
void HomeAssistantMediaPlayerGroup::sonos_favorites_changed(std::string state) {
  ESP_LOGI(TAG, "Sonos Favorites changes to %s", state.c_str());
  auto sources = parseJsonObject(state);
  for (auto& player : media_players_) {
    if (player->get_player_type() == SpeakerRemotePlayerType) {
      player->sources.assign(sources.begin(), sources.end());
    }
  }
  sonosFavorites = sources;
}

void HomeAssistantMediaPlayerGroup::playlists_changed(std::string state) {
  stripUnicode(&state);
  ESP_LOGI(TAG, "Spotify playlists changes to %s", state.c_str());
  auto sources = parseJsonDictionary(state, "name", "uri");
  for (auto& player : media_players_) {
    player->sources.assign(sources.begin(), sources.end());
  }
  spotifyPlaylists = sources;
}
}  // namespace homeassistant_media_player
}  // namespace esphome
