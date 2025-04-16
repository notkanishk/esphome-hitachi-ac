#include "climate_lgg.h"

namespace esphome
{
  namespace climate_lgg
  {

    static const char *const TAG = "climate_lgg";

    ClimateLGG::ClimateLGG(InternalGPIOPin *ir_led_pin, InternalGPIOPin *ir_recv_pin)
    {
      this->_ir_led_pin = ir_led_pin;
      this->_ir_recv_pin = ir_recv_pin;
      
    }

    void ClimateLGG::dump_config()
    {
      ESP_LOGCONFIG(TAG, "Climate LGG Configuration: \n");
      LOG_PIN("__IR_LED_PIN__", this->_ir_led_pin);
      LOG_PIN("__IR_RECV_PIN__", this->_ir_recv_pin);
    }

    void ClimateLGG::setup()
    {
    }

    void ClimateLGG::loop()
    {
      // Check if an IR packet has been received.
    }

    void ClimateLGG::control(const climate::ClimateCall &call)
    {
      // Publish the new state to Home Assistant.
      publish_state();
    }

    climate::ClimateTraits ClimateLGG::traits()
    {
      auto traits = climate::ClimateTraits();
      traits.set_visual_min_temperature(this->min_temp);
      traits.set_visual_max_temperature(this->max_temp);
      traits.set_visual_target_temperature_step(1.0f);
      
      traits.set_supports_current_temperature(false); // No current temperature sensor.
      traits.set_supported_modes({climate::CLIMATE_MODE_OFF,
                                  climate::CLIMATE_MODE_COOL,
                                  climate::CLIMATE_MODE_DRY,
                                  climate::CLIMATE_MODE_FAN_ONLY,
                                  climate::CLIMATE_MODE_AUTO});
      // Optional: set fan modes if applicable.
      traits.set_supports_two_point_target_temperature(false);
      traits.set_supported_swing_modes({climate::CLIMATE_SWING_HORIZONTAL,
                                        climate::CLIMATE_SWING_OFF});
      traits.set_supported_custom_fan_modes({
        "Quiet", "Low","Medium","High","Maximum","Auto"
      });
      traits.set_supported_custom_presets({
        "High Capacity - Turbo","Energy Save - 80%", "Energy Save - 60%","Energy Save - 40%"
      });
      return traits;
    }

  } // namespace hitachi_ac
} // namespace esphome