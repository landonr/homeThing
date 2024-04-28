import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.automation import maybe_simple_id
from esphome.const import CONF_ID

from .types import (
    HomeThingMenuBase,
    UpAction,
    DownAction,
    LeftAction,
    RightAction,
    SelectAction,
    ScrollClockwiseAction,
    ScrollCounterClockwiseAction,
    BackAction,
    OptionAction,
    HomeAction,
    AddNotificationAction,
    ClearNotificationsAction,
)

MENU_ACTION_SCHEMA = automation.maybe_simple_id(
    {
        cv.GenerateID(CONF_ID): cv.use_id(HomeThingMenuBase),
    }
)

# Menu Button Actions
@automation.register_action("homething_menu.up", UpAction, MENU_ACTION_SCHEMA)
async def menu_up_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

@automation.register_action("homething_menu.down", DownAction, MENU_ACTION_SCHEMA)
async def menu_down_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

@automation.register_action("homething_menu.left", LeftAction, MENU_ACTION_SCHEMA)
async def menu_left_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

@automation.register_action("homething_menu.right", RightAction, MENU_ACTION_SCHEMA)
async def menu_right_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

@automation.register_action("homething_menu.select", SelectAction, MENU_ACTION_SCHEMA)
async def menu_select_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

@automation.register_action("homething_menu.scroll_clockwise", ScrollClockwiseAction, MENU_ACTION_SCHEMA)
async def menu_scroll_clockwise_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

@automation.register_action("homething_menu.scroll_counter_clockwise", ScrollCounterClockwiseAction, MENU_ACTION_SCHEMA)
async def menu_scroll_counter_clockwise_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

# Menu Navigation Actions
@automation.register_action("homething_menu.back", BackAction, MENU_ACTION_SCHEMA)
async def menu_back_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

@automation.register_action("homething_menu.option", OptionAction, MENU_ACTION_SCHEMA)
async def menu_option_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

@automation.register_action("homething_menu.home", HomeAction, MENU_ACTION_SCHEMA)
async def menu_home_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)

# Notification Actions
CONF_TITLE = "title"
CONF_SUBTITLE = "subtitle"
CONF_MESSAGE = "message"
CONF_PERSISTENT = "persistent"

ADD_NOTIFICATION_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(HomeThingMenuBase),
        cv.Required(CONF_TITLE): cv.templatable(cv.string),
        cv.Optional(CONF_SUBTITLE, default=""): cv.templatable(cv.string),
        cv.Optional(CONF_MESSAGE, default=""): cv.templatable(cv.string),
        cv.Optional(CONF_PERSISTENT, default=False): cv.boolean,
    }
)

@automation.register_action("homething_menu.add_notification", AddNotificationAction, ADD_NOTIFICATION_SCHEMA)
async def menu_add_notification_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    if CONF_TITLE in config:
        templ = await cg.templatable(config[CONF_TITLE], args, cg.std_string)
        cg.add(var.set_title(templ))
    if CONF_SUBTITLE in config:
        templ = await cg.templatable(config[CONF_SUBTITLE], args, cg.std_string)
        cg.add(var.set_subtitle(templ))
    if CONF_MESSAGE in config:
        templ = await cg.templatable(config[CONF_MESSAGE], args, cg.std_string)
        cg.add(var.set_message(templ))
    if CONF_PERSISTENT in config:
        cg.add(var.set_persistent(config[CONF_PERSISTENT]))

    return var

@automation.register_action("homething_menu.clear_notifications", ClearNotificationsAction, MENU_ACTION_SCHEMA)
async def menu_clear_notifications_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)
