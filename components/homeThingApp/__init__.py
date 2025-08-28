import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ENTITY_ID, CONF_ID, CONF_NAME
from esphome.components import display
from esphome.components.homeThingDisplayState import homething_display_state_ns

homething_app_ns = cg.esphome_ns.namespace("homething_menu_app")

CONF_DISPLAY_STATE = "display_state"
CONF_DISPLAY = "display"

HomeThingApp = homething_app_ns.class_("HomeThingApp")

BASE_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_DISPLAY_STATE): cv.use_id(homething_display_state_ns.HomeThingDisplayState),
        cv.Required(CONF_DISPLAY): cv.use_id(display.Display),
    }
).extend(cv.COMPONENT_SCHEMA)

async def new_app_base(config):
    cg.add_build_flag("-DUSE_HOMETHING_APP")
    var = cg.new_Pvariable(config[CONF_ID])
    display = await cg.get_variable(config[CONF_DISPLAY])
    cg.add(var.set_display(display))
    display_state = await cg.get_variable(config[CONF_DISPLAY_STATE])
    cg.add(var.set_display_state(display_state))
    return var