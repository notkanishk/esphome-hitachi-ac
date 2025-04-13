import esphome.codegen as cg
from esphome.components import climate
import esphome.config_validation as cv
from esphome.const import (CONF_ID)


climate_lg2_ns = cg.esphome_ns.namespace("climate_lg2")

ClimateIR = climate_lg2_ns.class_(
    "ClimateLG2",
    climate.Climate,
)

CLIMATE_LG2_SCHEMA = (
    climate.CLIMATE_SCHEMA.extend(
        {
            cv.GenerateID: cv.declare_id(ClimateIR)
        }
    )
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await climate.register_climate(var, config)