# Yaml options for homeThing

example of a customized yaml
```yaml

homeThing:
  id: homeThingMenu
  settings:
    mode: 3_button
  sleep_switch: sleep_toggle
  backlight: backlight
  battery:
    battery_percent: battery_percent
    charging: connected
  media_player_group: media_group_component
  light_group: light_group_component
  service_group: service_group_component
  sensor_group: sensor_group_component
  switch_group: switch_group_component
  display: my_display
  on_redraw:
    then:
      component.update: my_display
  display_state:
    font_small: small_font
    font_medium: medium_font
    font_large: large_font
    font_large_heavy: large_heavy_font
    font_material_large: material_font_large
    font_material_small: material_font_small
    font_logo: home_thing_logo
    header_height: 24
    margin_size: 8
    slider_margin_size: 6
    scroll_bar_width: 8
    bottom_bar_margin: 8
    now_playing_max_lines: 5
    draw_now_playing_bottom_menu: true
    draw_battery_level: true
    dark_mode: true
    draw_volume_level: false
    draw_header_time: true
```

## Display State
### Required
- font_small # id of the small font
- font_medium    # id of the medium font
- font_large # id of the large font
- font_large_heavy   # id of the large bold font
- font_material_large    # id of the large material icon font
- font_material_small    # id of the small material icon font
- font_logo  # id of the boot logo font

### Optional
- header_height  # Customize the header height
- margin_size    # Customize the margin size
- bottom_bar_margin  # Distance the media duration bar is from the bottom
- slider_margin_size # Margin used for sliders (rgb lights)
- icon_size  # Size of icons used to calculate UI
- scroll_bar_width   # Customize the width of the scrollbar
- boot_logo_size # Size of the homeThing logo in the boot sequence
- now_playing_max_lines  # Max lines displayed on Now Playing Screen
- font_size_width_ratio  # The width:height ratio of the font used
- draw_shuffle   # Draw shuffle state in header (on/off/always)
- draw_repeat    # Draw repeat state in header (on/off/always)
- draw_header_time   # Draw time in header
- draw_battery_level # Draw battery level (battery required)
- dark_mode  # Make background white instead of black
- draw_volume_level  # Draw volume level in header
- draw_now_playing_bottom_menu   # Show navigation menu on Now Playing screen
- boot_device_name   # Customize the name displayed on boot
- colors # Defines Colors (below)

## Colors
### Required
- gray_dark  # 
- gray_dark_2    # 
- gray   # 
- accent_primary # 
- blue   # 
- green  # 
- black  # 
- white  # 
- pink   # 
- red    # 
- yellow # 

## Menu Settings
### Optional
- mode   # rotary, 3 button, 2 button
- display_timeout # seconds until screen fades out. default 16
- sleep_after # seconds until device sleeps. default 3600
- menu_rollover_on # menu rolls back to top if you scroll past to bottom
