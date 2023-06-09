import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import display, font, color, wifi, api, binary_sensor, sensor, switch, light, text_sensor, script, cover
from esphome.components.light import LightState
from esphome.const import  CONF_ID, CONF_TRIGGER_ID, CONF_MODE, CONF_RED, CONF_BLUE, CONF_GREEN, CONF_NAME, CONF_TYPE
from esphome.components.homeassistant_media_player import homeassistant_media_player_ns
from esphome.components.homeassistant_service_group import homeassistant_service_group_ns
from esphome.components.homeassistant_sensor_group import homeassistant_sensor_group_ns
from esphome.components.homeassistant_switch_group import homeassistant_switch_group_ns
homething_menu_base_ns = cg.esphome_ns.namespace("homething_menu_base")

HomeThingMenuBase = homething_menu_base_ns.class_("HomeThingMenuBase", cg.PollingComponent)
HomeThingMenuScreen = homething_menu_base_ns.class_("HomeThingMenuScreen")
MenuCommand = homething_menu_base_ns.class_("MenuCommand")
HomeThingMenuBoot = homething_menu_base_ns.class_("HomeThingMenuBoot")
HomeThingMenuSettings = homething_menu_base_ns.class_("HomeThingMenuSettings")
HomeThingMenuAnimation = homething_menu_base_ns.class_("HomeThingMenuAnimation")
HomeThingMenuDisplay = homething_menu_base_ns.class_("HomeThingMenuDisplay")
HomeThingMenuDisplayState = homething_menu_base_ns.class_("HomeThingMenuDisplayState")
HomeThingMenuHeader = homething_menu_base_ns.class_("HomeThingMenuHeader")
HomeThingMenuTextHelpers = homething_menu_base_ns.class_("HomeThingMenuTextHelpers")
HomeThingMenuRefactor = homething_menu_base_ns.class_("HomeThingMenuRefactor")
HomeThingMenuNowPlaying = homething_menu_base_ns.class_("HomeThingMenuNowPlaying")
HomeThingColorPalette = homething_menu_base_ns.class_("HomeThingColorPalette")

HomeThingMenuBaseConstPtr = HomeThingMenuBase.operator("ptr").operator("const")
HomeThingDisplayMenuOnRedrawTrigger = homething_menu_base_ns.class_("HomeThingDisplayMenuOnRedrawTrigger", automation.Trigger)

AUTO_LOAD = ["sensor"]
DEPENDENCIES = ["wifi", "api"]

CONF_DISPLAY = "display"
CONF_MENU_DISPLAY = "menu_display"
CONF_DISPLAY_STATE = "display_state"
CONF_TEXT_HELPERS = "text_helpers"
CONF_REFACTOR = "refactor_me"
CONF_NOW_PLAYING = "now_playing"
CONF_API = "api_connected"
CONF_BOOT = "boot"
CONF_HEADER = "header"
CONF_MEDIA_PLAYERS = "media_player_group"
CONF_SERVICE_GROUP = "service_group"
CONF_SENSOR_GROUP = "sensor_group"
CONF_SWITCH_GROUP = "switch_group"
CONF_ON_REDRAW = "on_redraw"
CONF_SCREENS = "screens"
CONF_SWITCHES = "switches"
CONF_TEXT_SENSORS = "text_sensors"
CONF_COMMANDS = "commands"
CONF_COMMAND = "command"
CONF_SHOW_VERSION = "show_version"
CONF_LIGHTS = "lights"
CONF_SENSORS = "sensors"
CONF_SENSOR = "sensor"
CONF_LIGHT = "light"
CONF_TEXT_SENSOR = "text_sensor"
CONF_SWITCH = "switch"
CONF_ENTITIES = "entities"
CONF_COVER = "cover"
CONF_NUMBER = "number"

# battery settings
CONF_CHARGING = "charging"
CONF_BATTERY_PERCENT = "battery_percent"
CONF_BATTERY = "battery"

# device settings
CONF_SETTINGS = "settings"
CONF_ROTARY = "rotary"
CONF_3_BUTTON = "3_button"
CONF_2_BUTTON = "2_button"
CONF_DISPLAY_TIMEOUT = "display_timeout"
CONF_MENU_ROLLOVER_ON = "menu_rollover"
CONF_SLEEP_SWITCH = "sleep_switch"
CONF_SLEEP_AFTER = "sleep_after"
CONF_BACKLIGHT = "backlight"
CONF_LOCK_AFTER = "lock_after"

