#include "voltas_climate.h"
#include "esphome/core/log.h"

namespace esphome {
namespace voltas_climate {

static const char *const TAG = "voltas_climate";

void VoltasClimate::setup() {
  // Initialize the IR sender
  this->ir_send_ = new IRsend(this->transmitter_pin_->get_pin());
  this->ir_send_->begin();

  // Initialize the IR Voltas AC controller
  this->ac_ = new IRVoltas(this->transmitter_pin_->get_pin());
  this->ac_->begin();
  this->ac_->stateReset();
  this->ac_->setModel(voltas_ac_remote_model_t::R122LZF); // Set to 122LZF model

  // Setup IR receiver if pin is provided
  if (this->receiver_pin_ != nullptr) {
    this->ir_recv_ = new IRrecv(this->receiver_pin_->get_pin());
    this->ir_recv_->enableIRIn();
  }

  // Set initial target temperature to 24°C
  this->target_temperature = 24;
  this->mode = climate::CLIMATE_MODE_OFF;
  this->fan_mode = climate::CLIMATE_FAN_AUTO;
  this->swing_mode = climate::CLIMATE_SWING_OFF;
}

void VoltasClimate::loop() {
  if (this->ir_recv_ != nullptr && this->ir_recv_->decode(&this->results_)) {
    this->ir_recv_->disableIRIn();
    this->decode_and_update();
    this->ir_recv_->enableIRIn();
  }
}

void VoltasClimate::control(const climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    auto mode = *call.get_mode();
    this->mode = mode;

    if (this->mode == climate::CLIMATE_MODE_OFF) {
      this->ac_->setPower(false);
    } else {
      this->ac_->setPower(true);
      
      if (this->mode_ir_int_map.count(this->mode)) {
        this->ac_->setMode(this->mode_ir_int_map[this->mode]);
      }
    }
  }

  if (call.get_target_temperature().has_value()) {
    float temp = *call.get_target_temperature();
    this->target_temperature = temp;
    // Voltas AC uses celsius temps, converting float to int
    this->ac_->setTemp(static_cast<uint8_t>(std::round(temp)));
  }

  if (call.get_fan_mode().has_value()) {
    auto fan_mode = *call.get_fan_mode();
    this->fan_mode = fan_mode;
    ESP_LOGD(TAG, "Fan Mode: %d", this->fan_mode_int_map[this->fan_mode]);
    this->ac_->setFan(this->fan_mode_int_map[this->fan_mode]);
  }

  if (call.get_swing_mode().has_value()) {
    auto swing_mode = *call.get_swing_mode();
    this->swing_mode = swing_mode;
    
    if (this->swing_mode_int_map.count(this->swing_mode)) {
      this->ac_->setSwing(this->swing_mode_int_map[this->swing_mode]);
    }
  }

  // Send the IR command
  this->ac_->send();

  ESP_LOGD(TAG, "Voltas AC sending command: mode=%d, temp=%.1f, fan=%d, swing=%d",
           static_cast<int>(this->mode), this->target_temperature, 
           static_cast<int>(this->fan_mode), static_cast<int>(this->swing_mode));

  this->publish_state();
}

climate::ClimateTraits VoltasClimate::traits() {
  auto traits = climate::ClimateTraits();
  traits.set_supports_current_temperature(false);
  traits.set_supported_modes({
      climate::CLIMATE_MODE_OFF,
      climate::CLIMATE_MODE_AUTO,
      climate::CLIMATE_MODE_COOL,
      climate::CLIMATE_MODE_DRY,
      climate::CLIMATE_MODE_FAN_ONLY,
      climate::CLIMATE_MODE_HEAT,
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
  });
  traits.set_supported_swing_modes({
      climate::CLIMATE_SWING_OFF,
      climate::CLIMATE_SWING_VERTICAL,
  });
  return traits;
}

void VoltasClimate::decode_and_update() {
  // Check if the decoded result is from a Voltas AC
  if (this->results_.decode_type == decode_type_t::VOLTAS) {
    
    this->ac_->setRaw(this->results_.state, this->results_.bits / 8);
    
    ESP_LOGD(TAG, "Received Voltas AC command");

    // Update local state based on received command
    if (this->ac_->getPower()) {
      uint8_t voltas_mode = this->ac_->getMode();

      if (this->mode_int_ir_map.count(voltas_mode)) {
        this->mode = this->mode_int_ir_map[voltas_mode];
      } else {
        this->mode = climate::CLIMATE_MODE_AUTO;
      }
    } else {
      this->mode = climate::CLIMATE_MODE_OFF;
    }

    this->target_temperature = this->ac_->getTemp();

    uint8_t voltas_fan = this->ac_->getFan();
    if (this->fan_int_mode_map.count(voltas_fan)) {
      this->fan_mode = this->fan_int_mode_map[voltas_fan];
    } else {
      this->fan_mode = climate::CLIMATE_FAN_AUTO;
    }

    uint8_t voltas_swing = this->ac_->getSwing();
    if (this->swing_int_mode_map.count(voltas_swing)) {
      this->swing_mode = this->swing_int_mode_map[voltas_swing];
    } else {
      this->swing_mode = climate::CLIMATE_SWING_OFF;
    }

    // Update the ESPHome climate state
    this->publish_state();
  }
}

}  // namespace voltas_climate
}  // namespace esphome