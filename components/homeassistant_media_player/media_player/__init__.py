import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import media_player, homeassistant_component, media_player_source
from esphome.const import CONF_ENTITY_ID, CONF_NAME, CONF_ID, CONF_INTERNAL, CONF_DISABLED_BY_DEFAULT

homeassistant_media_player_ns = cg.esphome_ns.namespace("homeassistant_media_player")

AUTO_LOAD = ['media_player', 'homeassistant_component', 'media_player_source']

HomeAssistantBaseMediaPlayer = homeassistant_media_player_ns.class_("HomeAssistantBaseMediaPlayer", media_player.MediaPlayer, cg.Component)
HomeAssistantSpeakerMediaPlayer = homeassistant_media_player_ns.class_("HomeAssistantSpeakerMediaPlayer", media_player.MediaPlayer, cg.Component)
HomeAssistantTVMediaPlayer = homeassistant_media_player_ns.class_("HomeAssistantTVMediaPlayer", media_player.MediaPlayer, cg.Component)
HomeAssistantTVRokuMediaPlayer = homeassistant_media_player_ns.class_("HomeAssistantTVRokuMediaPlayer", media_player.MediaPlayer, cg.Component)
HomeAssistantTVKodiMediaPlayer = homeassistant_media_player_ns.class_("HomeAssistantTVKodiMediaPlayer", media_player.MediaPlayer, cg.Component)
HomeAssistantTVSamsungMediaPlayer = homeassistant_media_player_ns.class_("HomeAssistantTVSamsungMediaPlayer", media_player.MediaPlayer, cg.Component)

CONF_SPEAKER = "speaker"
CONF_TV = "tv"
CONF_ROKU = "roku"
CONF_KODI = "kodi"
CONF_SAMSUNG = "samsung"
CONF_SOUNDBAR = "soundbar"
CONF_SOURCES = "sources"

MEDIA_PLAYER_COMMON_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ENTITY_ID): cv.entity_id,
        cv.Required(CONF_NAME): cv.string,
        cv.Optional(CONF_INTERNAL, default=True): cv.boolean,
        cv.Optional(CONF_DISABLED_BY_DEFAULT, default=True): cv.boolean,
        cv.Optional(CONF_SOURCES): cv.All(
            cv.ensure_list(media_player_source.SOURCE_REFERENCE_SCHEMA), cv.Length(min=1)
        ),
    }
).extend(homeassistant_component.COMPONENT_CONFIG_SCHEMA)

TV_CONFIG_SCHEMA = MEDIA_PLAYER_COMMON_SCHEMA.extend(
    {
        cv.Optional(CONF_SOUNDBAR): cv.Schema(
            {
                cv.Optional(CONF_SPEAKER): cv.use_id(HomeAssistantSpeakerMediaPlayer)
            },
            cv.has_exactly_one_key(CONF_SPEAKER),
        ),
    },
)

CONFIG_SCHEMA = cv.typed_schema(
    {
        CONF_SPEAKER: MEDIA_PLAYER_COMMON_SCHEMA.extend(
            {
                cv.GenerateID(CONF_ID): cv.declare_id(HomeAssistantSpeakerMediaPlayer),
            }
        ),
        CONF_TV: TV_CONFIG_SCHEMA.extend(
            {
                cv.GenerateID(CONF_ID): cv.declare_id(HomeAssistantTVMediaPlayer),
            }
        ),
        CONF_ROKU: TV_CONFIG_SCHEMA.extend(
            {
                cv.GenerateID(CONF_ID): cv.declare_id(HomeAssistantTVRokuMediaPlayer),
            }
        ),
        CONF_KODI: TV_CONFIG_SCHEMA.extend(
            {
                cv.GenerateID(CONF_ID): cv.declare_id(HomeAssistantTVKodiMediaPlayer),
            }
        ),
        CONF_SAMSUNG: TV_CONFIG_SCHEMA.extend(
            {
                cv.GenerateID(CONF_ID): cv.declare_id(HomeAssistantTVSamsungMediaPlayer),
            }
        ),
    },
    lower=True,
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await media_player.register_media_player(var, config)
    homeassistant_component.base_to_code(var, config)

    if CONF_SOUNDBAR in config:
        if CONF_SPEAKER in config[CONF_SOUNDBAR]:
            soundbar = await cg.get_variable(config[CONF_SOUNDBAR][CONF_SPEAKER])
            cg.add(var.set_soundbar(soundbar))
            cg.add(soundbar.set_tv(var))

    

    if CONF_SOURCES in config:
        for conf in config.get(CONF_SOURCES, []):
            new_source = await cg.get_variable(conf[CONF_ID])
            cg.add(var.register_source(new_source))