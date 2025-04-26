import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate
from esphome.const import (
    CONF_ID,
)
from esphome import pins

DEPENDENCIES = ["esp32"]

voltas_climate_ns = cg.esphome_ns.namespace("voltas_climate")
VoltasClimate = voltas_climate_ns.class_("VoltasClimate", climate.Climate, cg.Component)

CONF_TRANSMITTER_PIN = "transmitter_pin"
CONF_RECEIVER_PIN = "receiver_pin"

CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(VoltasClimate),
    cv.Required(CONF_TRANSMITTER_PIN): pins.gpio_output_pin_schema,
    cv.Optional(CONF_RECEIVER_PIN): pins.gpio_input_pin_schema,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)

    transmitter_pin = await cg.gpio_pin_expression(config[CONF_TRANSMITTER_PIN])
    cg.add(var.set_transmitter_pin(transmitter_pin))

    if CONF_RECEIVER_PIN in config:
        receiver_pin = await cg.gpio_pin_expression(config[CONF_RECEIVER_PIN])
        cg.add(var.set_receiver_pin(receiver_pin))

    # Add IRremoteESP8266 library dependency
    cg.add_library("crankyoldgit/IRremoteESP8266", "2.8.6")