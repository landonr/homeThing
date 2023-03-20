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

enum RemotePlayerSourceType {
  MusicRemotePlayerSourceType,
  FavoriteItemIDRemotePlayerSourceType,
  SourceRemotePlayerSourceType,
  PlaylistRemotePlayerSourceType
};

class MediaPlayerSource {
 public:
  MediaPlayerSource(RemotePlayerSourceType new_media_content_type,
                    std::string new_title, std::string new_media_content_id)
      : media_content_type_(new_media_content_type),
        title_(new_title),
        media_content_id_(new_media_content_id) {}

  RemotePlayerSourceType media_content_type_;
  std::string title_;
  std::string media_content_id_;

  std::string sourceTypeString() {
    switch (media_content_type_) {
      case MusicRemotePlayerSourceType:
        return "music";
      case FavoriteItemIDRemotePlayerSourceType:
        return "favorite_item_id";
      case SourceRemotePlayerSourceType:
        return "source";
      case PlaylistRemotePlayerSourceType:
        return "playlist";
    }
    return "";
  }
};

}  // namespace homeassistant_media_player
}  // namespace esphome
