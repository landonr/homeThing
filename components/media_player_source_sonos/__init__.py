import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_NAME
from esphome.components import media_player_source

AUTO_LOAD = ["media_player_source"]

media_player_source_sonos_ns = cg.esphome_ns.namespace("media_player_source_sonos")

SonosSourceComponent = media_player_source_sonos_ns.class_("SonosSourceComponent", cg.Component)

CONFIG_SCHEMA = media_player_source.BASE_SCHEMA.extend(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(SonosSourceComponent),
        cv.Optional(CONF_NAME, default="Sonos"): cv.string,
    }
)

async def to_code(config):
    var = media_player_source.new_source_base(config)
    await cg.register_component(var, config)