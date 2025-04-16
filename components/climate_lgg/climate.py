import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate
from esphome.const import (CONF_ID)
from esphome import pins

AUTO_LOAD = ["climate"]

CONF_IR_LED_PIN = "ir_led_pin"
CONF_IR_RECV_PIN = "ir_recv_pin"
CONF_MIN_TEMP = "min_temp"
CONF_MAX_TEMP = "max_temp"

climate_lgg_ns = cg.esphome_ns.namespace("climate_lgg")
ClimateLGG = climate_lgg_ns.class_("ClimateLGG",climate.Climate, cg.Component)


CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend(cv.COMPONENT_SCHEMA).extend(
    {
        cv.GenerateID(): cv.declare_id(ClimateLGG),
        cv.Required(CONF_IR_LED_PIN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_IR_RECV_PIN): pins.gpio_input_pin_schema,
        cv.Optional(CONF_MIN_TEMP, default=16): cv.positive_float,
        cv.Optional(CONF_MAX_TEMP, default= 30): cv.positive_float
    }
)



async def to_code(config):
    ir_led_pin = await cg.gpio_pin_expression(config[CONF_IR_LED_PIN])
    ir_recv_pin = await cg.gpio_pin_expression(config[CONF_IR_RECV_PIN])
    
    var = cg.new_Pvariable(config[CONF_ID],ir_led_pin,ir_recv_pin)
    await climate.register_climate(var,config)

    cg.add(var.set_min_temp(config[CONF_MIN_TEMP]))
    cg.add(var.set_max_temp(config[CONF_MAX_TEMP]))