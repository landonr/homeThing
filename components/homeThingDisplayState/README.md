# homeThing Display State

**Used for menu and apps**

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
- **boot_logo_size**: optional - boot logo size, default 48
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

## Include
```yaml
external_components:
  - source:
      type: git
      url: https://github.com/landonr/homeThing
      ref: main
    refresh: 1d
    components: [homeThingDisplayState]
```

## Example of a Display State yaml with all optional settings set
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
    boot_logo_size: 48
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
```
