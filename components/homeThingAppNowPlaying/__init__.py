import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import homeThingApp
from esphome.components.homeassistant_media_player import homeassistant_media_player_ns
CONF_MEDIA_PLAYERS = "media_player_group"
CONF_DISPLAY = "display"
CONF_DISPLAY_STATE = "display_state"
AUTO_LOAD = ["homeThingApp"]

homething_menu_now_playing_ns = cg.esphome_ns.namespace("homething_menu_now_playing")

HomeThingMenuNowPlayingControl = homething_menu_now_playing_ns.class_("HomeThingMenuNowPlayingControl", cg.Component)

CONF_REMOTE_TRANSMITTER = "remote_transmitter"
CONFIG_SCHEMA = homeThingApp.BASE_SCHEMA.extend(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(HomeThingMenuNowPlayingControl),
        cv.Required(CONF_MEDIA_PLAYERS): cv.use_id(homeassistant_media_player_ns.HomeAssistantMediaPlayerGroup)
    }
)

async def to_code(config):
    var = await homeThingApp.new_app_base(config)
    media_players = await cg.get_variable(config[CONF_MEDIA_PLAYERS])
    cg.add(var.set_media_player_group(media_players))

    display_buffer = await cg.get_variable(config[CONF_DISPLAY])
    display_state = await cg.get_variable(config[CONF_DISPLAY_STATE])
    cg.add(var.set_now_playing_display(display_buffer, display_state, media_players))
    await cg.register_component(var, config)