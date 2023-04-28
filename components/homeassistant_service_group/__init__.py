import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.const import CONF_ID, CONF_TRIGGER_ID

homeassistant_service_group_ns = cg.esphome_ns.namespace("homeassistant_service_group")

HomeAssistantServiceGroup = homeassistant_service_group_ns.class_(
    'HomeAssistantServiceGroup', cg.Component
)

HomeAssistantServiceCommand = homeassistant_service_group_ns.class_(
    'HomeAssistantServiceCommand', cg.Component
)

CONF_COMMANDS = "commands"
CONF_COMMAND = "command"
CONF_TEXT = "text"

ServiceCalledTrigger = homeassistant_service_group_ns.class_(
    "ServiceCalledTrigger", automation.Trigger.template()
)

SERVICE_LIST_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ID): cv.declare_id(HomeAssistantServiceCommand),
        cv.Required(CONF_COMMAND): automation.validate_automation(
            {
                cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(
                    ServiceCalledTrigger
                ),
            }
        ),
        cv.Required(CONF_TEXT): cv.string,
    }
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(HomeAssistantServiceGroup),
        cv.Required(CONF_COMMANDS): cv.All(
            cv.ensure_list(SERVICE_LIST_SCHEMA), cv.Length(min=1)
        ),
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    for conf in config[CONF_COMMANDS]:
        service = cg.new_Pvariable(conf[CONF_ID])
        cg.add(service.set_text(conf[CONF_TEXT]))
        
        for command in conf.get(CONF_COMMAND, []):
            trigger = cg.new_Pvariable(command[CONF_TRIGGER_ID], service)
            await automation.build_automation(trigger, [], command)
        cg.add(var.register_service_call(service))
