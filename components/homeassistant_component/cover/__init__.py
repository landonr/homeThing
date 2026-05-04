import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import cover, homeassistant_component
from esphome.const import CONF_ENTITY_ID, CONF_NAME, CONF_ID, CONF_INTERNAL

homeassistant_cover_ns = cg.esphome_ns.namespace("homeassistant_cover")

AUTO_LOAD = ['cover', 'homeassistant_component']

HomeAssistantCover = homeassistant_cover_ns.class_("HomeAssistantCover", cover.Cover, cg.Component, cg.EntityBase)

COVER_SCHEMA = cover.COVER_SCHEMA.extend(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(HomeAssistantCover),
        cv.Required(CONF_ENTITY_ID): cv.entity_id,
        cv.Required(CONF_NAME): cv.string,
        cv.Optional(CONF_INTERNAL, default=True): cv.boolean,
    }
).extend(homeassistant_component.COMPONENT_CONFIG_SCHEMA)

CONFIG_SCHEMA = homeassistant_component.COMPONENT_CONFIG_SCHEMA.extend(COVER_SCHEMA)

async def to_code(config):
    cg.add_build_flag("-DUSE_API_COVER")
    var = cg.new_Pvariable(config[CONF_ID])
    
    await cg.register_component(var, config)
    await cover.register_cover(var, config)
    homeassistant_component.base_to_code(var, config)
    return var