<h1 align = "center">ESPHome Remote: Getting Started 🌈</h1>

# Introduction
#### ESPHome Remote supports the following features:

- controls Sonos speakers
	-   play, pause, next, prev, volume, shuffle, mute
	-   manage group
	-   change source
- controls Roku tv
	- up, down, left, right, select, volume
	- change source
- Smart home stuff
	- runs scenes and scripts
	- toggle lights on and off 5
	- view Home Assistant Sensors
- more
	- display sleeps after 16s, powers off after 2h (configure through substitions)

# What You Need
-  Hardware (pick one):
	 - TTGo T-Display
	- TTGo T-Display T4
	- M5Stack Fire
- Software:
	- [ESPHome](https://esphome.io/)
	- [Home Assistant](https://www.home-assistant.io/)

# Install guide
1. Copy project into your ESPHome folder
2. Find the yaml for your device
	1. TTGo T-Display: lilygo.yaml
	2. TTGo T-Display T4: tdisplayt4.yaml
	3. M5Stack Fire: fireremote.yaml
3. The basic configuration is done through substitions
	1. font_size: Make sure the font sizes match the font sizes set above
	2. font_size_width_ratio: The width:height ratio of the font used
	3. header_height: Customize the header height
	4. margin_size: Customize the margin
	5. bottom_bar_margin: Distance the media duration bar is from the bottom
	6. scroll_bar_width: Customize the width of the scrollbar
	7. icon_size: Size of icons used to calculate UI
	8. now_playing_max_lines: Max lines displayed on Now Playing Screen
	9. draw_now_playing_menu: Show navigation menu on Now Playing screen
	10. boot_device_name: Customize the name displayed on boot
    11. display_timeout: Turn off display [s]
    12. sleep_after: Enter deep sleep [s]
	- More configuration can be done in esphome/esphome-remote-base.yaml
		1. Customize **Colors**
		2. Customize **Fonts**
			- If you edit the font size you have to update both the font size and the corresponding font_size in Globals
		3. Customize **Globals**

4. Update the **Custom Components** in the yaml to match your config
	1. Set your speaker media player names under speaker_group_component
		1. Update the TV to match your media player
		2. Set your soundbar media player to the TV config
	2. Update your Scenes and Scripts under scene_group_component
	3. Update your Light switches under light_group_component
	4. Update your Home Assistant sensors under sensor_group_component
5. Install the firmware to your device
6. Add the device to Home Assistant
7. Done! 🎉
