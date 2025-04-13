import esphome.codegen as cg
from esphome.components import climate, remote_base, sensor
import esphome.config_validation as cv
from esphome.const import CONF_SENSOR, CONF_SUPPORTS_COOL, CONF_SUPPORTS_HEAT


climate_ir_ns = cg.esphome_ns.namespace("climate_ir")
ClimateIR = climate_ir_ns.class_(
    "ClimateLG2",
    climate.Climate,
    cg.Component
)

CLIMATE_IR_SCHEMA = (
    climate.CLIMATE_SCHEMA.extend(
        {
            cv.GenerateID: cv.declare_id(ClimateIR)
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)



async def register_climate_ir(var, config):
    await cg.register_component(var, config)
    await climate.register_climate(var, config)