# display state
CONF_OFF = "off"
CONF_ON = "on"
CONF_ALWAYS = "always"
CONF_FONT_SMALL = "font_small"
CONF_FONT_MEDIUM = "font_medium"
CONF_FONT_LARGE = "font_large"
CONF_FONT_LARGE_HEAVY = "font_large_heavy"
CONF_FONT_MATERIAL_LARGE = "font_material_large"
CONF_FONT_MATERIAL_SMALL = "font_material_small"
CONF_FONT_LOGO = "font_logo"
CONF_DRAW_NOW_PLAYING_BOTTOM_MENU = "draw_now_playing_bottom_menu"
CONF_HEADER_HEIGHT = "header_height"
CONF_MARGIN_SIZE = "margin_size"
CONF_BOTTOM_BAR_MARGIN = "bottom_bar_margin"
CONF_SLIDER_MARGIN_SIZE = "slider_margin_size"
CONF_ICON_SIZE = "icon_size"
CONF_SCROLL_BAR_WIDTH = "scroll_bar_width"
CONF_BOOT_LOGO_SIZE = "boot_logo_size"
CONF_NOW_PLAYING_MAX_LINES = "now_playing_max_lines"
CONF_FONT_SIZE_WIDTH_RATIO = "font_size_width_ratio"
CONF_DRAW_SHUFFLE = "draw_shuffle"
CONF_DRAW_REPEAT = "draw_repeat"
CONF_DRAW_HEADER_TIME = "draw_header_time"
CONF_DRAW_BATTERY_LEVEL = "draw_battery_level"
CONF_DARK_MODE = "dark_mode"
CONF_DRAW_VOLUME_LEVEL = "draw_volume_level"
CONF_BOOT_DEVICE_NAME = "boot_device_name"

# colors
CONF_COLORS = "colors"
CONF_GRAY_DARK = "gray_dark"
CONF_GRAY_DARK_2 = "gray_dark_2"
CONF_GRAY = "gray"
CONF_ACCENT_PRIMARY = "accent_primary"
CONF_BLACK = "black"
CONF_WHITE = "white"
CONF_PINK = "pink"
CONF_YELLOW = "yellow"

BOOT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HomeThingMenuBoot),
        cv.Optional(CONF_API, default="api_connected"): cv.use_id(binary_sensor.BinarySensor),
    }
)

MENU_DISPLAY_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HomeThingMenuDisplay),
        cv.GenerateID(CONF_TEXT_HELPERS): cv.declare_id(HomeThingMenuTextHelpers),
        cv.GenerateID(CONF_REFACTOR): cv.declare_id(HomeThingMenuRefactor),
        cv.GenerateID(CONF_NOW_PLAYING): cv.declare_id(HomeThingMenuNowPlaying),
        cv.GenerateID(CONF_HEADER): cv.declare_id(HomeThingMenuHeader),
    }
)

BATTERY_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_CHARGING): cv.use_id(binary_sensor.BinarySensor),
        cv.Required(CONF_BATTERY_PERCENT): cv.use_id(sensor.Sensor),
    }
)

MenuMode = homething_menu_base_ns.enum("MenuMode")
MENU_MODES = {
    CONF_ROTARY: MenuMode.MENU_MODE_ROTARY,
    CONF_3_BUTTON: MenuMode.MENU_MODE_3_BUTTON,
    CONF_2_BUTTON: MenuMode.MENU_MODE_2_BUTTON
}

MENU_SETTINGS_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HomeThingMenuSettings),
        cv.Optional(CONF_MODE, default=CONF_ROTARY): cv.enum(MENU_MODES),
        cv.Optional(CONF_DISPLAY_TIMEOUT, default=16): cv.int_,
        cv.Optional(CONF_SLEEP_AFTER, default=3600): cv.int_,
        cv.Optional(CONF_MENU_ROLLOVER_ON, default=False): cv.boolean,
        cv.Optional(CONF_LOCK_AFTER, default=0): cv.int_,
    }
)

