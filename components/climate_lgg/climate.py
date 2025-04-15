import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate
from esphome.const import (CONF_ID)

AUTO_LOAD = ["climate"]

climate_lgg_ns = cg.esphome_ns.namespace("climate_lgg")
ClimateLGG = climate_lgg_ns.class_("ClimateLGG",climate.Climate)

CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend(
    {
    cv.GenerateID(): cv.declare_id(ClimateLGG),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await climate.register_climate(var,config)