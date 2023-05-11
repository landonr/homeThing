<h1 align = "center">homeThing: Getting Started 🌈</h1>

<h2 align = "center">
<a href="https://github.com/landonr/homeThing/blob/main/example-homeThing.yaml">see a Full Example Config here</a></h2>

### Required
-  Hardware (pick one):
	- LilyGo T-Display
	- LilyGo T-Display T4
	- LilyGo T-Display S3
	- M5Stack Fire
	- M5Stick C
	- Generic ESP32 + [Supported display](https://esphome.io/components/display/index.html#see-also "Supported display")
- Software:
	- [ESPHome](https://esphome.io/) (required)
	- [Home Assistant](https://www.home-assistant.io/) (required for now, MQTT support is coming soon)
	- [Spotcast](https://github.com/fondberg/spotcast) - [(optional)](#spotcast-setup) 

# Install guide
1. [Install](#1-install-esphome-on-your-hardware "Install")
2. [Include](#2-include-the-homething-components-in-your-yaml "Include")
3. [Setup Device](#3-setup-device "Setup Device")
4. [Setup Home](#4-setup-your-home-config "Setup Home")
5. [Setup Menu](#5-set-up-the-menu-groups "Setup Menu")
6. [Setup homeThing](#6-set-up-the-homething-menu "Setup homeThing")
7. [Download Fonts](#7-download-fonts "Download Fonts")
7. [Upload](#7-install-on-your-device "Upload")
8. [Connect](#8-add-the-device-to-home-assistant "Connect")
9. **Done!**

### 1. Install ESPHome on your hardware
[ESPHome install guide](https://esphome.io/guides/getting_started_hassio.html)

### 2. Include the homeThing components in your yaml
```yaml
external_components:
  - source:
      type: git
      url: https://github.com/landonr/homeThing
      ref: main
    components: [homeThing] # homething menu
  - source:
      type: git
      url: https://github.com/landonr/esphome-components
      ref: main
    components: [
      homeassistant_component, # base component to control home assistant entities. required for all
      homeassistant_switch_group, # only include if you use switches in menu
      homeassistant_sensor_group, # only include if you use text sensors in menu
      homeassistant_light_group, # only include if you use lights in menu
      homeassistant_media_player, # only include if you use media players in menu
      homeassistant_service_group, # only include if you want to call services/scripts
      media_player_source, # required for all media player sources
      media_player_source_sonos, # loads sonos favorites into a list
      media_player_source_spotify, # loads spotify playlists from Spotcast sensor into a list
      media_player_source_custom # define custom source lists
    ]
```

### 3. Setup Device
these packages are for a Lilygo TDisplay with a rotary encoder and battery
```yaml
packages:
  remote_package:
    url: https://github.com/landonr/homeThing
    ref: main
    files: [
      common/device_base.yaml, # defines api, ota, free memory and uptime sensor
      common/ipod/lilygo_tdisplay_ipod_backlight.yaml, # used for toggling backlight
      common/ipod/lilygo_tdisplay_ipod_battery.yaml, # used for battery percent
      common/ipod/lilygo_tdisplay_ipod_binary_sensor.yaml, # used for button controls
      common/ipod/lilygo_tdisplay_ipod_rotary.yaml, # used for rotary controls
      common/ipod/lilygo_tdisplay_ipod_sleep.yaml, # required for device to sleep
      common/fonts.yaml, # default font
      common/icon_fonts.yaml # material icons
    ]
    refresh: 0s
```

### 4. Setup your home config
- detailed information is here https://github.com/landonr/esphome-components
- example

```yaml
switch:
    - platform: homeassistant_switch_group
      entity_id: "switch.oven_fan"
      name: "Oven Fan"
      id: oven_fan_switch

text_sensor:
  - platform: homeassistant
    entity_id: "sensor.vancouver_forecast"
    name: "Weather"
    id: sensor_weather

light:
  - platform: homeassistant_light_group
    id: light_office_lamp
    entity_id: light.office_lamp
    name: Office Lamp

# sonos favorite source
media_player_source_sonos:
  id: sonos

media_player:
  - platform: homeassistant_media_player
    name: Beam
    entity_id: "media_player.beam"
    id: media_player_beam
    type: speaker
    sources:
      - id: sonos
        type: sonos
  - platform: homeassistant_media_player
    name: TV
    entity_id: "media_player.55_tcl_roku_tv"
    id: media_player_tv
    type: roku
    soundbar:
      speaker: media_player_beam
```
### 5. Set up the menu groups
```yaml
# switch menu - replace with your IDs
homeassistant_switch_group:
  # defined above with switches
  id: switch_group_component
  switches:
    - id: oven_fan_switch

# sensor menu - replace with your IDs
homeassistant_sensor_group:
  id: sensor_group_component
  sensors:
    - id: sensor_weather

# light menu - replace with your IDs
homeassistant_light_group:
  id: light_group_component
  lights:
    - id: light_office_lamp

# media player menu - replace with your IDs
homeassistant_media_player:
  id: media_group_component
  media_players:
    - id: media_player_beam
      type: speaker
    - id: media_player_tv
      type: roku

# service menu - replace with your IDs
homeassistant_service_group:
  id: service_group_component
  commands:
    - text: "desk nudge up"
      command:
        - homeassistant.service:
            service: button.press
            data:
              entity_id: "button.desk_position_nudge_up"
```

### 6. Set up the homeThing menu
```yaml
# homeThing config
# you only need one menu _group
homeThing:
  id: homeThingMenu
  sleep_switch: sleep_toggle #optional
  backlight: backlight #optional
  battery: #optional
    battery_percent: battery_percent
    charging: charging
   # need atleast 1 group_component
  media_player_group: media_group_component
  light_group: light_group_component
  service_group: service_group_component
  sensor_group: sensor_group_component
  switch_group: switch_group_component
  display: my_display # required
  on_redraw: # required
    then:
      component.update: my_display
  display_state:
    draw_battery_level: true # only needed if you have a battery
    # probably leave these alone
    font_small: small_font
    font_medium: medium_font
    font_large: large_font
    font_large_heavy: large_heavy_font
    font_material_large: material_font_large
    font_material_small: material_font_small
    font_logo: home_thing_logo
```

### 7. Download Fonts
This step needs improvement because the files need to be checked out manually. You can either
1. run this on home assistant
```
curl -LJO https://github.com/landonr/homeThing/archive/refs/heads/main.zip && \
unzip -j main.zip "homeThing-main/fonts/*" -d esphome/fonts/ && \
rm main.zip
```

2. or download the fonts from here https://github.com/landonr/homeThing/tree/main/fonts
and copy to **esphome/fonts**. You can do this by uploading to the folder using the **File Editor** add-on
### 8. Install on your device
### 9. Add the device to Home Assistant
### 10. **Done! 🎉**

---

# If you have any trouble with set up, please make an [issue](https://github.com/landonr/homeThing/issues)