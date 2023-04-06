import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ENTITY_ID, CONF_ID, CONF_NAME

media_player_source_ns = cg.esphome_ns.namespace("media_player_source")

MediaPlayerSourceBase = media_player_source_ns.class_("MediaPlayerSourceBase", cg.Component)

CONF_SONOS = "sonos"
CONF_SPOTIFY = "spotify"
CONF_SOURCES = "sources"

SonosSourceComponent = cg.esphome_ns.namespace("media_player_source_sonos").class_("SonosSourceComponent", cg.Component)
SpotifySourceComponent = cg.esphome_ns.namespace("media_player_source_spotify").class_("SpotifySourceComponent", cg.Component)
SOURCE_REFERENCE_SCHEMA = cv.typed_schema(
    {
        CONF_SONOS: cv.Schema(
            {
                cv.GenerateID(CONF_ID): cv.use_id(SonosSourceComponent),
            }
        ),
        CONF_SPOTIFY: cv.Schema(
            {
                cv.GenerateID(CONF_ID): cv.use_id(SpotifySourceComponent),
            }
        ),
    }
)

BASE_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_ENTITY_ID): cv.entity_id,
        cv.Optional(CONF_NAME): cv.string,
    }
).extend(cv.COMPONENT_SCHEMA)

def new_source_base(config):
    var = cg.new_Pvariable(config[CONF_ID])
    
    if CONF_ENTITY_ID in config:
        cg.add(var.set_entity_id(config[CONF_ENTITY_ID]))
    if CONF_NAME in config:
        cg.add(var.set_name(config[CONF_NAME]))
    return var
# async def to_code(config):
#     var = cg.new_Pvariable(config[CONF_ID])
#     await cg.register_component(var, config)