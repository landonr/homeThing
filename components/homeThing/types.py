import esphome.codegen as cg
from esphome import automation

# Base
homething_menu_base_ns = cg.esphome_ns.namespace("homething_menu_base")

HomeThingMenuBase = homething_menu_base_ns.class_("HomeThingMenuBase", cg.PollingComponent)
HomeThingMenuScreen = homething_menu_base_ns.class_("HomeThingMenuScreen")
MenuCommand = homething_menu_base_ns.class_("MenuCommand")
HomeThingMenuBoot = homething_menu_base_ns.class_("HomeThingMenuBoot")
HomeThingMenuSettings = homething_menu_base_ns.class_("HomeThingMenuSettings")
HomeThingMenuAnimation = homething_menu_base_ns.class_("HomeThingMenuAnimation")
HomeThingMenuDisplay = homething_menu_base_ns.class_("HomeThingMenuDisplay")
HomeThingMenuHeader = homething_menu_base_ns.class_("HomeThingMenuHeader")
HomeThingMenuRefactor = homething_menu_base_ns.class_("HomeThingMenuRefactor")

HomeThingMenuBaseConstPtr = HomeThingMenuBase.operator("ptr").operator("const")

homething_app_ns = cg.esphome_ns.namespace("homething_menu_app")

# Automation Actions

UpAction = homething_menu_base_ns.class_("UpAction", automation.Action)
DownAction = homething_menu_base_ns.class_("DownAction", automation.Action)
LeftAction = homething_menu_base_ns.class_("LeftAction", automation.Action)
RightAction = homething_menu_base_ns.class_("RightAction", automation.Action)
SelectAction = homething_menu_base_ns.class_("SelectAction", automation.Action)
ScrollClockwiseAction = homething_menu_base_ns.class_("ScrollClockwiseAction", automation.Action)
ScrollCounterClockwiseAction = homething_menu_base_ns.class_("ScrollCounterClockwiseAction", automation.Action)

BackAction = homething_menu_base_ns.class_("BackAction", automation.Action)
OptionAction = homething_menu_base_ns.class_("OptionAction", automation.Action)
HomeAction = homething_menu_base_ns.class_("HomeAction", automation.Action)

AddNotificationAction = homething_menu_base_ns.class_("AddNotificationAction", automation.Action)
ClearNotificationsAction = homething_menu_base_ns.class_("ClearNotificationsAction", automation.Action)

# Triggers
HomeThingDisplayMenuOnRedrawTrigger = homething_menu_base_ns.class_("HomeThingDisplayMenuOnRedrawTrigger", automation.Trigger)