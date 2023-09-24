import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import display, font, color, binary_sensor, sensor, switch, light, text_sensor, number, cover, time, button, image, remote_transmitter
from esphome.components.light import LightState
from esphome.const import  CONF_ID, CONF_TRIGGER_ID, CONF_MODE, CONF_RED, CONF_BLUE, CONF_GREEN, CONF_NAME, CONF_TYPE, CONF_TIME_ID
from esphome.components.homeassistant_media_player import homeassistant_media_player_ns
from esphome.components.homeThingDisplayState import homething_display_state_ns
homething_menu_base_ns = cg.esphome_ns.namespace("homething_menu_base")

HomeThingMenuBase = homething_menu_base_ns.class_("HomeThingMenuBase", cg.PollingComponent)
HomeThingMenuScreen = homething_menu_base_ns.class_("HomeThingMenuScreen")
MenuCommand = homething_menu_base_ns.class_("MenuCommand")
HomeThingMenuBoot = homething_menu_base_ns.class_("HomeThingMenuBoot")
HomeThingMenuSettings = homething_menu_base_ns.class_("HomeThingMenuSettings")
HomeThingMenuAnimation = homething_menu_base_ns.class_("HomeThingMenuAnimation")
HomeThingMenuDisplay = homething_menu_base_ns.class_("HomeThingMenuDisplay")
HomeThingMenuHeader = homething_menu_base_ns.class_("HomeThingMenuHeader")
HomeThingMenuTextHelpers = homething_menu_base_ns.class_("HomeThingMenuTextHelpers")
HomeThingMenuRefactor = homething_menu_base_ns.class_("HomeThingMenuRefactor")

homething_menu_now_playing_ns = cg.esphome_ns.namespace("homething_menu_now_playing")
HomeThingMenuNowPlaying = homething_menu_now_playing_ns.class_("HomeThingMenuNowPlaying")
HomeThingMenuNowPlayingControl = homething_menu_now_playing_ns.class_("HomeThingMenuNowPlayingControl")

HomeThingMenuApp = cg.esphome_ns.namespace("homething_menu_app").class_("HomeThingMenuApp")
HomeThingCatToyApp = cg.esphome_ns.namespace("homething_cattoy_app").class_("HomeThingCatToyApp")

HomeThingMenuBaseConstPtr = HomeThingMenuBase.operator("ptr").operator("const")
HomeThingDisplayMenuOnRedrawTrigger = homething_menu_base_ns.class_("HomeThingDisplayMenuOnRedrawTrigger", automation.Trigger)

HomeThingAppSnake = cg.esphome_ns.namespace("homething_app_snake").class_("HomeThingAppSnake")

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
CONF_ON_REDRAW = "on_redraw"
CONF_SCREENS = "screens"
CONF_HOME_SCREEN = "home_screen"
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
CONF_BUTTON = "button"
CONF_APPS = "apps"
CONF_APP = "app"
CONF_CATTOY_APP = "cattoy_app"
CONF_REMOTE_TRANSMITTER = "remote_transmitter"
CONF_SNAKE = "snake"

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
CONF_MENU_ROLLBACK_ON = "menu_rollback"
CONF_SLEEP_SWITCH = "sleep_switch"
CONF_SLEEP_AFTER = "sleep_after"
CONF_BACKLIGHT = "backlight"
CONF_LOCK_AFTER = "lock_after"
CONF_DISPLAY_TIMEOUT_WHILE_CHARGING = "display_timeout_while_charging"

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
        cv.GenerateID(CONF_HEADER): cv.use_id(HomeThingMenuHeader),
    }
)

BATTERY_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_CHARGING): cv.use_id(binary_sensor.BinarySensor),
        cv.Required(CONF_BATTERY_PERCENT): cv.use_id(sensor.Sensor),
    }
)

