import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import font, color, image, switch
from esphome.const import  CONF_ID, CONF_RED, CONF_BLUE, CONF_GREEN, CONF_NAME

homething_display_state_ns = cg.esphome_ns.namespace("homething_display_state")
HomeThingDisplayState = homething_display_state_ns.class_("HomeThingDisplayState")
HomeThingColorPalette = homething_display_state_ns.class_("HomeThingColorPalette")
DisplayIconEnabledState = homething_display_state_ns.enum("DisplayIconEnabledState")

CONF_DISPLAY_STATE = "display_state"

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
CONF_HEADER_HEIGHT = "header_height"
CONF_MARGIN_SIZE = "margin_size"
CONF_BOTTOM_BAR_MARGIN = "bottom_bar_margin"
CONF_SLIDER_MARGIN_SIZE = "slider_margin_size"
CONF_ICON_SIZE = "icon_size"
CONF_SCROLL_BAR_WIDTH = "scroll_bar_width"
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

DISPLAY_ICON_MODES = {
    CONF_OFF: DisplayIconEnabledState.OFF,
    CONF_ON: DisplayIconEnabledState.ON,
    CONF_ALWAYS: DisplayIconEnabledState.ALWAYS,
}

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HomeThingDisplayState),
        cv.Optional(CONF_DARK_MODE): cv.use_id(switch.Switch),
        cv.Required(CONF_FONT_SMALL): cv.use_id(font.Font),
        cv.Required(CONF_FONT_MEDIUM): cv.use_id(font.Font),
        cv.Required(CONF_FONT_LARGE): cv.use_id(font.Font),
        cv.Required(CONF_FONT_LARGE_HEAVY): cv.use_id(font.Font),
        cv.Required(CONF_FONT_MATERIAL_LARGE): cv.use_id(font.Font),
        cv.Required(CONF_FONT_MATERIAL_SMALL): cv.use_id(font.Font),
        cv.Optional(CONF_HEADER_HEIGHT, default=16): cv.int_,
        cv.Optional(CONF_MARGIN_SIZE, default=4): cv.int_,
        cv.Optional(CONF_BOTTOM_BAR_MARGIN, default=1): cv.int_,
        cv.Optional(CONF_SLIDER_MARGIN_SIZE, default=8): cv.int_,
        cv.Optional(CONF_ICON_SIZE, default=18): cv.int_,
        cv.Optional(CONF_SCROLL_BAR_WIDTH, default=6): cv.int_,
        cv.Optional(CONF_FONT_SIZE_WIDTH_RATIO, default=0.6): cv.float_,
        cv.Optional(CONF_DRAW_SHUFFLE, default=CONF_ON): cv.enum(DISPLAY_ICON_MODES),
        cv.Optional(CONF_DRAW_REPEAT, default=CONF_ON): cv.enum(DISPLAY_ICON_MODES),
        cv.Optional(CONF_DRAW_HEADER_TIME, default=True): cv.boolean,
        cv.Optional(CONF_DRAW_BATTERY_LEVEL, default=False): cv.boolean,
        cv.Optional(CONF_DRAW_VOLUME_LEVEL, default=False): cv.boolean,
        cv.Optional(CONF_BOOT_DEVICE_NAME, default="homeThing"): cv.string,
        cv.Optional(CONF_COLORS, default={}): COLOR_SCHEMA,
    }
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

DISPLAY_STATE_IDS = [
    CONF_FONT_SMALL,
    CONF_FONT_MEDIUM,
    CONF_FONT_LARGE,
    CONF_FONT_LARGE_HEAVY,
    CONF_FONT_MATERIAL_LARGE,
    CONF_FONT_MATERIAL_SMALL
]

DISPLAY_STATE_TYPES = [
    CONF_HEADER_HEIGHT,
    CONF_MARGIN_SIZE,
    CONF_BOTTOM_BAR_MARGIN,
    CONF_SLIDER_MARGIN_SIZE,
    CONF_ICON_SIZE,
    CONF_SCROLL_BAR_WIDTH,
    CONF_FONT_SIZE_WIDTH_RATIO,
    CONF_DRAW_SHUFFLE,
    CONF_DRAW_REPEAT,
    CONF_DRAW_HEADER_TIME,
    CONF_DRAW_BATTERY_LEVEL,
    CONF_DRAW_VOLUME_LEVEL,
    CONF_BOOT_DEVICE_NAME
]

async def to_code(config):
    display_state = cg.new_Pvariable(config[CONF_ID])
    keys_to_code(config, display_state, DISPLAY_STATE_TYPES)
    await ids_to_code(config, display_state, DISPLAY_STATE_IDS)
    
    if CONF_COLORS in config:
        color_palette = cg.new_Pvariable(config[CONF_COLORS][CONF_ID])
        await ids_to_code(config[CONF_COLORS], color_palette, COLOR_PALETTE_IDS)
        cg.add(display_state.set_color_palette(color_palette))

    if CONF_DARK_MODE in config:
        dark_mode = await cg.get_variable(config[CONF_DARK_MODE])
        cg.add(display_state.set_dark_mode_switch(dark_mode))
