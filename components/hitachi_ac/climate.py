import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate
from esphome.const import CONF_ID

AUTO_LOAD = ["climate"]

hitachi_ac_ns = cg.esphome_ns.namespace("hitachi_ac")
HitachiACClimate = hitachi_ac_ns.class_("HitachiACClimate",climate.Climate, cg.Component)

CONFIG_SCHEMA = cv.All(climate.CLIMATE_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(HitachiACClimate),
    }
).extend(cv.COMPONENT_SCHEMA))

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)
