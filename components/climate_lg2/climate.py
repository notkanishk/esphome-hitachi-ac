import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate
from esphome.const import (
    CONF_ID,
    CONF_PIN,
)

DEPENDENCIES = ["esp8266", "esp32"]

lg_climate_ns = cg.esphome_ns.namespace("lg_climate")
LGClimate = lg_climate_ns.class_("LGClimate", climate.Climate, cg.Component)

CONF_TRANSMITTER_PIN = "transmitter_pin"
CONF_RECEIVER_PIN = "receiver_pin"

CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(LGClimate),
    cv.Required(CONF_TRANSMITTER_PIN): cv.gpio_output_pin_schema,
    cv.Optional(CONF_RECEIVER_PIN): cv.gpio_input_pin_schema,
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