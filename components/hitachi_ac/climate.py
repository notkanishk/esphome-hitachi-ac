import esphome.codegen as cg
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

AUTO_LOAD = ["climate"]

hitachi_ac_ns = cg.esphome_ns.namespace("hitachi_ac")
HitachiAC = hitachi_ac_ns.class_("HitachiACClimate",climate.Climate)

CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend(
    {
    cv.GenerateID(): cv.declare_id(HitachiAC),
}
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await climate.register_climate(var,config)