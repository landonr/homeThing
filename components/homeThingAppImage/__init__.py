import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components import homeThingApp, sensor, text_sensor, image

AUTO_LOAD = ["homeThingApp"]
homething_app_image_ns = cg.esphome_ns.namespace("homething_app_image")

HomeThingAppImage = homething_app_image_ns.class_("HomeThingAppImage", cg.Component)

CONF_CAMERA_IMAGE = "camera_image"

CONFIG_SCHEMA = homeThingApp.BASE_SCHEMA.extend(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(HomeThingAppImage),
        cv.Required(CONF_CAMERA_IMAGE): cv.use_id(image.Image_),
    }
)

async def to_code(config):
    cg.add_build_flag("-DUSE_IMAGE")
    var = await homeThingApp.new_app_base(config)
    await cg.register_component(var, config)

    cg.add(var.set_camera_image(await cg.get_variable(config[CONF_CAMERA_IMAGE])))