# esphome-remote
ipod style wifi home assistant remote with scroll wheel, directional buttons and display

## features
- ipod classic style menu
	- current media player, volume level, play/pause, shuffle, and battery lever are shown in header
		- change current media player (tv or speaker) from home menu
		- on boot the current media player is the first speaker found playing music or the tv
	- ipod style menu that uses scroll wheel for navigation
		- scroll bar appears if list extends past the screen. 
	- screen returns to now playing after idle
		- display sleeps after 10s of idle
		- deep sleep after 2h of idle
	- on average 3 days of battery life depending on use
		- charges by usb c
- control sonos speakers
	- play / pause, next track, previous track, toggle shuffle
	- scroll for volume
	- manage speaker group
	- change source from sonos favourites list
- control roku tv
	- up, down, left, right, select
	- scroll for volume
	- power, back, home, pause in second menu
	- change source from list
- toggle lights on and off
- run scenes and scripts

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
- mute / unmute, repeat/all/single/off
- marquee long text
- media duration bar
- clean up soundbar code
- case
- photos
- demo video