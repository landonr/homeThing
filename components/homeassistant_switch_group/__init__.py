import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

homeassistant_switch_group_ns = cg.esphome_ns.namespace("homeassistant_switch_group")

DEPENDENCIES = ["api", "homeassistant_switch"]
HomeAssistantSwitchGroup = homeassistant_switch_group_ns.class_(
    'HomeAssistantSwitchGroup', cg.Component
)

CONFIG_SCHEMA = cv.COMPONENT_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(HomeAssistantSwitchGroup)
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)