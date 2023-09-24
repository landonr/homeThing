import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import homeThingApp, remote_transmitter

CONF_MEDIA_PLAYERS = "media_player_group"
CONF_REMOTE_TRANSMITTER = "remote_transmitter"
AUTO_LOAD = ["homeThingApp"]
homething_cattoy_app_ns = cg.esphome_ns.namespace("homething_cattoy_app")

HomeThingCatToyApp = homething_cattoy_app_ns.class_("HomeThingCatToyApp", cg.Component)

CONFIG_SCHEMA = homeThingApp.BASE_SCHEMA.extend(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(HomeThingCatToyApp),
        cv.Required(CONF_REMOTE_TRANSMITTER): cv.use_id(remote_transmitter.RemoteTransmitterComponent)
    }
)

async def to_code(config):
    var = await homeThingApp.new_app_base(config)
    remote_transmitter = await cg.get_variable(config[CONF_REMOTE_TRANSMITTER])
    cg.add(var.set_remote_transmitter(remote_transmitter))
    await cg.register_component(var, config)