COLOR_PALETTE_IDS = [
    CONF_GRAY_DARK,
    CONF_GRAY_DARK_2,
    CONF_GRAY,
    CONF_ACCENT_PRIMARY,
    CONF_BLUE,
    CONF_GREEN,
    CONF_BLACK,
    CONF_WHITE,
    CONF_PINK,
    CONF_RED,
    CONF_YELLOW
]

COLOR_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HomeThingColorPalette),
        cv.Optional(CONF_GRAY_DARK): cv.use_id(color.ColorStruct),
        cv.Optional(CONF_GRAY_DARK_2): cv.use_id(color.ColorStruct),
        cv.Optional(CONF_GRAY): cv.use_id(color.ColorStruct),
        cv.Optional(CONF_ACCENT_PRIMARY): cv.use_id(color.ColorStruct),
        cv.Optional(CONF_BLUE): cv.use_id(color.ColorStruct),
        cv.Optional(CONF_GREEN): cv.use_id(color.ColorStruct),
        cv.Optional(CONF_BLACK): cv.use_id(color.ColorStruct),
        cv.Optional(CONF_WHITE): cv.use_id(color.ColorStruct),
        cv.Optional(CONF_PINK): cv.use_id(color.ColorStruct),
        cv.Optional(CONF_RED): cv.use_id(color.ColorStruct),
        cv.Optional(CONF_YELLOW): cv.use_id(color.ColorStruct),
    },
    cv.has_at_least_one_key()
)

DisplayIconEnabledState = homething_menu_base_ns.enum("DisplayIconEnabledState")

DISPLAY_ICON_MODES = {
    CONF_OFF: DisplayIconEnabledState.OFF,
    CONF_ON: DisplayIconEnabledState.ON,
    CONF_ALWAYS: DisplayIconEnabledState.ALWAYS,
}

DISPLAY_STATE_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HomeThingMenuDisplayState),
        cv.Required(CONF_FONT_SMALL): cv.use_id(font.Font),
        cv.Required(CONF_FONT_MEDIUM): cv.use_id(font.Font),
        cv.Required(CONF_FONT_LARGE): cv.use_id(font.Font),
        cv.Required(CONF_FONT_LARGE_HEAVY): cv.use_id(font.Font),
        cv.Required(CONF_FONT_MATERIAL_LARGE): cv.use_id(font.Font),
        cv.Required(CONF_FONT_MATERIAL_SMALL): cv.use_id(font.Font),
        cv.Required(CONF_FONT_LOGO): cv.use_id(font.Font),
        cv.Optional(CONF_HEADER_HEIGHT, default=16): cv.int_,
        cv.Optional(CONF_MARGIN_SIZE, default=4): cv.int_,
        cv.Optional(CONF_BOTTOM_BAR_MARGIN, default=1): cv.int_,
        cv.Optional(CONF_SLIDER_MARGIN_SIZE, default=8): cv.int_,
        cv.Optional(CONF_ICON_SIZE, default=18): cv.int_,
        cv.Optional(CONF_SCROLL_BAR_WIDTH, default=6): cv.int_,
        cv.Optional(CONF_BOOT_LOGO_SIZE, default=48): cv.int_,
        cv.Optional(CONF_NOW_PLAYING_MAX_LINES, default=5): cv.int_,
        cv.Optional(CONF_FONT_SIZE_WIDTH_RATIO, default=0.6): cv.float_,
        cv.Optional(CONF_DRAW_SHUFFLE, default=CONF_ON): cv.enum(DISPLAY_ICON_MODES),
        cv.Optional(CONF_DRAW_REPEAT, default=CONF_ON): cv.enum(DISPLAY_ICON_MODES),
        cv.Optional(CONF_DRAW_HEADER_TIME, default=True): cv.boolean,
        cv.Optional(CONF_DRAW_BATTERY_LEVEL, default=False): cv.boolean,
        cv.Optional(CONF_DARK_MODE, default=True): cv.boolean,
        cv.Optional(CONF_DRAW_VOLUME_LEVEL, default=False): cv.boolean,
        cv.Optional(CONF_DRAW_NOW_PLAYING_BOTTOM_MENU, default=False): cv.boolean,
        cv.Optional(CONF_BOOT_DEVICE_NAME, default="homeThing"): cv.string,
        cv.Optional(CONF_COLORS, default={}): COLOR_SCHEMA,
    }
)

