import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import homeThingApp, sensor, text_sensor, image

AUTO_LOAD = ["homeThingApp"]
homething_app_weather_ns = cg.esphome_ns.namespace("homething_app_weather")

HomeThingAppWeather = homething_app_weather_ns.class_("HomeThingAppWeather", cg.Component)

CONF_TEMPERATURE_SENSOR = "temperature_sensor"
CONF_HUMIDITY_SENSOR = "humidity_sensor"
CONF_CONDITION_SENSOR = "condition_sensor"
CONF_CLEAR = "clear"
CONF_CLOUDY = "cloudy"
CONF_PARTLY_CLOUDY = "partly_cloudy"
CONF_FOG = "fog"
CONF_HAIL = "hail"
CONF_LIGHTNING = "lightning"
CONF_RAINY = "rainy"
CONF_SNOW = "snow"
CONF_SUNNY = "sunny"
CONF_WINDY = "windy"
CONF_IMAGES = "images"

IMAGE_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_CLEAR): cv.use_id(image.Image_),
        cv.Required(CONF_CLOUDY): cv.use_id(image.Image_),
        cv.Required(CONF_FOG): cv.use_id(image.Image_),
        cv.Required(CONF_HAIL): cv.use_id(image.Image_),
        cv.Required(CONF_RAINY): cv.use_id(image.Image_),
        cv.Required(CONF_SNOW): cv.use_id(image.Image_),
        cv.Required(CONF_SUNNY): cv.use_id(image.Image_),
        cv.Required(CONF_WINDY): cv.use_id(image.Image_),
        cv.Required(CONF_PARTLY_CLOUDY): cv.use_id(image.Image_),
        cv.Required(CONF_LIGHTNING): cv.use_id(image.Image_),
    }
)


CONFIG_SCHEMA = homeThingApp.BASE_SCHEMA.extend(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(HomeThingAppWeather),
        cv.Required(CONF_TEMPERATURE_SENSOR): cv.use_id(sensor.Sensor),
        cv.Required(CONF_HUMIDITY_SENSOR): cv.use_id(sensor.Sensor),
        cv.Required(CONF_CONDITION_SENSOR): cv.use_id(text_sensor.TextSensor),
        cv.Required(CONF_IMAGES): IMAGE_SCHEMA,
    }
)

async def to_code(config):
    cg.add_build_flag("-DUSE_IMAGE")
    var = await homeThingApp.new_app_base(config)
    await cg.register_component(var, config)

    temperature_sensor = await cg.get_variable(config[CONF_TEMPERATURE_SENSOR])
    humidity_sensor = await cg.get_variable(config[CONF_HUMIDITY_SENSOR])
    condition_sensor = await cg.get_variable(config[CONF_CONDITION_SENSOR])
    cg.add(var.set_temperature_sensor(temperature_sensor))
    cg.add(var.set_humidity_sensor(humidity_sensor))
    cg.add(var.set_condition_sensor(condition_sensor))

    images = config[CONF_IMAGES]
    cg.add(var.set_clear_image(await cg.get_variable(images[CONF_CLEAR])))
    cg.add(var.set_cloudy_image(await cg.get_variable(images[CONF_CLOUDY])))
    cg.add(var.set_fog_image(await cg.get_variable(images[CONF_FOG])))
    cg.add(var.set_hail_image(await cg.get_variable(images[CONF_HAIL])))
    cg.add(var.set_rainy_image(await cg.get_variable(images[CONF_RAINY])))
    cg.add(var.set_snow_image(await cg.get_variable(images[CONF_SNOW])))
    cg.add(var.set_sunny_image(await cg.get_variable(images[CONF_SUNNY])))
    cg.add(var.set_windy_image(await cg.get_variable(images[CONF_WINDY])))
    cg.add(var.set_partly_cloudy_image(await cg.get_variable(images[CONF_PARTLY_CLOUDY])))
    cg.add(var.set_lightning_image(await cg.get_variable(images[CONF_LIGHTNING])))