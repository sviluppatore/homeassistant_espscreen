"""Experimental DIS08070H V3.0 touch initialization from Elecrow's example."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c
from esphome.const import CONF_ID

DEPENDENCIES = ["i2c"]
ns = cg.esphome_ns.namespace("crowpanel_v3")
CrowPanelV3 = ns.class_("CrowPanelV3", cg.Component, i2c.I2CDevice)
CONF_RESET_TOUCH = "reset_touch"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(CrowPanelV3),
    cv.Optional(CONF_RESET_TOUCH, default=True): cv.boolean,
}).extend(cv.COMPONENT_SCHEMA).extend(i2c.i2c_device_schema(0x18))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    cg.add(var.set_reset_touch(config[CONF_RESET_TOUCH]))