HEADER_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HomeThingMenuHeader),
    }
)

ServiceCalledTrigger = homething_menu_base_ns.class_(
    "ServiceCalledTrigger", automation.Trigger.template()
)

MENU_COMMAND_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(MenuCommand),
        cv.Required(CONF_COMMAND): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(
                    ServiceCalledTrigger
                ),
            }
        ),
        cv.Required(CONF_NAME): cv.string,
    }
)

MENU_ENTITY_TYPED_SCHEMA = cv.typed_schema(
    {
        CONF_SWITCH: cv.Schema(
            {
                cv.GenerateID(CONF_ID): cv.use_id(switch.Switch),
            }
        ),
        CONF_TEXT_SENSOR: cv.Schema(
            {
                cv.GenerateID(CONF_ID): cv.use_id(text_sensor.TextSensor),
            }
        ),
        CONF_COMMAND: MENU_COMMAND_SCHEMA,
        CONF_LIGHT: cv.Schema(
            {
                cv.GenerateID(CONF_ID): cv.use_id(LightState)
            }
        ),
        CONF_SENSOR: cv.Schema(
            {
                cv.GenerateID(CONF_ID): cv.use_id(sensor.Sensor)
            }
        ),
        CONF_COVER: cv.Schema(
            {
                cv.GenerateID(CONF_ID): cv.use_id(cover.Cover),
                # cv.Required(CONF_NAME): cv.string,
        },
        CONF_NUMBER: cv.Schema(
            {
                cv.GenerateID(CONF_ID): cv.use_id(number.Number)
            }
        ),
    },
    key=CONF_TYPE
)

MENU_SCREEN_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HomeThingMenuScreen),
        cv.Required(CONF_NAME): cv.string,
        cv.Optional(CONF_SHOW_VERSION, default=False): cv.boolean,
        cv.Required(CONF_ENTITIES): cv.All(
            cv.ensure_list(MENU_ENTITY_TYPED_SCHEMA), cv.Length(min=1)
        ),
    }
).extend(cv.COMPONENT_SCHEMA)

CONFIG_SCHEMA =  cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(HomeThingMenuBase),
            cv.Required(CONF_DISPLAY): cv.use_id(display.DisplayBuffer),
            cv.Optional(CONF_SETTINGS, default={}): MENU_SETTINGS_SCHEMA,
            cv.Optional(CONF_SLEEP_SWITCH): cv.use_id(switch.Switch),
            cv.Optional(CONF_BATTERY): BATTERY_SCHEMA,
            cv.Optional(CONF_BACKLIGHT): cv.use_id(light.LightState),
            cv.Required(CONF_DISPLAY_STATE): DISPLAY_STATE_SCHEMA,
            cv.Optional(CONF_HEADER, default={}): HEADER_SCHEMA,
            cv.Optional(CONF_MENU_DISPLAY, default={}): MENU_DISPLAY_SCHEMA,
            cv.Optional(CONF_MEDIA_PLAYERS): cv.use_id(homeassistant_media_player_ns.HomeAssistantMediaPlayerGroup),
            cv.Optional(CONF_SERVICE_GROUP): cv.use_id(homeassistant_service_group_ns.HomeAssistantServiceGroup),
            cv.Optional(CONF_SWITCH_GROUP): cv.use_id(homeassistant_switch_group_ns.HomeAssistantSwitchGroup),
            cv.Optional(CONF_SENSOR_GROUP): cv.use_id(homeassistant_sensor_group_ns.HomeAssistantSensorsGroup),
            cv.Optional(CONF_BOOT, default={}): BOOT_SCHEMA,
            cv.Optional(CONF_ON_REDRAW): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(
                        HomeThingDisplayMenuOnRedrawTrigger
                    )
                }
            ),
            cv.Optional(CONF_SCREENS): cv.All(
                cv.ensure_list(MENU_SCREEN_SCHEMA), cv.Length(min=1)
            ),
        }
    ).extend(cv.polling_component_schema("1s")),
    cv.has_at_least_one_key(CONF_MEDIA_PLAYERS, CONF_SERVICE_GROUP, CONF_SWITCH_GROUP, CONF_SENSORS, CONF_SCREENS)
)

