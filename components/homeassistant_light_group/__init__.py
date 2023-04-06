import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components.homeassistant_light_group.light import HomeAssistantLight, HomeAssistantLightState
from esphome.const import CONF_ID, CONF_ENTITY_ID, CONF_NAME
from esphome.components import sensor
from .. import homeassistant_component

homeassistant_light_group_ns = cg.esphome_ns.namespace("homeassistant_light_group")

CONF_LIGHTS = "lights"
CONF_LIGHT = "light"

AUTO_LOAD = ['homeassistant_component']
DEPENDENCIES = ["api"]

HOMEASSISTANT_LIGHT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(HomeAssistantLightState)
    }
)


HomeAssistantLightGroup = homeassistant_light_group_ns.class_(
    'HomeAssistantLightGroup', cg.Component, sensor.Sensor
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HomeAssistantLightGroup),
        cv.Required(CONF_LIGHTS): cv.All(
            cv.ensure_list(HOMEASSISTANT_LIGHT_SCHEMA), cv.Length(min=1)
        ),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    for conf in config.get(CONF_LIGHTS, []):
        newLight = await cg.get_variable(conf[CONF_ID])
        cg.add(var.register_light(newLight))
