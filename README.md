# esphome-remote
iPod style wifi smart home remote. Supports M5Stack Fire and Lilygo T-Display with Adafruit ANO Scrollwheel. Uses ESPHome and Home Assistant to integrate with Sonos, Roku and custom scenes and scripts.

[![Demo Video](https://img.youtube.com/vi/Tg7Op2hr42o/0.jpg)](https://youtu.be/Tg7Op2hr42o)

## features
- ipod classic style menu for media control
	- current media player, volume level, play/pause, shuffle, and battery lever are shown in header
		- change current media player (tv or speaker) from home menu
		- on boot the current media player is the first speaker found playing music or the tv
	- scroll bar appears if list extends past the screen. 
	- screen returns to now playing after idle
		- display sleeps after 10s of idle
		- deep sleep after 2h of idle
	- on average 3 days of battery life depending on use
	- font and UI size is adjustable in yaml for different screen sizes
- control sonos speakers
	- play / pause, next track, previous track, toggle shuffle and mute
	- scroll for volume (with scroll wheel)
	- manage speaker group
	- change source from sonos favourites list
- control roku tv
	- remote functions like up, down, left, right, select (with scroll wheel)
	- scroll for volume (with scroll wheel)
	- power, back, home, pause in second menu (with scroll wheel)
	- change source from tv source list
- toggle lights on and off
- run scenes and scripts
- simple yaml configuration. all of the complicated code is included in other files

# Lilygo T-Display with Scrollwheel
- T-Display with scrollwheel has the same interface as an ipod
	- scroll to navigate ui and for volume on now playing
	- up for menu, left for previous track, right for next track, down for pause, center for select
	- 2 additional buttons for home and options menu
	- charges with usb-c
	- 900 mah battery

# M5Stack Fire
- M5Stack Fire is supported without any additional hardware
	- Uses the 3 buttons for navigation
	- Simpler UI
	- Uses internal light for booting sequence and while on

## includes
- esphome https://esphome.io/
- Adafruit ANO Directional Navigation and Scroll Wheel Rotary Encoder https://www.adafruit.com/product/5001
- LILYGO ESP32 T-Display
- 900mah lipo https://leeselectronic.com/en/product/88341-battery-rechargeable-li-poly-37v-900mah.html
- iosevka font https://github.com/be5invis/Iosevka

## todo
- show current playlist
- queue count
- sensors screen
- sonos night sound / speach boost
- repeat/all/single/off
- marquee long text
- media duration bar
- case
- photos
