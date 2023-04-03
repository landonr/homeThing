import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ENTITY_ID, CONF_NAME, CONF_ID, CONF_INTERNAL

homeassistant_component_ns = cg.esphome_ns.namespace("homeassistant_component")

HomeAssistantComponent = homeassistant_component_ns.class_("HomeAssistantComponent", cg.Component)

CONF_HOME_ASSISTANT_COMPONENT_ID = "conf_home_assistant_component_id"
COMPONENT_CONFIG_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ENTITY_ID): cv.entity_id,
        cv.Required(CONF_NAME): cv.string,
        cv.Optional(CONF_INTERNAL, default=True): cv.boolean,
    }
).extend(cv.COMPONENT_SCHEMA)

def base_to_code(base, config):
    cg.add(base.set_entity_id(config[CONF_ENTITY_ID]))
    cg.add(base.set_name(config[CONF_NAME]))
    if CONF_INTERNAL in config:
        cg.add(base.set_internal(config[CONF_INTERNAL]))
    else:
        cg.add(base.set_internal(True))
    return base