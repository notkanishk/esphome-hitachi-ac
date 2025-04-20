#include "climate_lg2.h"

namespace esphome
{
    namespace climate_lg2
    {
        static const char *const TAG = "climate.climate_lg2";

        ClimateLG::ClimateLG(InternalGPIOPin *ir_led_pin, InternalGPIOPin *ir_recv_pin)
        {
            this->ir_led_pin = ir_led_pin;
            this->ir_recv_pin = ir_recv_pin;
            this->ir_helper = new ClimateIRHelper(this->ir_led_pin->get_pin(),this->ir_recv_pin->get_pin());
        }

        void ClimateLG::dump_config()
        {
            LOG_CLIMATE(TAG, "IR Climate", this);
            ESP_LOGCONFIG(TAG, "  Target. Temperature: %.1f°C", this->target_temperature);
            ESP_LOGCONFIG(TAG, "  Temperature Step: %.1f°C", 1.f);
            LOG_PIN("  IR LED Pin:\t ",this->ir_led_pin);
            LOG_PIN("  IR Receiver Pin:\t ",this->ir_recv_pin);
            // ESP_LOGCONFIG(TAG, "  Max. Temperature: %.1f°C", this->maximum_temperature_);
            // ESP_LOGCONFIG(TAG, "  Supports HEAT: %s", YESNO(this->supports_heat_));
            // ESP_LOGCONFIG(TAG, "  Supports COOL: %s", YESNO(this->supports_cool_));
        }

        void ClimateLG::setup()
        {
            this->target_temperature = 24.0;
            this->mode = climate::CLIMATE_MODE_COOL;
            this->action = climate::CLIMATE_ACTION_COOLING;
            this->fan_mode = climate::CLIMATE_FAN_HIGH;
            this->custom_preset = {"Power Save - 60%"};
            this->publish_state();
        }

        void ClimateLG::loop()
        {
            // Main Runtime
            this->ir_helper->runtime();
        }

        void ClimateLG::control(const climate::ClimateCall &call)
        {
            if (call.get_mode().has_value())
            {
                this->mode = *call.get_mode();
            }
            if (call.get_target_temperature().has_value())
            {
                this->target_temperature = *call.get_target_temperature();
            }
            if (call.get_target_temperature_low().has_value())
            {
                this->target_temperature_low = *call.get_target_temperature_low();
            }
            if (call.get_target_temperature_high().has_value())
            {
                this->target_temperature_high = *call.get_target_temperature_high();
            }
            if (call.get_fan_mode().has_value())
            {
                this->fan_mode = *call.get_fan_mode();
                this->custom_fan_mode.reset();
            }
            if (call.get_swing_mode().has_value())
            {
                this->swing_mode = *call.get_swing_mode();
            }
            if (call.get_custom_fan_mode().has_value())
            {
                this->custom_fan_mode = *call.get_custom_fan_mode();
                this->fan_mode.reset();
            }
            if (call.get_preset().has_value())
            {
                this->preset = *call.get_preset();
                this->custom_preset.reset();
            }
            if (call.get_custom_preset().has_value())
            {
                this->custom_preset = *call.get_custom_preset();
                this->preset.reset();
            }
            this->publish_state();
        }

        climate::ClimateTraits ClimateLG::traits()
        {
            auto traits = climate::ClimateTraits();
            traits.set_visual_min_temperature(16.0f);
            traits.set_visual_max_temperature(30.0f);
            traits.set_supports_current_temperature(false);
            traits.set_visual_temperature_step(1.0f);
            traits.set_supported_modes({
                climate::CLIMATE_MODE_OFF,
                climate::CLIMATE_MODE_COOL,
                climate::CLIMATE_MODE_AUTO,
                climate::CLIMATE_MODE_DRY,
                climate::CLIMATE_MODE_FAN_ONLY,
            });
            traits.set_supports_action(true);
            traits.set_supported_fan_modes({
                climate::CLIMATE_FAN_AUTO,
                climate::CLIMATE_FAN_LOW,
                climate::CLIMATE_FAN_MEDIUM,
                climate::CLIMATE_FAN_HIGH,
                climate::CLIMATE_FAN_QUIET,
            });
            traits.set_supported_custom_fan_modes({"Max"});
            traits.set_supported_swing_modes({climate::CLIMATE_SWING_OFF,
                                              climate::CLIMATE_SWING_VERTICAL});
            traits.set_supported_custom_presets({"Power - 80%", "Power - 60%", "Power - 40%", "Turbo"});
            return traits;
        }

    }
}