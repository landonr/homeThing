import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import media_player_source

AUTO_LOAD = ["media_player_source"]

sonos_source_component_ns = cg.esphome_ns.namespace("sonos_source_component")

SonosSourceComponent = sonos_source_component_ns.class_("SonosSourceComponent", cg.Component)

CONFIG_SCHEMA = media_player_source.BASE_SCHEMA.extend(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(SonosSourceComponent)
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)