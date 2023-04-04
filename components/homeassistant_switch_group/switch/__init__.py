import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch, homeassistant_component
from esphome.const import CONF_ATTRIBUTE, CONF_ENTITY_ID, CONF_ID, CONF_NAME, CONF_INTERNAL

homeassistant_switch_ns = cg.esphome_ns.namespace("homeassistant_switch")

AUTO_LOAD = ['switch']

HomeAssistantSwitch = homeassistant_switch_ns.class_("HomeAssistantSwitch", switch.Switch, cg.Component)

CONFIG_SCHEMA = switch.SWITCH_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(HomeAssistantSwitch),
        cv.Required(CONF_ENTITY_ID): cv.entity_id,
        cv.Required(CONF_NAME): cv.string,
        cv.Optional(CONF_ATTRIBUTE): cv.string,
        cv.Optional(CONF_INTERNAL, default=True): cv.boolean,
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await switch.register_switch(var, config)
    homeassistant_component.base_to_code(var, config)
    if CONF_ATTRIBUTE in config:
        cg.add(var.set_attribute(config[CONF_ATTRIBUTE]))
