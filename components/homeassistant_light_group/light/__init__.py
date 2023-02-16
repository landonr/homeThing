import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import light
from esphome.const import CONF_ENTITY_ID, CONF_OUTPUT_ID, CONF_NAME, CONF_ID

homeassistant_light_ns = cg.esphome_ns.namespace("homeassistant_light")

AUTO_LOAD = ['light']

HomeAssistantLight = homeassistant_light_ns.class_("HomeAssistantLight", light.LightOutput, cg.Component)
HomeAssistantLightState = homeassistant_light_ns.class_("HomeAssistantLightState", light.LightState)

CONFIG_SCHEMA = light.LIGHT_SCHEMA.extend(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(HomeAssistantLightState),
        cv.GenerateID(CONF_OUTPUT_ID): cv.declare_id(HomeAssistantLight),
        cv.Required(CONF_ENTITY_ID): cv.entity_id,
        cv.Required(CONF_NAME): cv.string
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_OUTPUT_ID])
    cg.add(var.set_entity_id(config[CONF_ENTITY_ID]))
    cg.add(var.set_name(config[CONF_NAME]))
    await cg.register_component(var, config)
    await light.register_light(var, config)