
<h1 align = "center">homeThing Remote ⚙️</h1>
<h5 align = "center">(esphome remote)</h1>
<p align="center">
<a href="https://github.com/landonr/esphome-remote/stargazers"><img src="https://img.shields.io/github/stars/landonr/esphome-remote?style=for-the-badge"/></a>
<a href="https://discord.gg/BX6ZtGKHTy"><img src="https://img.shields.io/discord/1021434469917413498?style=for-the-badge"/></a>
</p>

iPod style wifi remote that uses ESPHome and Home Assistant to integrate with your smart home
- Controls music playback from Spotify and Sonos on local speakers
- Works as a remote for Roku TVs
- Toggles lights, runs custom scenes and scripts
- Shows the status of your smart home sensors

Supports many different hardware configurations:
- M5Stack Fire (no additional hardware)
- M5Stick C (no additional hardware)
- Lilygo T-Display T4 (no additional hardware, scrollwheel coming soon)
- Lilygo T-Display TTGO with Adafruit ANO Scrollwheel

### Learn More
 - [Get Started](GettingStarted.md) - Learn how to build your own remote
 - [Discord](https://discord.gg/BX6ZtGKHTy) - support / chat with the community and the team
 
## Demo Video 🎬
[![Demo Video](https://img.youtube.com/vi/Tg7Op2hr42o/0.jpg)](https://youtu.be/Tg7Op2hr42o)

## Features 📝
### iPod classic style menu 
|t-display demo|m5stack fire demo|lilygo T4 demo|
|--|--|--|
|![Scroll Demo Gif](docs/scrollListSelectTDisplay.gif)|![Scroll List Fire Demo Gif](docs/m5stackFireDemo.gif)|![Lilygo T4 Demo](docs/t4Demo.gif)

- Scroll through list and press center to select
- With a scrollwheel the top button goes back
- On M5Stack going up past the top of the list will go back
- A scroll bar appears if list extends past the screen. 
- Screen returns to now playing after display sleeps
    - Display sleeps after 10s of idle. Device sleeps after 2h of idle 💤
- Font size and UI size is adjustable in yaml 🔨
- Simple yaml configuration. all of the complicated code is included in other files 🛠
- Header is always shown at the top with info about the current state

### Now playing screen ⏯
<table>
<tbody>
<tr>
<td style="width: 50%;"><img src="docs/nowPlayingOverlay.png"/></td>
<td style="width: 50%;">
<ol>
<li>Current media player + # of grouped speakers</li>
<li>Volume %</li>
<li>Shuffle state</li>
<li>Play/Pause/Stopped state</li>
<li>Battery level</li>
<li>Playlist or album title</li>
<li>Media artist</li>
<li>Media title</li>
<li>Media position and playback bar</li>
<li>Media duration</li>
</ol>
</td>
</tr>
</tbody>
</table>

## Media Control 🔊📺
### 1. Control Sonos speakers
|Play / pause, next track, previous track, toggle shuffle and mute|Scroll for volume (with scroll wheel)|
|--|--|
|![Shuffle Gif](docs/shuffleTDisplay.gif)|![Scroll Volume Demo Gif](docs/volumeScrollTDisplay.gif)|

|Manage speaker group|Change source from Spotify playlists and Sonos Favorites|
|--|--|
|![Speaker Group Demo Gif](docs/groupTDisplay.gif)|![Scroll Volume Demo Gif](docs/sourceSelectTDisplay.gif)|

### 2. Control Roku tv 📺
- Remote directional buttons up, down, left, right, select (with scroll wheel)
- Scroll for volume (with scroll wheel)
- Power, back, home, pause in second menu
- Change source from tv source list
- Optional soundbar support

- Change current media player (tv or speaker) from home menu
	- On boot the current media player is the first speaker found playing music or the tv

## Smart Home Control 🧠🏠
|Toggle lights on and off|View sensors from Home Assistant|Run scenes and scripts|
|--|--|--|
|![Lights Menu](docs/lights.jpg)|![Sensors Menu](docs/sensors.jpg)|![Scenes Menu](docs/scenes.jpg)|

## Hardware 
### 1. Lilygo T-Display with Scrollwheel ⚙️
#### T-Display with scrollwheel has the same controls as an ipod

|Lilygo T-Display Controls|
|--|
|<img src="docs/tdisplayOverlay.jpg" width="525"/>|

- Scroll to navigate in list menus and for volume on Now Playing Menu
- Charges with usb-c
- 1100 mah battery
- On average 3 days of battery life depending on use
	- Adafruit ANO Directional Navigation and Scroll Wheel Rotary Encoder https://www.adafruit.com/product/5001
	- Adafruit ANO Rotary Navigation Encoder Breakout PCB https://www.adafruit.com/product/5221
	- 1100mah lipo https://leeselectronic.com/en/product/88345-battery-rechargeable-li-poly-37v-1100mah-jst-zh.html

### 2. M5Stack Fire 🔥
#### M5Stack Fire is supported without any additional hardware
- https://www.mouser.ca/ProductDetail/M5Stack/K007-V26?qs=TCDPyi3sCW1MGXwu64XB7w%3D%3D
- Uses the 3 buttons for navigation
- Simpler UI
- Uses internal RGB light for booting sequence and while on

|M5Stack Fire Controls|M5Stack Fire Boot sequence|
|--|--|
|<img src="docs/m5stackFire.jpg" width="300"/>|![Fire Boot Demo Gif](docs/m5StackFireBoot.gif)|

### 3. Lilygo T-Display T4 
#### Lilygo T-Display T4 is supported without any additional hardware
- https://www.aliexpress.com/item/32854502767.html
- Uses the 3 buttons for navigation
- Simpler UI
- Scrollwheel and battery planned

|Lilygo T4 Controls|
|--|
|<img src="docs/tdisplayT4.jpg" width="300"/>|

## Mentions
- Hackaday https://hackaday.com/2022/09/08/scroll-through-esphome-with-ipod-style-click-wheel/
- Hackster https://www.hackster.io/news/ipod-style-esphome-remote-3ca38edab2b4
- Reddit post https://www.reddit.com/r/homeassistant/comments/x59dcp/i_built_an_ipod_style_esphome_remote_for_home/
- Self Hosted - Episode 80 - Solving Whole Home Audio (around 30:00) https://selfhosted.show/80 
## includes
- esphome https://esphome.io/
- iosevka font https://github.com/be5invis/Iosevka
- [@ssieb](https://github.com/ssieb)'s IP5306 esphome component https://github.com/ssieb/custom_components/tree/master/components/ip5306

## todo
- playlist queue count
- sonos night sound / speech boost
- repeat/all/single/off
- adjust light color
- weather screen
- home / room screen
	- temp display
	- rgb / temp lights
	- media player
	- hvac
	- switches
	- sliders
- settings menu
- esphome custom components
	- display menu code
	- rotary and button control code
	- tdisplay battery / charging
	- m5stack fire battery / charging
- case
- grid ui
- circuit diagram for tdisplay
- add sensors for player state
	- current screen
	- active player
	- dark mode
- set state from service
	- change player
	- dark mode
