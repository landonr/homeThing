import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_ENTITY_ID, CONF_NAME
from esphome.components.homeassistant_media_player.media_player import (
    homeassistant_media_player_ns, 
    HomeAssistantSpeakerMediaPlayer, 
    HomeAssistantRokuMediaPlayer,
    CONF_SPEAKER,
    CONF_ROKU,
)
from esphome.components import sensor

CONF_MEDIA_PLAYERS = "media_players"

DEPENDENCIES = ["api"]

HOMEASSISTANT_MEDIA_PLAYER_REFERENCE_SCHEMA = cv.typed_schema(
    {
        CONF_SPEAKER: cv.Schema(
            {
                cv.GenerateID(CONF_ID): cv.use_id(HomeAssistantSpeakerMediaPlayer),
            }
        ),
        CONF_ROKU: cv.Schema(
            {
                cv.GenerateID(CONF_ID): cv.use_id(HomeAssistantRokuMediaPlayer),
            }
        ),
    },
    lower=True,
)

HomeAssistantMediaPlayerGroup = homeassistant_media_player_ns.class_(
    'HomeAssistantMediaPlayerGroup', cg.Component, sensor.Sensor
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HomeAssistantMediaPlayerGroup),
        cv.Required(CONF_MEDIA_PLAYERS): cv.All(
            cv.ensure_list(HOMEASSISTANT_MEDIA_PLAYER_REFERENCE_SCHEMA), cv.Length(min=1)
        ),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    for conf in config.get(CONF_MEDIA_PLAYERS, []):
        new_media_player = await cg.get_variable(conf[CONF_ID])
        cg.add(var.register_media_player(new_media_player))