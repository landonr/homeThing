# esphome-remote 
iPod style wifi smart home remote. Supports M5Stack Fire or Lilygo T-Display with Adafruit ANO Scrollwheel. Uses ESPHome and Home Assistant to integrate with Sonos, Roku and run custom scenes and scripts.

## Demo Video 🎬
[![Demo Video](https://img.youtube.com/vi/Tg7Op2hr42o/0.jpg)](https://youtu.be/Tg7Op2hr42o)

## Features 📝
### iPod classic style menu 
|t-display demo|m5stack fire demo|
|--|--|
|![Scroll Demo Gif](docs/scrollListSelectTDisplay.gif)| ![Scroll List Fire Demo Gif](docs/scrollListSelectFire.gif)|

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

|Manage speaker group|Change source from Sonos favourites list|
|--|--|
|![Speaker Group Demo Gif](docs/groupTDisplay.gif)|![Scroll Volume Demo Gif](docs/sourceSelectTDisplay.gif)|

### 2. Control Roku tv 📺
- Remote directional buttons up, down, left, right, select (with scroll wheel)
- Scroll for volume (with scroll wheel)
- Power, back, home, pause in second menu (with scroll wheel)
- Change source from tv source list

- Change current media player (tv or speaker) from home menu
	- On boot the current media player is the first speaker found playing music or the tv

## Smart Home Control 🧠🏠
|Toggle lights on and off|View sensors from Home Assistant|
|--|--|
|![Lights Menu](docs/lightsTDisplay.png)|![Sensors Menu](docs/sensorsTDisplay.png)|

- #### Also run scenes and scripts

## Hardware 
### 1. Lilygo T-Display with Scrollwheel ⚙️
#### T-Display with scrollwheel has the same controls as an ipod
- Scroll to navigate ui and for volume on now playing
- Up for menu, left for previous track, right for next track, down for pause, center for select
- 2 additional buttons for home and options menu
- Charges with usb-c
- 1100 mah battery
- On average 3 days of battery life depending on use
	- Adafruit ANO Directional Navigation and Scroll Wheel Rotary Encoder https://www.adafruit.com/product/5001
	- Adafruit ANO Rotary Navigation Encoder Breakout PCB https://www.adafruit.com/product/5221
	- 1100mah lipo https://leeselectronic.com/en/product/88345-battery-rechargeable-li-poly-37v-1100mah-jst-zh.html

### 2. M5Stack Fire 🔥
#### M5Stack Fire is supported without any additional hardware
- Uses the 3 buttons for navigation
- Simpler UI
- Uses internal light for booting sequence and while on
	- https://www.mouser.ca/ProductDetail/M5Stack/K007-V26?qs=TCDPyi3sCW1MGXwu64XB7w%3D%3D

![Pause Play Fire Demo Gif](docs/pausePlayFire.gif)

## includes
- esphome https://esphome.io/
- iosevka font https://github.com/be5invis/Iosevka

## todo
- queue count
- sonos night sound / speach boost
- repeat/all/single/off
- marquee long text
- case
- circuit diagram for tdisplay
- support tdisplay t4