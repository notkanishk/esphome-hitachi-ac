import esphome.codegen as cg
from esphome.components import climate
import esphome.config_validation as cv
from esphome.const import (CONF_ID)

AUTO_LOAD = ["climate"]

climate_lg2_ns = cg.esphome_ns.namespace("climate_lg2")
ClimateLG2 = climate_lg2_ns.class_("ClimateLG2",climate.Climate)

CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend(
    {
        cv.GenerateID: cv.declare_id(ClimateLG2)
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await climate.register_climate(var, config)