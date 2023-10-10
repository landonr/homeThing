import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import homeThingApp

AUTO_LOAD = ["homeThingApp"]
homething_app_breakout_ns = cg.esphome_ns.namespace("homething_app_breakout")

HomeThingAppBreakout = homething_app_breakout_ns.class_("HomeThingAppBreakout", cg.Component)

CONFIG_SCHEMA = homeThingApp.BASE_SCHEMA.extend(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(HomeThingAppBreakout)
    }
)

async def to_code(config):
    var = await homeThingApp.new_app_base(config)
    await cg.register_component(var, config)