async def ids_to_code(config, var, types):
    for key in types:
        if key in config:
            conf = await cg.get_variable(config[key])
            cg.add(getattr(var, f"set_{key}")(conf))

def keys_to_code(config, var, types):
    for key in types:
        if key in config:
            conf = config[key]
            cg.add(getattr(var, f"set_{key}")(conf))

MENU_SETTING_TYPES = [
    CONF_MODE,
    CONF_DISPLAY_TIMEOUT,
    CONF_SLEEP_AFTER,
    CONF_MENU_ROLLOVER_ON,
    CONF_LOCK_AFTER
]

async def menu_settings_to_code(config):
    menu_settings = cg.new_Pvariable(config[CONF_ID])
    keys_to_code(config, menu_settings, MENU_SETTING_TYPES)
    return menu_settings

DISPLAY_STATE_IDS = [
    CONF_FONT_SMALL,
    CONF_FONT_MEDIUM,
    CONF_FONT_LARGE,
    CONF_FONT_LARGE_HEAVY,
    CONF_FONT_MATERIAL_LARGE,
    CONF_FONT_MATERIAL_SMALL,
    CONF_FONT_LOGO
]

DISPLAY_STATE_TYPES = [
    CONF_DRAW_NOW_PLAYING_BOTTOM_MENU,
    CONF_HEADER_HEIGHT,
    CONF_MARGIN_SIZE,
    CONF_BOTTOM_BAR_MARGIN,
    CONF_SLIDER_MARGIN_SIZE,
    CONF_ICON_SIZE,
    CONF_SCROLL_BAR_WIDTH,
    CONF_BOOT_LOGO_SIZE,
    CONF_NOW_PLAYING_MAX_LINES,
    CONF_FONT_SIZE_WIDTH_RATIO,
    CONF_DRAW_SHUFFLE,
    CONF_DRAW_REPEAT,
    CONF_DRAW_HEADER_TIME,
    CONF_DRAW_BATTERY_LEVEL,
    CONF_DARK_MODE,
    CONF_DRAW_VOLUME_LEVEL,
    CONF_BOOT_DEVICE_NAME
]

async def display_state_to_code(config):

    display_state = cg.new_Pvariable(config[CONF_ID])
    keys_to_code(config, display_state, DISPLAY_STATE_TYPES)
    await ids_to_code(config, display_state, DISPLAY_STATE_IDS)
    
    if CONF_COLORS in config:
        color_palette = cg.new_Pvariable(config[CONF_COLORS][CONF_ID])
        await ids_to_code(config[CONF_COLORS], color_palette, COLOR_PALETTE_IDS)
        cg.add(display_state.set_color_palette(color_palette))

    return display_state

async def text_helpers_to_code(config, display_buffer, display_state):
    text_helpers = cg.new_Pvariable(config)
    cg.add(text_helpers.set_display_buffer(display_buffer))
    cg.add(text_helpers.set_display_state(display_state))
    return text_helpers

MENU_BOOT_IDS = [
    CONF_API,
    CONF_MEDIA_PLAYERS
]

async def menu_boot_to_code(config, display_buffer, display_state, menu_header, text_helpers):
    menu_boot = cg.new_Pvariable(config[CONF_ID], display_buffer, display_state, menu_header, text_helpers)
    await ids_to_code(config, menu_boot, MENU_BOOT_IDS)
    return menu_boot

BATTERY_IDS = [
    CONF_BATTERY_PERCENT,
    CONF_CHARGING,
]

async def battery_to_code(config, var):
    if CONF_BATTERY in config:
        await ids_to_code(config[CONF_BATTERY], var, BATTERY_IDS)

NOW_PLAYING_IDS = [
    CONF_MEDIA_PLAYERS
]
MENU_HEADER_IDS = [
    CONF_MEDIA_PLAYERS
]
MENU_DISPLAY_IDS = [
    CONF_MEDIA_PLAYERS
]

