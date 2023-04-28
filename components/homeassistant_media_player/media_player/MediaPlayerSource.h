#pragma once

namespace esphome {
namespace homeassistant_media_player {

enum RemotePlayerMediaSource {
  NoRemotePlayerMediaSource,
  HomeRemotePlayerMediaSource,
  YouTubeRemotePlayerMediaSource,
  SpotifyRemotePlayerMediaSource,
  NetflixRemotePlayerMediaSource,
  PlexRemotePlayerMediaSource,
  TVRemotePlayerMediaSource
};

static std::string playerSourceStateString(
    RemotePlayerMediaSource playingState) {
  switch (playingState) {
    case NoRemotePlayerMediaSource:
      return "No Source";
    case HomeRemotePlayerMediaSource:
      return "Home";
    case YouTubeRemotePlayerMediaSource:
      return "YouTube";
    case SpotifyRemotePlayerMediaSource:
      return "Spotify";
    case NetflixRemotePlayerMediaSource:
      return "Netflix";
    case PlexRemotePlayerMediaSource:
      return "Plex";
    case TVRemotePlayerMediaSource:
      return "TV";
  }
  return "";
}

}  // namespace homeassistant_media_player
}  // namespace esphome
