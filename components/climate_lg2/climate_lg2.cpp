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
        }

        void ClimateLG::dump_config()
        {
            LOG_CLIMATE(TAG, "IR Climate", this);
            ESP_LOGCONFIG(TAG, "  Target. Temperature: %.1f°C", this->target_temperature);
            ESP_LOGCONFIG(TAG, "  Temperature Step: %.1f°C", 1.f);
            LOG_PIN("  IR LED Pin:\t ", this->ir_led_pin);
            LOG_PIN("  IR Receiver Pin:\t ", this->ir_recv_pin);
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
            this->swing_mode = climate::CLIMATE_SWING_HORIZONTAL;
            this->custom_preset = {"Power - 100%"};
            this->publish_state();

            this->ac = new IRLgAc(this->ir_led_pin->get_pin());
            this->ac->setModel(lg_ac_remote_model_t::AKB75215403);
            this->ac->begin();

            this->ir_results = new decode_results;
            this->ir_recv = new IRrecv(this->ir_recv_pin->get_pin(), 1024, 50, true);
            this->ir_recv->setTolerance(kTolerance);
            this->ir_recv->setUnknownThreshold(12);
            this->ir_recv->enableIRIn();
            ESP_LOGD(TAG, "Low Level Sanity Check: %d", irutils::lowLevelSanityCheck());
        }

        void ClimateLG::onReceive(decode_results *results)
        {

            String hex = resultToHexidecimal(results);
            
            // Ensure the Encoding is LG2
            if (results->decode_type != decode_type_t::LG2)
            {
                return;
            }
            if (!this->ac->isValidLgAc())
            {
                return;
            }
            this->ac->setRaw(results->value, results->decode_type);
            
            if (this->ac->getPower() == 0)
            {
                ESP_LOGD(TAG,"isPowerOff %d",this->ac->isOffCommand());

                if (this->ac->isOffCommand())
                {
                    ESP_LOGD(TAG, "Power Off State");
                    this->mode = climate::CLIMATE_MODE_OFF;
                    this->fan_mode = climate::CLIMATE_FAN_OFF;
                    this->custom_fan_mode.reset();
                    this->custom_preset.reset();

                    // Restore the backup
                    this->publish_state();
                    return;
                }

                // Process Convertible Modes
                if (this->power_mode_hex_str_map.count(hex.c_str()))
                {
                    this->custom_preset = esphome::make_optional(this->power_mode_hex_str_map[hex.c_str()].c_str());
                    this->preset.reset();
                    
                    // Restore the backup
                    this->publish_state();
                    return;
                }
            }

            // Process  Turbo
            if (this->special_mode_map.count(hex))
            {
                if (special_mode_map[hex] == "Turbo")
                {
                    this->custom_preset = esphome::make_optional(this->special_mode_map[hex.c_str()].c_str());
                    this->preset.reset();
                }
            }

            // Process Mode, Temperature, Fan
            this->ac->setRaw(this->ir_results->value, this->ir_results->decode_type);

            // Temp
            this->target_temperature = this->ac->getTemp();

            // Mode
            this->mode = this->mode_vec[this->ac->getMode()];

            // Fan
            uint32_t fan = this->ac->getFan();

            if (this->int_fan_map.count(fan))
            {
                this->fan_mode = this->int_fan_map[fan];
                this->custom_fan_mode.reset();
            }
            else if (fan == 4)
            {
                this->set_custom_fan_mode_("Max");
                this->fan_mode.reset();
            }
            else
            {
                ESP_LOGE(TAG, "  Invalid Fan Mode Received: %d", fan);
            }
            this->publish_state();
        }


        void ClimateLG::transmit()
        {
            ESP_LOGD(TAG,"ir transmit called()");





            // Power
            // this->ac->stateReset();
            this->ac->setPower(this->mode != climate::CLIMATE_MODE_OFF);

            if (this->mode == climate::CLIMATE_MODE_OFF) {
                this->ac->send();
                return;
            }

            // TODO: Power Modes


            // TODO: Swing
            
            // TODO: Special Modes - Turbo

            // Mode
            this->ac->setMode(this->mode_ir_mode_map[this->mode]);

            // Temp
            this->ac->setTemp(std::floor(this->target_temperature));

            // TODO: Fan
            if (this->fan_mode.has_value()) {
                this->ac->setFan(fan_int_map[this->fan_mode.value()]);
            }

            if (this->custom_fan_mode.has_value()) {
                this->ac->setFan(this->custom_fan_int_map[this->custom_fan_mode.value().c_str()]);
            }

            // Send
            this->ac->send(0);
        }

        void ClimateLG::loop()
        {
            // Main Runtime
            if (this->ir_recv->decode(this->ir_results))
            {
                if (this->ir_results->overflow)
                {
                    ESP_LOGE(TAG, "Buffer Flow: %s", YESNO(true));
                    return;
                }

                this->onReceive(this->ir_results);
            }
        }

        void ClimateLG::control(const climate::ClimateCall &call)
        {

            // Save a copy of the current state before you update
            this->prev_state = {
                mode: this->mode;
            };


            if (call.get_mode().has_value())
            {
                this->mode = *call.get_mode();
            }
            if (call.get_target_temperature().has_value())
            {
                this->target_temperature = *call.get_target_temperature();
            }
            // if (call.get_target_temperature_low().has_value())
            // {
            //     this->target_temperature_low = *call.get_target_temperature_low();
            // }
            // if (call.get_target_temperature_high().has_value())
            // {
            //     this->target_temperature_high = *call.get_target_temperature_high();
            // }
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
            this->transmit();
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
            traits.set_supported_fan_modes({climate::CLIMATE_FAN_AUTO,
                                            climate::CLIMATE_FAN_LOW,
                                            climate::CLIMATE_FAN_MEDIUM,
                                            climate::CLIMATE_FAN_HIGH,
                                            climate::CLIMATE_FAN_QUIET,
                                            climate::CLIMATE_FAN_OFF});
            traits.set_supported_custom_fan_modes({"Max"});
            traits.set_supported_swing_modes({climate::CLIMATE_SWING_OFF,
                                              climate::CLIMATE_SWING_VERTICAL});
            traits.set_supported_custom_presets({"Power - 100%", "Power - 80%", "Power - 60%", "Power - 40%", "Turbo"});
            return traits;
        }

    }
}