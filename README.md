
<h1 align = "center">homeThing Remote ⚙️</h1>
<h5 align = "center">(esphome remote)</h1>
<p align="center">
<a href="https://github.com/landonr/esphome-remote/stargazers"><img src="https://img.shields.io/github/stars/landonr/esphome-remote?style=for-the-badge"/></a>
<a href="https://discord.gg/BX6ZtGKHTy"><img src="https://img.shields.io/discord/1021434469917413498?style=for-the-badge"/></a>
</p>

A collection of Open Source Universal Remotes that run a custom menu system to integrate with your smart home

- iPod style menu system for controlling media players, lights, hvac, window covers, etc
- Open source, customizable, and 3D printable hardware specific for homeThing remotes

### Learn More
 - [Get Started](GettingStarted.md) - Learn how to build your own remote
 - [Discord](https://discord.gg/BX6ZtGKHTy) - support / chat with the community and the team
 
## Demo Video 🎬
[![Demo Video](https://img.youtube.com/vi/Tg7Op2hr42o/0.jpg)](https://youtu.be/Tg7Op2hr42o)

# Hardware 💪

We made a couple of custom devices for running homeThing, but it can also run on almost any ESP32 such as,
- M5Stack Fire
- M5Stick C
- Lilygo T-Display T4
- Lilygo T-Display TTGO
- Lilygo T-Display S3

## homeThing Devices
<table>
  <tr>
    <th>TDisplay homeThing</th>
    <th>TDisplay S3 homeThing</th>
  </tr>
  <tr>
    <td><img src="https://github.com/landonr/homeThing/assets/2607659/4054f110-6d8d-4764-8aab-d7287209cb47" width="400"/></td>
    <td><img src="https://github.com/landonr/homeThing/assets/2607659/b19a5b39-b0dd-4bac-9803-df9a452ab56b" width="400"/></td>

  </tr>
  <tr>
    <td><a href="https://www.pcbway.com/project/shareproject/homeThing_V1_3_e3572f8f.html">Order Case from PCBWay</a></td>
    <td>
      <a href="https://www.pcbway.com/project/shareproject/homeThing_V1_3_e3572f8f.html">Order Case from PCBWay</a><br>
      <a href="https://www.pcbway.com/project/shareproject/homeThing_V1_3_e3572f8f.html">Order PCB from PCBWay</a>
    </td>
  </tr>
  <tr>
    <td><a href="https://github.com/landonr/homeThing/tree/main/Case%20Design">Case 3D Files</a></td>
    <td><a href="https://github.com/landonr/homeThing/tree/main/Case%20Design">Case 3D Files</a></td>
  </tr>
  <tr>
    <td><a href="tdisplay-ipod.yaml">ESPHome Config</a></td>
    <td><a href="tdisplay-s3.yaml">ESPHome Config</a></td>
  </tr>
  <tr>
  	<td>
	<h4>Features</h4>
	<ul>
	<li>Low cost remote with minimal parts</li>
	<li>Scroll wheel and 2 configurable buttons</li>
	<li>USB-C Charging</li>
	<li>3D Printable case with 3 parts</li>
	</ul>
	</td>
  	<td>
	<h4>Features</h4>
	<ul>
	<li>Faster processor and larger display</li>
	<li>Custom PCB for easier setup</li>
	<li>IR Blaster for controlling infrared devices</li>
	<li>Interal microphone for Voice Assistant</li>
	<li>Scroll wheel and 5 configurable buttons</li>
	<li>USB-C Charging</li>
	<li>3D Printable case with 2 parts</li>
	</ul>
	</td>
  </tr>
  <tr>
  	<td>
	<h4>Parts</h4>
	<ul>
	<li><a href="https://www.adafruit.com/product/5001">Adafruit ANO Directional Navigation and Scroll Wheel Rotary Encoder</a></li>
	<li><a href="https://www.adafruit.com/product/5221">Adafruit ANO Rotary Navigation Encoder Breakout PCB</a></li>
	<li><a href="https://leeselectronic.com/en/product/88345-battery-rechargeable-li-poly-37v-1100mah-jst-zh.html">1100mah lipo</a></li>
	</ul>
	</td>
  	<td>
	<h4>Parts</h4>
	<ul>
	<li><a href="https://www.adafruit.com/product/5001">Adafruit ANO Directional Navigation and Scroll Wheel Rotary Encoder</a></li>
	<li><a href="https://www.adafruit.com/product/5221">Adafruit ANO Rotary Navigation Encoder Breakout PCB</a></li>
	<li><a href="https://www.adafruit.com/product/5221">Custom PCB</a></li>
	<li><a href="https://www.aliexpress.us/item/32962426410.html">INMP441 Microphone</a></li>
	<li><a href="https://leeselectronic.com/en/product/71446-ic-i-o-expander-for-i2c-8bit-pcf8574p.html">PCF8574 GPIO Extender</a></li>
	</ul>
	</td>
  </tr>
</table>

# Menu Features 📝
## iPod classic style menu 
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

## Now playing screen ⏯
<img src="docs/nowPlayingOverlay.png"/>
<ul>
<li>Control media players using the scroll wheel buttons</li>
<li>Scroll for volume using the scroll wheel</li>
</ul>

|Music|TV|
|--|--|
|<ul><li>Control smart speakers like Sonos, Spotify, or Google Home</li><li>Change source from Spotify playlists, Sonos Favorites, and custom lists</li><li>Manage speaker groups</li></ul>|<ul><li>Menu navigation such as up, down, left, right, select (with scroll wheel)</li><li>Power, back, home, pause in second menu</li><li>Change source from tv source list</li><li>Optional soundbar support</li></ul>|

## IR Remote
<ul>
<li>Custom UI and buttons to control infrared cat toys</li>
</ul>

## Games
<ul>
<li>Snake</li>
<li>Breakout</li>
</ul>


# Mentions
- Hackaday https://hackaday.com/2022/09/08/scroll-through-esphome-with-ipod-style-click-wheel/
- Hackster https://www.hackster.io/news/ipod-style-esphome-remote-3ca38edab2b4
- Reddit post https://www.reddit.com/r/homeassistant/comments/x59dcp/i_built_an_ipod_style_esphome_remote_for_home/
- Self Hosted - Episode 80 - Solving Whole Home Audio (around 30:00) https://selfhosted.show/80 

# includes
- <a href="https://esphome.io/">ESPHome</a>
- <a href="https://github.com/be5invis/Iosevka">Iosevka Font</a>
- [@ssieb](https://github.com/ssieb)'s <a href="https://github.com/ssieb/custom_components/tree/master/components/ip5306">IP5306 esphome component</a>
- <a href="https://github.com/landonr/esphome-components">homeThing ESPHome Components</a>

# Sponsorship

<img src="https://camo.githubusercontent.com/3fab6e1d2bd2504dd94cfc2927b151aee553c106eef66ef09e49bdcd4c1013dc/68747470733a2f2f63646e2e6861636b616461792e696f2f696d616765732f3838313032313637333238353832393239312e706e67" data-caption="PCBWay Logo" data-no-zoom="">

## [PCBWay](https://pcbway.com/g/Xymq6O "PCBWay") sponsors 3D Printing and PCB assembly costs on this project during the prototyping phase! 
If you are interested in their awesome fabrication services please check them out.&nbsp; They offer 

3D prototyping,&nbsp;PCB design and assembly, as well as CNC metal fabrication.&nbsp; Costs are reasonable and the quality is as good as it gets.&nbsp; Thank you PCBWay for sponsoring us and other fun projects!<br><br>

## [cargo](https://cargo.site/ "cargo.site") sponsors our website!

They make it super easy to keep things pretty and up to date! It’s honestly so much easier than managing our own site, highly recommended!
