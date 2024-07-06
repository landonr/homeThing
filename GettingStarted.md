<h1 align = "center">homeThing: Getting Started 🌈</h1>

<h2 align = "center">
<a href="https://github.com/landonr/homeThing/blob/main/tdisplay-s3.yaml">See a Full Config for TDisplay S3 here 📦</a></h2>

<h1 align="center"><a href="https://www.youtube.com/watch?v=LEmA9E3Gzkk">
	Watch on YouTube!
	<br>
	<img src="https://github.com/landonr/homeThing/assets/2607659/e96cb4d9-3e7b-4c9f-aabb-ac2efdb432ca"/>
</a></h1>


### Required
-  Hardware (pick one):
	- homeThing S3
	- LilyGo T-Display S3
	- LilyGo T-Display
	- LilyGo T-Display T4
	- M5Stack Fire
	- M5Stick C
	- Generic ESP32 + [Supported display](https://esphome.io/components/display/index.html#see-also "Supported display")
- Software:
	- [ESPHome](https://esphome.io/) (required)
	- [Home Assistant](https://www.home-assistant.io/) (required for now, MQTT support is coming soon)
	- [Spotcast](https://github.com/fondberg/spotcast) - [(optional)](#spotcast-setup)
 	- Cairosvg, Pillow, Libmagic (optional)


### 1. Install ESPHome on your hardware
[ESPHome install guide](https://esphome.io/guides/getting_started_hassio.html)
> Note: To use fonts and images in this project, you will need to have the python pillow package installed. If you are running this project as a Home Assistant add-on or with the official ESPHome docker image, the package should already be installed. Otherwise, you can install it using the command `pip install "pillow==10.1.0"`. For more information, refer to the [ESPHome documentation on images](https://esphome.io/components/display/index.html#images).

### 2. Include the homeThing components in your yaml
```yaml
external_components:
  - source: # homeThing, display setup, and now playing app
      type: git
      url: https://github.com/landonr/homeThing
      ref: main
    refresh: 0s
    components: [
      homeThing, 
      homeThingDisplayState,
      homeThingApp, # only required if using apps
      homeThingAppNowPlaying # only required for Now Playing app
    ]
  - source:
      type: git
      url: https://github.com/landonr/esphome-components
      ref: main
    refresh: 0s
    components: [
      homeassistant_component,
      homeassistant_media_player,
      media_player_source,
      media_player_source_sonos,
      media_player_source_spotify,
      media_player_source_custom
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
      common/images.yaml, # boot screen image
    ]
    refresh: 0s
```

### 4. Setup your home config
- detailed information is here https://github.com/landonr/esphome-components
- example

```yaml
switch:
    - platform: homeassistant_component
      entity_id: "switch.oven_fan"
      name: "Oven Fan"
      id: oven_fan_switch

text_sensor:
  - platform: homeassistant
    entity_id: "sensor.vancouver_forecast"
    name: "Weather"
    id: sensor_weather
```

## 5. Music control setup *(Optional)*
 - Set up a [Home Assistant media player](https://github.com/landonr/esphome-components/tree/main/components/homeassistant_media_player/media_player)

 - Set up the [Media Player Group](https://github.com/landonr/esphome-components/tree/main/components/homeassistant_media_player)

 - Set up the [Now Playing app](https://github.com/landonr/homeThing/tree/readme-update/components/homeThingAppNowPlaying)

### 6. Set up the homeThing menu

full config documentation [here](MenuOptions.md)

```yaml
# homeThing config

homeThingDisplayState:
  id: display_state_id
  draw_battery_level: true
  font_small: small_font
  font_medium: medium_font
  font_large: large_font
  font_large_heavy: large_font
  font_material_large: material_font_large
  font_material_small: material_font_small

homeThing:
  id: homeThingMenu
  settings:
    sleep_after: 14400
  boot:
    api_connected: api_connected
    launch_image: launch_image    
  sleep_switch: sleep_toggle
  backlight: backlight
  battery:
    battery_percent: battery_percent
    charging: charging
  display: my_display
  display_state: display_state_id
  apps:
    - now_playing
  on_redraw:
    then:
      component.update: my_display
  home_screen:
    name: homeThing
    entities:
      - type: text_sensor
        id: sensor_weather
  screens:
    - name: Kitchen Screen
      entities:
        - type: switch
          id: oven_fan_switch
```
### 7. Install on your device
- [ESPHome Docker/Addon](https://esphome.io/guides/getting_started_hassio.html)
- [ESPHome Command Line](https://esphome.io/guides/installing_esphome.html)
### 8. Add the device to Home Assistant
- Home Assistant should auto discover the device and send a notification
	- If not, add a ESPHome integration using the ip address of the homeThing device
- Allow API calls from the device by clicking configure on it, then checking the **Allow the device to make Home Assistant service calls** checkbox
### 9. **Done! 🎉**

---

# If you have any trouble with set up, please make an [issue](https://github.com/landonr/homeThing/issues)
