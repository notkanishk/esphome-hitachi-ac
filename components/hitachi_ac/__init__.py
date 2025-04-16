# components/hitachi_ac/__init__.py

from esphome import core as esphome_core
from .climate import CONFIG_SCHEMA as CLIMATE_AC_SCHEMA, to_code as climate_ac_to_code

__version__ = "1.0.0"

# When the component is included (via the YAML configuration), this file ensures that
# the climate type "hitachi_ac" is registered.
def to_code(config):
    # Delegate code generation to climate.py
    yield from climate_ac_to_code(config)