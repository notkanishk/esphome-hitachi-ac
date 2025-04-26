#include "lloyd_climate.h"
#include "esphome/core/log.h"

namespace esphome
{
  namespace voltas_climate
  {

    static const char *const TAG = "voltas_climate";

    void VoltasClimate::setup()
    {
      // Initialize the IR sender
      this->ir_send_ = new IRsend(this->transmitter_pin_->get_pin());
      this->ir_send_->begin();

      // Initialize the IR LG AC controller
      this->ac_ = new IRVoltas(this->transmitter_pin_->get_pin());
      this->ac_->begin();
      this->ac_->stateReset();
      this->ac_->setModel(voltas_ac_remote_model_t::kVoltasUnknown);

      // Setup IR receiver if pin is provided
      if (this->receiver_pin_ != nullptr)
      {
        this->ir_recv_ = new IRrecv(this->receiver_pin_->get_pin());
        this->ir_recv_->enableIRIn();
      }

      // Set initial target temperature to 25°C
      this->target_temperature = 24;
      this->mode = climate::CLIMATE_MODE_OFF;
      this->fan_mode = climate::CLIMATE_FAN_AUTO;
      this->swing_mode = climate::CLIMATE_SWING_OFF;
    }

    void VoltasClimate::loop()
    {
      if (this->ir_recv_ != nullptr && this->ir_recv_->decode(&this->results_))
      {
        this->decode_and_update();
        this->ir_recv_->resume();
      }
    }

    void VoltasClimate::control(const climate::ClimateCall &call)
    {
      if (call.get_mode().has_value())
      {
        auto mode = *call.get_mode();
        this->mode = mode;

        switch (mode)
        {
        case climate::CLIMATE_MODE_COOL:
          this->ac_->setMode(kVoltasCool);
          break;
        case climate::CLIMATE_MODE_HEAT:
          this->ac_->setMode(kVoltasHeat);
          break;
        case climate::CLIMATE_MODE_DRY:
          this->ac_->setMode(kVoltasDry);
          break;
        case climate::CLIMATE_MODE_FAN_ONLY:
          this->ac_->setMode(kVoltasFan);
          break;
        case climate::CLIMATE_MODE_AUTO:
          this->ac_->setMode(kVoltasFanAuto);
          break;
        case climate::CLIMATE_MODE_OFF:
        default:
          this->ac_->setPower(false);
          break;
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
        this->ac_->setFan(this->fan_mode_int_map[this->fan_mode.value()]);
        this->custom_fan_mode.reset();
        // switch (fan_mode)
        // {
        // case climate::CLIMATE_FAN_LOW:
        //   this->ac_->setFan(kLgAcFanLow);
        //   break;
        // case climate::CLIMATE_FAN_MEDIUM:
        //   this->ac_->setFan(kLgAcFanMedium);
        //   break;
        // case climate::CLIMATE_FAN_HIGH:
        //   this->ac_->setFan(kLgAcFanHigh);
        //   break;
        // case climate::CLIMATE_FAN_AUTO:
        // default:
        //   this->ac_->setFan(kLgAcFanAuto);
        //   break;
        // }
      }
      if (call.get_custom_fan_mode().has_value())
      {
        this->custom_fan_mode = *call.get_custom_fan_mode();
        this->ac_->setFan(this->custom_fan_mode_int_map[this->custom_fan_mode.value().c_str()]);
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

      // Send the IR command
      this->ac_->send();

      ESP_LOGD(TAG, "Voltas AC sending command: mode=%d, temp=%.1f, fan=%d",
               static_cast<int>(this->mode), this->target_temperature, static_cast<int>(this->fan_mode.value()));

      this->publish_state();
    }

    climate::ClimateTraits VoltasClimate::traits()
    {
      auto traits = climate::ClimateTraits();
      traits.set_supports_current_temperature(false);
      traits.set_supports_auto_mode(true);
      traits.set_supports_cool_mode(true);
      traits.set_supports_heat_mode(false);
      traits.set_supports_fan_only_mode(true);
      traits.set_supports_dry_mode(true);
      // traits.set_supports_fan_mode(true);
      // traits.set_supports_swing_mode(true);

      traits.set_supports_two_point_target_temperature(false);
      traits.set_visual_min_temperature(16);
      traits.set_visual_max_temperature(30);
      traits.set_visual_temperature_step(1.0f);

      traits.set_supported_fan_modes({climate::CLIMATE_FAN_AUTO,
                                      climate::CLIMATE_FAN_LOW,
                                      climate::CLIMATE_FAN_MEDIUM,
                                      climate::CLIMATE_FAN_HIGH,
                                      climate::CLIMATE_FAN_QUIET,
                                      climate::CLIMATE_FAN_OFF});
      traits.set_supported_swing_modes({climate::CLIMATE_SWING_OFF,
                                        climate::CLIMATE_SWING_VERTICAL});

      traits.set_supported_custom_presets({"Power - 100%", "Power - 80%", "Power - 60%", "Power - 40%", "Turbo"});

      return traits;
    }

    void VoltasClimate::decode_and_update()
    {

      
      // Check if the decoded result is from an LG AC
      if (this->results_.decode_type == decode_type_t::LG ||
        this->results_.decode_type == decode_type_t::LG2)
        {
          
        // this->ac_->setRaw(this->results_.value,this->results_.decode_type);

        ESP_LOGD(TAG, "Received Voltas AC command");

        // Update local state based on received command
        if (this->ac_->getPower())
        {
          uint8_t lg_mode = this->ac_->getMode();
          switch (lg_mode)
          {
          case kVoltasCool:
            this->mode = climate::CLIMATE_MODE_COOL;
            break;
          case kVoltasHeat:
            this->mode = climate::CLIMATE_MODE_HEAT;
            break;
          case kVoltasDry:
            this->mode = climate::CLIMATE_MODE_DRY;
            break;
          case kVoltasFan:
            this->mode = climate::CLIMATE_MODE_FAN_ONLY;
            break;
          case kVoltasFanAuto:
            this->mode = climate::CLIMATE_MODE_AUTO;
            break;
          default:
            this->mode = climate::CLIMATE_MODE_AUTO;
          }
        }
        else
        {
          this->mode = climate::CLIMATE_MODE_OFF;
        }

        this->target_temperature = this->ac_->getTemp();

        uint8_t lg_fan = this->ac_->getFan();
        switch (lg_fan)
        {
        case kVoltasFanLow:
          this->fan_mode = climate::CLIMATE_FAN_LOW;
          break;
        case kVoltasFanMed:
          this->fan_mode = climate::CLIMATE_FAN_MEDIUM;
          break;
        case kVoltasFanHigh:
          this->fan_mode = climate::CLIMATE_FAN_HIGH;
          break;
        case kVoltasFanAuto:
          this->fan_mode = climate::CLIMATE_FAN_AUTO;
          break;
        default:
          this->fan_mode = climate::CLIMATE_FAN_AUTO;
        }

        if (this->ac_->getSwingV() && this->ac_->getSwingH())
        {
          this->swing_mode = climate::CLIMATE_SWING_BOTH;
        }
        else if (this->ac_->getSwingV())
        {
          this->swing_mode = climate::CLIMATE_SWING_VERTICAL;
        }
        else if (this->ac_->getSwingH())
        {
          this->swing_mode = climate::CLIMATE_SWING_HORIZONTAL;
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