APP_TYPED_SCHEMA = cv.typed_schema(
    {
        CONF_NOW_PLAYING: cv.Schema(
            {
                cv.GenerateID(CONF_ID): cv.declare_id(HomeThingMenuNowPlayingControl),
                cv.Required(CONF_MEDIA_PLAYERS): cv.use_id(homeassistant_media_player_ns.HomeAssistantMediaPlayerGroup)
            }
        ),
        CONF_APP: cv.Schema(
            {
                cv.GenerateID(CONF_ID): cv.declare_id(HomeThingMenuApp),
            }
        ),
        CONF_CATTOY_APP: cv.Schema(
            {
                cv.GenerateID(CONF_ID): cv.declare_id(HomeThingCatToyApp),
                cv.Required(CONF_REMOTE_TRANSMITTER): cv.use_id(remote_transmitter.RemoteTransmitterComponent)
            }
        ),
        CONF_SNAKE: cv.Schema(
            {
                cv.GenerateID(CONF_ID): cv.declare_id(HomeThingAppSnake)
            }
        )
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
        cv.Optional(CONF_MENU_ROLLBACK_ON, default=False): cv.boolean,
        cv.Optional(CONF_LOCK_AFTER, default=0): cv.int_,
        cv.Optional(CONF_DISPLAY_TIMEOUT_WHILE_CHARGING, default=0): cv.int_,
    }
)

HEADER_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HomeThingMenuHeader),
        cv.Optional(CONF_TIME_ID): cv.use_id(time.RealTimeClock),
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
            }
        ),
        CONF_NUMBER: cv.Schema(
            {
                cv.GenerateID(CONF_ID): cv.use_id(number.Number)
            }
        ),
        CONF_BUTTON: cv.Schema(
            {
                cv.GenerateID(CONF_ID): cv.use_id(button.Button)
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
            cv.Required(CONF_DISPLAY_STATE): cv.use_id(homething_display_state_ns.HomeThingDisplayState),
            cv.Optional(CONF_HEADER, default={}): HEADER_SCHEMA,
            cv.Optional(CONF_MENU_DISPLAY, default={}): MENU_DISPLAY_SCHEMA,
            cv.Optional(CONF_APPS): cv.All(
                cv.ensure_list(APP_TYPED_SCHEMA), cv.Length(min=1)
            ),
            cv.Optional(CONF_BOOT, default={}): BOOT_SCHEMA,
            cv.Optional(CONF_ON_REDRAW): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(
                        HomeThingDisplayMenuOnRedrawTrigger
                    )
                }
            ),
            cv.Optional(CONF_HOME_SCREEN): MENU_SCREEN_SCHEMA,
            cv.Optional(CONF_SCREENS): cv.All(
                cv.ensure_list(MENU_SCREEN_SCHEMA), cv.Length(min=1)
            ),
        }
    ).extend(cv.polling_component_schema("1s")),
    cv.has_at_least_one_key(CONF_HOME_SCREEN, CONF_SCREENS)
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
    CONF_MENU_ROLLBACK_ON,
    CONF_LOCK_AFTER,
    CONF_DISPLAY_TIMEOUT_WHILE_CHARGING
]

async def menu_settings_to_code(config):
    menu_settings = cg.new_Pvariable(config[CONF_ID])
    keys_to_code(config, menu_settings, MENU_SETTING_TYPES)
    return menu_settings

async def text_helpers_to_code(config, display_buffer, display_state):
    text_helpers = cg.new_Pvariable(config)
    cg.add(text_helpers.set_display_buffer(display_buffer))
    cg.add(text_helpers.set_display_state(display_state))
    return text_helpers

MENU_BOOT_IDS = [
    CONF_API
]

async def menu_boot_to_code(config, display_buffer, display_state, menu_header, text_helpers):
    menu_boot = cg.new_Pvariable(config[CONF_BOOT][CONF_ID], display_buffer, display_state, menu_header, text_helpers)
    await ids_to_code(config[CONF_BOOT], menu_boot, MENU_BOOT_IDS)
    if CONF_APPS in config:
        for app in config[CONF_APPS]:
            if app[CONF_TYPE] == CONF_NOW_PLAYING:
                if CONF_MEDIA_PLAYERS in app:
                    media_player_group = await cg.get_variable(app[CONF_MEDIA_PLAYERS])
                    cg.add(menu_boot.set_media_player_group(media_player_group))
    return menu_boot

BATTERY_IDS = [
    CONF_BATTERY_PERCENT,
    CONF_CHARGING,
]

async def battery_to_code(config, var):
    if CONF_BATTERY in config:
        await ids_to_code(config[CONF_BATTERY], var, BATTERY_IDS)

