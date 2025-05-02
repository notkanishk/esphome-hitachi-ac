"""Hitachi AC component for ESPHome."""
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate
from esphome import pins
from esphome.const import (
    CONF_ID,
    CONF_NAME,
)

DEPENDENCIES = ["climate"]
AUTO_LOAD = ["climate"]

# Import and register external library dependencies
CODEOWNERS = ["@username"]
REQUIRES_ARDUINO = True

# Additional configuration options
CONF_TRANSMITTER_PIN = "transmitter_pin"
CONF_RECEIVER_PIN = "receiver_pin"
CONF_SUPPORTS_SWING_MODES = "supports_swing_modes"

# Define namespace and component class
hitachi_ac_ns = cg.esphome_ns.namespace("hitachi_ac")
HitachiAC = hitachi_ac_ns.class_("HitachiAC", climate.Climate, cg.Component)

# Configuration schema
CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(HitachiAC),
        cv.Required(CONF_NAME): cv.string,
        cv.Required(CONF_TRANSMITTER_PIN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_RECEIVER_PIN): pins.gpio_input_pin_schema,
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    """Code generation for the Hitachi AC component."""
    var = cg.new_Pvariable(config[CONF_ID], config[CONF_NAME])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)

    # Add component-specific dependencies
    cg.add_library("crankyoldgit/IRremoteESP8266", "2.8.4")

    # Set pin configurations
    transmitter_pin = await cg.gpio_pin_expression(config[CONF_TRANSMITTER_PIN])
    cg.add(var.set_transmitter_pin(transmitter_pin))
    receiver_pin = await cg.gpio_pin_expression(config[CONF_RECEIVER_PIN])
    cg.add(var.set_receiver_pin(receiver_pin))