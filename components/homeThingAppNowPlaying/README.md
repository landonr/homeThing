# HomeThingMenuNowPlayingControl Component

The `HomeThingMenuNowPlayingControl` component for ESPHome integrates with HomeThing and allows you to display a "Now Playing" screen for a group of media players. It provides options to show the currently playing media, control playback, and customize the display.

Can be used in the [homeThing](https://github.com/landonr/homeThing) menu system.

## Base Configuration

The `HomeThingMenuNowPlayingControl` component has several base configuration options. By default, appropriate values will be chosen for all of these options, but you can override them if you want to.

### Example Configuration

```yaml
# Example HomeThingMenuNowPlayingControl configuration
homeThingAppNowPlaying:
  id: now_playing
  display: my_display
  display_state: display_state_id
  media_player_group: media_group_component
  now_playing_image: media_player_image
```

### Notes

- The `now_playing_image` component should be configured if you want to display an image for the currently playing media.
- The bottom menu is useful for remotes with a limited number of buttons.

## Configuration Details

#### Configuration Variables

- **id** *(Required, string)*: Manually specify the ID for code generation.
- **display** *(Required)*: The display buffer to use.
- **display_state** *(Required)*: The display state to use. [Display state info](../homeThingDisplayState)
- **media_player_group** *(Required)*: The ID of the `HomeAssistantMediaPlayerGroup` component to use. [Media player group info](https://github.com/landonr/esphome-components/tree/main/components/homeassistant_media_player)
- **now_playing_image** *(Optional)*: The ID of the image to display for the currently playing media. [Now Playing image info](../../common/nowPlayingImage.yaml)
- **draw_bottom_menu** *(Optional, boolean)*: Whether to draw the bottom menu on the display. Defaults to `false`.
