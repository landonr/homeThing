import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import media_player
from esphome.const import CONF_ENTITY_ID, CONF_NAME, CONF_ID, CONF_INTERNAL, CONF_DISABLED_BY_DEFAULT

homeassistant_media_player_ns = cg.esphome_ns.namespace("homeassistant_media_player")

AUTO_LOAD = ['media_player']

HomeAssistantBaseMediaPlayer = homeassistant_media_player_ns.class_("HomeAssistantBaseMediaPlayer", media_player.MediaPlayer, cg.Component)
HomeAssistantSonosMediaPlayer = homeassistant_media_player_ns.class_("HomeAssistantSonosMediaPlayer", media_player.MediaPlayer, cg.Component)
HomeAssistantRokuMediaPlayer = homeassistant_media_player_ns.class_("HomeAssistantRokuMediaPlayer", media_player.MediaPlayer, cg.Component)

CONF_SONOS = "sonos"
CONF_ROKU = "roku"
CONF_SOUNDBAR = "soundbar"

MEDIA_PLAYER_COMMON_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(HomeAssistantBaseMediaPlayer),
        cv.Required(CONF_ENTITY_ID): cv.entity_id,
        cv.Required(CONF_NAME): cv.string,
        cv.Optional(CONF_INTERNAL, default=True): cv.boolean,
        cv.Optional(CONF_DISABLED_BY_DEFAULT, default=True): cv.boolean,
    }
).extend(cv.COMPONENT_SCHEMA)

CONFIG_SCHEMA = cv.typed_schema(
    {
        CONF_SONOS: MEDIA_PLAYER_COMMON_SCHEMA.extend(
            {
                cv.GenerateID(CONF_ID): cv.declare_id(HomeAssistantSonosMediaPlayer),
            }
        ),
        CONF_ROKU: MEDIA_PLAYER_COMMON_SCHEMA.extend(
            {
                cv.GenerateID(CONF_ID): cv.declare_id(HomeAssistantRokuMediaPlayer),
                cv.Optional(CONF_SOUNDBAR): cv.Schema(
                    {
                        cv.Optional(CONF_SONOS): cv.use_id(HomeAssistantSonosMediaPlayer)
                    },
                    cv.has_exactly_one_key(CONF_SONOS),
                ),
            },
        ),
    },
    lower=True,
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await media_player.register_media_player(var, config)
    cg.add(var.set_entity_id(config[CONF_ENTITY_ID]))
    cg.add(var.set_name(config[CONF_NAME]))
    cg.add(var.set_internal(config[CONF_INTERNAL]))
    cg.add(var.set_disabled_by_default(config[CONF_DISABLED_BY_DEFAULT]))

    if CONF_SOUNDBAR in config:
        if CONF_SONOS in config[CONF_SOUNDBAR]:
            soundbar = await cg.get_variable(config[CONF_SOUNDBAR][CONF_SONOS])
            cg.add(var.set_soundbar(soundbar))