async def menu_display_to_code(config, display_buffer):
    menu_display_conf = config[CONF_MENU_DISPLAY]

    display_state = await display_state_to_code(config[CONF_DISPLAY_STATE])
    text_helpers = await text_helpers_to_code(menu_display_conf[CONF_TEXT_HELPERS], display_buffer, display_state)
    refactor = cg.new_Pvariable(menu_display_conf[CONF_REFACTOR], display_buffer, display_state, text_helpers)
    menu_header = cg.new_Pvariable(menu_display_conf[CONF_HEADER], display_buffer, display_state, text_helpers)
    await ids_to_code(config, menu_header, MENU_HEADER_IDS)
    await battery_to_code(config, menu_header)
    menu_boot = await menu_boot_to_code(config[CONF_BOOT], display_buffer, display_state, menu_header, text_helpers)
    await ids_to_code(config, menu_boot, MENU_BOOT_IDS)

    menu_display = cg.new_Pvariable(menu_display_conf[CONF_ID], display_buffer, display_state, text_helpers, refactor, menu_header, menu_boot)
    await ids_to_code(config, menu_display, MENU_DISPLAY_IDS)
    
    if CONF_MEDIA_PLAYERS in config:
        now_playing = cg.new_Pvariable(menu_display_conf[CONF_NOW_PLAYING], display_buffer, display_state, text_helpers)
        await ids_to_code(config, now_playing, NOW_PLAYING_IDS)
        cg.add(menu_display.set_now_playing(now_playing))
    return menu_display

async def menu_screen_to_code(config):
    menu_screen = cg.new_Pvariable(config[CONF_ID], config[CONF_NAME])

    if CONF_SHOW_VERSION in config:
        cg.add_define("SHOW_VERSION")
        cg.add(menu_screen.set_show_version(config[CONF_SHOW_VERSION]))

    for conf in config.get(CONF_ENTITIES, []):
        if conf[CONF_TYPE] == CONF_SWITCH:
            new_switch = await cg.get_variable(conf[CONF_ID])
            cg.add(menu_screen.register_switch(new_switch))
        elif conf[CONF_TYPE] == CONF_TEXT_SENSOR:
            new_text_sensor = await cg.get_variable(conf[CONF_ID])
            cg.add(menu_screen.register_text_sensor(new_text_sensor))
        elif conf[CONF_TYPE] == CONF_LIGHT:
            new_light = await cg.get_variable(conf[CONF_ID])
            cg.add(menu_screen.register_light(new_light))
        elif conf[CONF_TYPE] == CONF_COMMAND:
            cg.add_build_flag("-DUSE_COMMAND")
            service = cg.new_Pvariable(conf[CONF_ID])
            cg.add(service.set_name(conf[CONF_NAME]))
            
            for command in conf.get(CONF_COMMAND, []):
                trigger = cg.new_Pvariable(command[CONF_TRIGGER_ID], service)
                await automation.build_automation(trigger, [], command)
            cg.add(menu_screen.register_command(service))
        elif conf[CONF_TYPE] == CONF_SENSOR:
            new_sensor = await cg.get_variable(conf[CONF_ID])
            cg.add(menu_screen.register_sensor(new_sensor))
        elif conf[CONF_TYPE] == CONF_COVER:
            new_cover = await cg.get_variable(conf[CONF_ID])
            cg.add(menu_screen.register_cover(new_cover))
        elif conf[CONF_TYPE] == CONF_NUMBER:
            new_number = await cg.get_variable(conf[CONF_ID])
            cg.add(menu_screen.register_number(new_number))
    return menu_screen

MENU_IDS = [
    CONF_BACKLIGHT,
    CONF_SLEEP_SWITCH,
    CONF_MEDIA_PLAYERS,
    CONF_SERVICE_GROUP, 
    CONF_SWITCHES, 
    CONF_SENSORS
]

async def to_code(config):
    display_buffer = await cg.get_variable(config[CONF_DISPLAY])
    menu_display = await menu_display_to_code(config, display_buffer)

    menu_settings = await menu_settings_to_code(config[CONF_SETTINGS])

    menu = cg.new_Pvariable(config[CONF_ID], menu_settings, menu_display)
    await cg.register_component(menu, config)

    for conf in config.get(CONF_SCREENS, []):
        menu_screen = await menu_screen_to_code(conf)
        cg.add(menu.register_screen(menu_screen))

    await battery_to_code(config, menu)
    await ids_to_code(config, menu, MENU_IDS)

    for conf in config.get(CONF_ON_REDRAW, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], menu)
        await automation.build_automation(trigger, [(HomeThingMenuBaseConstPtr, "it")], conf)


    # animation = await menu_animation_to_code(menu)

   
