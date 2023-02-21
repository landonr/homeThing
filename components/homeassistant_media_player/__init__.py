import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components.homeassistant_media_player.media_player import HomeAssistantSonosMediaPlayer
from esphome.const import CONF_ID, CONF_ENTITY_ID, CONF_NAME

homeassistant_media_player_ns = cg.esphome_ns.namespace("homeassistant_media_player")

CONF_MEDIA_PLAYERS = "media_players"

DEPENDENCIES = ["api"]

HOMEASSISTANT_MEDIA_PLAYER_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(HomeAssistantSonosMediaPlayer)
    }
)


HomeAssistantMediaPlayerGroup = homeassistant_media_player_ns.class_(
    'HomeAssistantMediaPlayerGroup', cg.Component
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HomeAssistantMediaPlayerGroup),
        cv.Required(CONF_MEDIA_PLAYERS): cv.All(
            cv.ensure_list(HOMEASSISTANT_MEDIA_PLAYER_SCHEMA), cv.Length(min=1)
        ),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    for conf in config.get(CONF_MEDIA_PLAYERS, []):
        new_media_player = await cg.get_variable(conf[CONF_ID])
        cg.add(var.register_media_player(new_media_player))
