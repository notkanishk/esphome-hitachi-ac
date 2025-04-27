#include "lg_climate.h"
#include "esphome/core/log.h"

namespace esphome
{
  namespace lg_climate
  {

    static const char *const TAG = "lg_climate";

    void LGClimate::setup()
    {
      // Initialize the IR sender
      this->ir_send_ = new IRsend(this->transmitter_pin_->get_pin());
      this->ir_send_->begin();

      // Initialize the IR LG AC controller
      this->ac_ = new IRLgAc(this->transmitter_pin_->get_pin());
      this->ac_->begin();
      this->ac_->stateReset();
      this->ac_->setModel(lg_ac_remote_model_t::AKB75215403);

      // Setup IR receiver if pin is provided
      if (this->receiver_pin_ != nullptr)
      {
        this->ir_recv_ = new IRrecv(this->receiver_pin_->get_pin(), 1024, 50);
        this->ir_recv_->setTolerance(kTolerance);
        this->ir_recv_->enableIRIn();
      }

      // TODO: Restore config from flash

      auto restore = this->restore_state_();
      if (restore.has_value())
      {
        restore->apply(this);
      }
      else
      {
        this->target_temperature = 25;
        this->mode = climate::CLIMATE_MODE_OFF;
        this->fan_mode = climate::CLIMATE_FAN_AUTO;
        this->swing_mode = climate::CLIMATE_SWING_OFF;
      }

      if (std::isnan(this->target_temperature))
      {
        this->target_temperature = 25;
      }
    }

    void LGClimate::loop()
    {
      if (this->ir_recv_ != nullptr && this->ir_recv_->decode(&this->results_))
      {
        this->ir_recv_->disableIRIn();
        this->decode_and_update();
        this->ir_recv_->enableIRIn();
      }
    }

    void LGClimate::control(const climate::ClimateCall &call)
    {
      if (this->preset.has_value())
      {
        this->prev_preset_ = this->preset.value();
      }
      if (this->custom_preset.has_value())
      {
        this->prev_custom_preset_ = this->custom_preset.value().c_str();
      }

      // // Handle Preset Change
      // auto incomming_preset = call.get_preset().value();
      // ESP_LOGI(TAG, "Fake Error");
      // if (call.get_preset().has_value())
      // {
      //   ESP_LOGI(TAG + 'IR_RECV', "call.get_preset().value() == this->preset.value() : %s", TRUEFALSE(call.get_preset().value() != this->preset.value()));
      //   if (call.get_preset().value() != this->preset.value())
      //   {
      //     if (incomming_preset == climate::CLIMATE_PRESET_BOOST)
      //     {
      //       String hex = this->preset_hex_map[incomming_preset].substring(2);
      //       ESP_LOGI(TAG, "Settings Mode: Turbo \t Hex: %s", hex.c_str());
      //       uint64_t ir_code = strtoull(hex.c_str(), NULL, 16);
      //       this->ir_send_->sendLG2(ir_code);
      //       this->target_temperature = 15;
      //       this->fan_mode = climate::CLIMATE_FAN_AUTO;
      //       this->custom_fan_mode.reset();
      //       this->publish_state();
      //     }
      //     else
      //     {
      //       ESP_LOGI(TAG, "Unknown State");
      //     }
      //   }

      //   return;
      // }

      if (call.get_mode().has_value())
      {
        auto mode = *call.get_mode();
        this->mode = mode;

        if (this->mode == climate::CLIMATE_MODE_OFF)
        {
          this->ac_->setPower(false);
        }

        if (this->mode_ir_int_map.count(this->mode))
        {
          this->ac_->setMode(this->mode_ir_int_map[this->mode]);
        }

        if (mode != climate::CLIMATE_MODE_OFF)
        {
          this->ac_->setPower(true);
        }
      }

      if (call.get_target_temperature().has_value())
      {
        float temp = *call.get_target_temperature();
        this->target_temperature = temp;
        // LG AC uses celsius temps, converting float to int
        this->ac_->setTemp(static_cast<uint8_t>(std::round(temp)));
      }

      if (call.get_fan_mode().has_value())
      {
        auto fan_mode = *call.get_fan_mode();
        this->fan_mode = fan_mode;
        ESP_LOGD(TAG, "Fan Mode: %d\n", this->fan_mode_int_map[this->fan_mode.value()]);
        this->ac_->setFan(this->fan_mode_int_map[this->fan_mode.value()]);
        this->custom_fan_mode.reset();
      }
      if (call.get_custom_fan_mode().has_value())
      {
        this->custom_fan_mode = *call.get_custom_fan_mode();
        this->ac_->setFan(this->custom_fanmode_int_map[this->custom_fan_mode.value().c_str()]);
        this->fan_mode.reset();
      }

      if (call.get_swing_mode().has_value())
      {
        auto swing_mode = *call.get_swing_mode();
        this->swing_mode = swing_mode;

        switch (swing_mode)
        {
        case climate::CLIMATE_SWING_VERTICAL:
          this->ac_->setSwingV(true);
          break;
        case climate::CLIMATE_SWING_OFF:
        default:
          this->ac_->setSwingV(false);
          break;
        }
      }

      if (call.get_preset().has_value())
      {
        this->custom_preset.reset();
        this->preset = *call.get_preset();
      }

      if (call.get_custom_preset().has_value())
      {
        this->preset.reset();
        this->custom_preset = *call.get_custom_preset();
      }

      // Send the IR command
      this->ir_recv_->disableIRIn();
      this->ac_->send();
      this->ir_recv_->enableIRIn();

      ESP_LOGD(TAG, "LG AC sending command: mode=%d, temp=%.1f, fan=%d",
               static_cast<int>(this->mode), this->target_temperature, static_cast<int>(this->fan_mode.value()));

      this->publish_state();
    }

