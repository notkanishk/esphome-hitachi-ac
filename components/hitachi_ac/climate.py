import esphome.config_validation as cv
from esphome.components import climate
from esphome.const import (
    CONF_ID,
    CONF_NAME,
    CLIMATE_MODE_AUTO,
    CLIMATE_MODE_COOL,
    CLIMATE_MODE_HEAT,
    CLIMATE_MODE_OFF,
)
import voluptuous as vol

# Define the configuration schema for the standard climate component.
# We extend the base CLIMATE_SCHEMA and add any component-specific parameters.
CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(),
    vol.Optional(CONF_NAME, default="IR AC"): cv.string,
    # Example of a custom parameter. For instance, specifying an initial or default target temperature.
    vol.Optional("default_temperature", default=24.0): cv.temperature,
}).extend(cv.COMPONENT_SCHEMA)

def to_code(config):
    # Create the C++ instance of our IR AC climate component.
    # Here we assume you have a constructor function that creates a new HitachiACClimate instance.
    var = yield climate.new_climate_device(config[CONF_ID], config[CONF_NAME])
    # Set the default temperature if provided.
    if "default_temperature" in config:
        temp = config["default_temperature"]
        # This setter function must be implemented in your C++ class (HitachiACClimate).
        var.set_default_temperature(temp)
    # Register the component so it becomes part of the ESPHome configuration.
    yield climate.register_climate_device(var, config)