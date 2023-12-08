# Yaml options for homeThing

## Display State
[Display State is a separate component so that it can be reused. It needs to be included separately, full readme](https://github.com/landonr/homeThing/tree/main/components/homeThingDisplayState)
### Required
- **font_small**: required - id of the small font
- **font_medium**: required - id of the medium font
- **font_large**: required - id of the large font
- **font_large_heavy**: required - id of the large heavy font
- **font_material_large**: required - id of the large material font icons
- **font_material_small**: required - id of the small material font icons

### Optional
- **launch_image**: optional - image to display on boot
- **header_height**: optional - height of header, default 16
- **margin_size**: optional - margin size, default 4
- **bottom_bar_margin**: optional - bottom bar margin, default 1
- **slider_margin_size**: optional - slider margin size (used for rgb lights), default 8
- **icon_size**: optional - icon size, default 18
- **scroll_bar_width**: optional - scroll bar width, default 6
- **now_playing_max_lines**: optional - max lines for now playing, default 5
- **font_size_width_ratio**: optional - font size height/width ratio, default 0.6
- **draw_shuffle**: optional - draw shuffle icon, default on (on, off, always)
- **draw_repeat**: optional - draw repeat icon, default on (on, off, always)
- **draw_header_time**: optional - draw time in header, default true
- **draw_battery_level**: optional - draw battery level in header, default false
- **dark_mode**: optional - dark mode (black background), default true
- **draw_volume_level**: optional - draw volume level in header, default false
- **draw_now_playing_bottom_menu**: optional - draw bottom menu on now playing, default false
- **boot_device_name**: optional - boot device name, default homeThing
- **colors**: optional - color palette
  - **gray_dark**: optional - dark gray (default 102, 102, 102)
  - **gray_dark_2**: optional - dark gray 2 (default 20, 20, 20)
  - **gray**: optional - gray (default 25, 25, 25)
  - **accent_primary**: optional - accent primary (default 0, 114, 165)
  - **blue**: optional - blue (default 0, 0, 255)
  - **green**: optional - green (default 0, 255, 0)
  - **black**: optional - black (default 0, 0, 0)
  - **white**: optional - white (default 255, 255, 255)
  - **pink**: optional - pink (default 255, 40, 255)
  - **red**: optional - red (default 255, 0, 0)
  - **yellow**: optional - yellow (default 255, 191, 25)

## Menu
### Required
- **display**: required - id of the display, used to draw display
- **display_state**: required - id of the display setup

### Optional
- **settings**: optional - more menu settings
  - **mode**: required - rotary - 3 button - 2 button
  - **display_timeout**: optional - seconds until screen fades out. default 16
  - **sleep_after**: optional - seconds until device sleeps. default 3600
  - **menu_rollover_on**: optional - menu rolls back to top if you scroll past to bottom
  - **lock_after**: optional - seconds until device locks. default 0 (no lock)
- **sleep_switch**: optional - allows the device to sleep itself
- **battery**: optional - shows battery life
- **backlight**: optional - allows the device to turn off backlight
- **header**: optional - shows a header on top of menu
  - **time_id**: optional - id of time entity
- **menu_display**: optional - customize display drawing
- **apps**: optional - downloadable apps
- **boot**: optional - customize boot settings
  - **api_connected**: optional - shows whether device is connected to home assistant
  - **media_player_group**: optional - waits for media players to load before entering menu  
- **on_redraw**: optional - automation to redraw display
- **home_screen**: optional - customize entities on home screen
- **screens**: optional - custom screens for entities


## Example of a customized yaml
```yaml

homeThingDisplayState:
    id: homeThingDisplayState # required
    font_small: font_small # required
    font_medium: font_medium # required
    font_large: font_large # required
    font_large_heavy: font_large_heavy # required
    font_material_large: font_material_large # required
    font_material_small: font_material_small # required
    launch_image: launch_image 
    header_height: 16
    margin_size: 4
    bottom_bar_margin: 1
    slider_margin_size: 8
    icon_size: 18
    scroll_bar_width: 6
    now_playing_max_lines: 5
    font_size_width_ratio: 0.6
    draw_shuffle: on # optional (on, off, always)
    draw_repeat: on # optional (on, off, always)
    draw_header_time: True
    draw_battery_level: False
    dark_mode: True
    draw_volume_level: False
    draw_now_playing_bottom_menu: False
    boot_device_name: homeThing
    colors:
      gray_dark: gray_dark
      gray_dark_2: gray_dark_2
      gray: gray
      accent_primary: accent_primary
      blue: blue
      green: green
      black: black
      white: white
      pink: pink
      red: red
      yellow: yellow

homeThing:
  id: homeThingMenu
  display: my_display
  display_state: display_state_id
  settings:
    mode: rotary
    display_timeout: 16
    sleep_after: 3600
    menu_rollover_on: False
    lock_after: 0
  sleep_switch: sleep_toggle
  battery:
    battery_percent: battery_percent
    charging: charging
  backlight: backlight
  header:
    time_id: time_id
  apps:
    - now_playing
  boot:
    api_connected: api_connected
    media_player_group: media_player_group
  on_redraw:
    then:
      component.update: my_display
  home_screen:
    - type: light
      id: light_all_lights
  screens:
    - name: Desk Screen
      entities:
        - type: cover
          id: cover_megadesk
        - type: number
          id: desk_height_number
        - type: command
          name: "desk nudge up"
          command:
            - homeassistant.service:
                service: button.press
                data:
                  entity_id: "button.desk_position_nudge_up"
        - type: command
          name: "desk nudge down"
          command:
            - homeassistant.service:
                service: button.press
                data:
                  entity_id: "button.desk_position_nudge_down"
        - id: light_desk_lamp
          type: light
    - name: Lights Screen
      entities:
        - id: light_all_lights
          type: light
        - type: command
          name: "color lights"
          command:
            - homeassistant.service:
                service: script.colors
        - type: command
          name: "lights off"
          command:
            - homeassistant.service:
                service: script.off_script
    - name: Settings Screen
      show_version: True
      entities:
        - id: backlight
          type: light
        - id: "restart_switch"
          type: switch
        - id: wifi_ssid
          type: text_sensor
        - id: wifi_signal_percent
          type: sensor
        - id: wifi_ip
          type: text_sensor
```