    climate::ClimateTraits LGClimate::traits()
    {
      auto traits = climate::ClimateTraits();
      traits.set_supports_current_temperature(false);
      traits.set_supported_modes({
          climate::CLIMATE_MODE_OFF,
          climate::CLIMATE_MODE_AUTO,
          climate::CLIMATE_MODE_COOL,
          climate::CLIMATE_MODE_FAN_ONLY,
          climate::CLIMATE_MODE_DRY,
      });
      traits.set_supports_two_point_target_temperature(false);
      traits.set_visual_min_temperature(16);
      traits.set_visual_max_temperature(30);
      traits.set_visual_temperature_step(1.0f);
      traits.set_supported_fan_modes({
          climate::CLIMATE_FAN_AUTO,
          climate::CLIMATE_FAN_LOW,
          climate::CLIMATE_FAN_MEDIUM,
          climate::CLIMATE_FAN_HIGH,
          climate::CLIMATE_FAN_QUIET,
      });
      traits.set_supported_presets({climate::CLIMATE_PRESET_BOOST});
      traits.set_supported_swing_modes({climate::CLIMATE_SWING_OFF,
                                        climate::CLIMATE_SWING_VERTICAL});
      traits.set_supported_custom_presets({"Power - 100%", "Power - 80%", "Power - 60%", "Power - 40%"});
      traits.set_supported_custom_fan_modes({"Max"});
      return traits;
    }

    void LGClimate::decode_and_update()
    {

      // Check if the decoded result is from an LG AC
      if (this->results_.decode_type == decode_type_t::LG ||
          this->results_.decode_type == decode_type_t::LG2)
      {

        uint32_t backup_hex = this->ac_->getRaw();
        this->ac_->setRaw(this->results_.value, this->results_.decode_type);

        String hex = resultToHexidecimal(&this->results_);
        ESP_LOGD(TAG, "Received LG AC command \t Hex: %s", hex.c_str());

        // check for presets
        if (this->hex_preset_map.count(hex))
        {
          if (this->hex_preset_map[hex] == climate::CLIMATE_PRESET_BOOST)
            this->custom_preset.reset();
          this->preset = this->hex_preset_map[hex];
          this->target_temperature = 15;
          this->fan_mode = climate::CLIMATE_FAN_AUTO;
          this->mode = climate::CLIMATE_MODE_COOL;
          // Restore the backed up hex in the internal ac state;
          this->ac_->setRaw(backup_hex);
          this->publish_state();
          return;
        }

        this->preset.reset();
        this->custom_preset.reset();

        // Update local state based on received command
        if (this->ac_->getPower())
        {
          uint8_t lg_mode = this->ac_->getMode();

          this->mode = climate::CLIMATE_MODE_AUTO;

          if (this->mode_int_ir_map.count(lg_mode))
          {
            this->mode = this->mode_int_ir_map[lg_mode];
          }
        }
        else
        {
          this->mode = climate::CLIMATE_MODE_OFF;
        }

        this->target_temperature = this->ac_->getTemp();

        uint8_t lg_fan = this->ac_->getFan();

        if (this->fan_int_mode_map.count(lg_fan))
        {
          this->fan_mode = this->fan_int_mode_map[lg_fan];
          this->custom_fan_mode.reset();
        }
        else if (this->custom_int_fanmode_map.count(lg_fan))
        {
          this->custom_fan_mode = esphome::make_optional(this->custom_int_fanmode_map[lg_fan].c_str());
          this->fan_mode.reset();
        }
        else
        {
          this->fan_mode = climate::CLIMATE_FAN_AUTO;
          this->custom_fan_mode.reset();
        }

        if (this->ac_->getSwingV())
        {
          this->swing_mode = climate::CLIMATE_SWING_VERTICAL;
        }
        else
        {
          this->swing_mode = climate::CLIMATE_SWING_OFF;
        }

        // Update the ESPHome climate state
        this->publish_state();
      }
    }

  } // namespace lg_climate
} // namespace esphome