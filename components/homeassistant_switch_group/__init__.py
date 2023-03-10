import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components.homeassistant_switch_group.switch import HomeAssistantSwitch
from esphome.const import CONF_ID, CONF_ENTITY_ID, CONF_NAME
from esphome.components import sensor

homeassistant_switch_group_ns = cg.esphome_ns.namespace("homeassistant_switch_group")

CONF_SWITCHES = "switches"
CONF_SWITCH = "switch"

DEPENDENCIES = ["api"]

HOMEASSISTANT_SWITCH_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(HomeAssistantSwitch)
    }
)


HomeAssistantSwitchGroup = homeassistant_switch_group_ns.class_(
    'HomeAssistantSwitchGroup', cg.Component, sensor.Sensor
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HomeAssistantSwitchGroup),
        cv.Required(CONF_SWITCHES): cv.All(
            cv.ensure_list(HOMEASSISTANT_SWITCH_SCHEMA), cv.Length(min=1)
        ),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    for conf in config.get(CONF_SWITCHES, []):
        newSwitch = await cg.get_variable(conf[CONF_ID])
        cg.add(var.register_switch(newSwitch))
