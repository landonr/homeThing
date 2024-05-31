# HomeThing Menu Configuration Guide

This README provides an overview of the configuration options for the `HomeThingMenuBase` custom component in ESPHome. This setup example configures a device using the LilyGo T-Display S3 with various apps, settings, and menu screens for an interactive display interface.

**For the full configuration file, please refer to the [example configuration](https://github.com/landonr/homeThing/blob/main/tdisplay-s3.yaml).**

## Configuration Options

### **homeThing**

#### General Options
- **id**: `string`
  - Unique ID for the HomeThing component.
- **apps**: `list`
  - List of applications available on the menu.
  - [See the full list of apps](../) 
- **settings**: `map`
  - **Optional**, Configuration for device behavior and display.
  - **sleep_after**: `int`
    - Time (in seconds) before the device goes to sleep.
  - **display_timeout_while_charging**: `int`
    - Time (in seconds) for display timeout while charging.
  - **idle_app**: `string`
    - Default application when the device is idle.
  - **max_brightness**: `float`
    - Maximum brightness level.
- **backlight**: `Light`
  - ID of the backlight component.
- **header**: `map`
  - Configuration for the display header.
- **battery**: `map`
  - Configuration for battery monitoring.
  - **charging**: `BinarySensor`
    - ID of the binary sensor that indicates whether the device is currently charging.
  - **battery_percent**: `Sensor`
    - ID of the sensor that monitors the battery percentage.
- **display**: `string`
  - ID of the display component.
- **on_redraw**: `action`
  - Actions to perform when the display is redrawn.
  - example 
    ```yaml
    homeThing:
      ...
      on_redraw:
      then:
        component.update: my_display
- **display_state**: `string`
  - ID of the display state component.
  - [Link to Display State Configuration Guide](https://github.com/landonr/homeThing/blob/main/components/homeThingDisplayState/README.md)
- **boot**: `map`
  - **Optional**, Configuration for actions during device boot.
  - **api_connected**: `BinarySensor`
      - ID of the API connected sensor that the boot screen will wait for.
  - **media_players_loaded**: `BinarySensor`
      - ID of the media players loaded sensor that the boot screen will wait for.
  - **launch_image**: `Image`
      - ID of the launch image.
- **home_screen**: `map`
  - Configuration for the main home screen.
  - example:
    ```yaml
    homeThing:
      ...
      home_screen:
        name: homeThing
        entities:
          - id: cat_max_servings
            type: number
- **screens**: `list`
  - Configuration for additional screens.
  - example:
    ```yaml
    homeThing:
      ...
      screens:
        - name: Desk Screen
          entities:
            - id: light_desk_lamp
              type: light
              