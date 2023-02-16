import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components.homeassistant.text_sensor import HomeassistantTextSensor
from esphome.const import CONF_ID, CONF_ENTITY_ID, CONF_NAME

homeassistant_sensor_group_ns = cg.esphome_ns.namespace("homeassistant_sensor_group")

CONF_SENSORS = "sensors"

HOMEASSISTANT_SENSOR_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.use_id(HomeassistantTextSensor)
    }
)


HomeAssistantSensorGroup = homeassistant_sensor_group_ns.class_(
    'HomeAssistantSensorGroup', cg.Component
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HomeAssistantSensorGroup),
        cv.Required(CONF_SENSORS): cv.All(
            cv.ensure_list(HOMEASSISTANT_SENSOR_SCHEMA), cv.Length(min=1)
        ),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    for conf in config.get(CONF_SENSORS, []):
        newSensor = await cg.get_variable(conf[CONF_ID])
        cg.add(var.register_sensor(newSensor))
