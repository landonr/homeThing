import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ENTITY_ID

media_player_source_ns = cg.esphome_ns.namespace("media_player_source")

MediaPlayerSourceBase = media_player_source_ns.class_("MediaPlayerSourceBase", cg.Component)

CONF_SONOS = "sonos"
CONF_TV = "tv"
CONF_SOURCES = "sources"

# SOURCE_REFERENCE_SCHEMA = cv.typed_schema(
#     {
#         CONF_SONOS: cv.Schema(
#             {
#                 cv.GenerateID(CONF_ID): cv.use_id(SonosSourceComponent),
#             }
#         ),
#         CONF_TV: cv.Schema(
#             {
#                 cv.GenerateID(CONF_ID): cv.use_id(SonosSourceComponent),
#             }
#         ),
#     }
# )

BASE_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_ENTITY_ID): cv.entity_id,
    }
).extend(cv.COMPONENT_SCHEMA)

# async def to_code(config):
#     var = cg.new_Pvariable(config[CONF_ID])
#     await cg.register_component(var, config)