async def menu_display_to_code(config, display_buffer, display_state, text_helpers):
    menu_display_conf = config[CONF_MENU_DISPLAY]

    refactor = cg.new_Pvariable(menu_display_conf[CONF_REFACTOR], display_buffer, display_state, text_helpers)
    menu_header = cg.new_Pvariable(menu_display_conf[CONF_HEADER], display_buffer, display_state, text_helpers)
    if CONF_TIME_ID in config[CONF_HEADER]:
        time_ = await cg.get_variable(config[CONF_HEADER][CONF_TIME_ID])
        cg.add(menu_header.set_time_id(time_))
    await battery_to_code(config, menu_header)
    menu_boot = await menu_boot_to_code(config, display_buffer, display_state, menu_header, text_helpers)
    await ids_to_code(config, menu_boot, MENU_BOOT_IDS)

    menu_display = cg.new_Pvariable(menu_display_conf[CONF_ID], menu_boot, display_buffer, display_state, text_helpers, refactor, menu_header)

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
        elif conf[CONF_TYPE] == CONF_BUTTON:
            new_button = await cg.get_variable(conf[CONF_ID])
            cg.add(menu_screen.register_button(new_button))
    return menu_screen

MENU_IDS = [
    CONF_BACKLIGHT,
    CONF_SLEEP_SWITCH
]

async def to_code(config):
    display_buffer = await cg.get_variable(config[CONF_DISPLAY])
    display_state = await cg.get_variable(config[CONF_DISPLAY_STATE])
    text_helpers = await text_helpers_to_code(config[CONF_MENU_DISPLAY][CONF_TEXT_HELPERS], display_buffer, display_state)
    menu_display = await menu_display_to_code(config, display_buffer, display_state, text_helpers)

    menu_settings = await menu_settings_to_code(config[CONF_SETTINGS])

    menu = cg.new_Pvariable(config[CONF_ID], menu_settings, menu_display)
    await cg.register_component(menu, config)

    if CONF_HOME_SCREEN in config:
        home_screen = await menu_screen_to_code(config[CONF_HOME_SCREEN])
        cg.add(menu.register_home_screen(home_screen))

    for conf in config.get(CONF_SCREENS, []):
        menu_screen = await menu_screen_to_code(conf)
        cg.add(menu.register_screen(menu_screen))

    if CONF_APPS in config:
        for app in config[CONF_APPS]:
            if app[CONF_TYPE] == CONF_NOW_PLAYING:
                now_playing_control = cg.new_Pvariable(app[CONF_ID])
                if CONF_MEDIA_PLAYERS in app:
                    media_player_group = await cg.get_variable(app[CONF_MEDIA_PLAYERS])
                    cg.add(now_playing_control.set_media_player_group(media_player_group))
                    cg.add(now_playing_control.set_now_playing_display(display_buffer, display_state, text_helpers, media_player_group))
                    cg.add(menu.register_app(now_playing_control))
            elif app[CONF_TYPE] == CONF_CATTOY_APP:
                cattoy_app = cg.new_Pvariable(app[CONF_ID])
                remote_transmitter = await cg.get_variable(app[CONF_REMOTE_TRANSMITTER])
                cg.add(cattoy_app.set_remote_transmitter(remote_transmitter))
                cg.add(cattoy_app.set_display_buffer(display_buffer))
                cg.add(cattoy_app.set_display_state(display_state))
                cg.add(cattoy_app.set_text_helpers(text_helpers))
                cg.add(menu.register_app(cattoy_app))
            elif app[CONF_TYPE] == CONF_SNAKE:
                snake_app = cg.new_Pvariable(app[CONF_ID])
                cg.add(snake_app.set_display_buffer(display_buffer))
                cg.add(snake_app.set_display_state(display_state))
                cg.add(snake_app.set_text_helpers(text_helpers))
                cg.add(menu.register_app(snake_app))
            else:
                app = cg.new_Pvariable(app[CONF_ID])
                cg.add(menu.register_app(app))

    await battery_to_code(config, menu)
    await ids_to_code(config, menu, MENU_IDS)

    for conf in config.get(CONF_ON_REDRAW, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], menu)
        await automation.build_automation(trigger, [(HomeThingMenuBaseConstPtr, "it")], conf)


    # animation = await menu_animation_to_code(menu)

   
