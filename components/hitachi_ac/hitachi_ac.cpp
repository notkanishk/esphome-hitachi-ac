#include "esphome/core/log.h"
#include "hitachi_ac.h"

namespace esphome {
namespace hitachi_ac {

static const char *TAG = "hitachi_ac.climate";

// Climate setup - initialize hardware and set initial state
void HitachiAC::setup() {
  // Initialize the IR sender
//   this->ir_send_ = new IRsend(this->transmitter_pin_);
//   this->ir_send_->begin();
  
  // Initialize the Hitachi AC controller
  this->hitachi_ = new IRHitachiAc1(this->transmitter_pin_->get_pin());
  
  // Initialize the IR receiver if enabled
  this->ir_recv_ = new IRrecv(this->receiver_pin_->get_pin(), 512);
  this->ir_recv_->enableIRIn();
  
  // Set default values
  this->mode = climate::CLIMATE_MODE_OFF;
  this->target_temperature = 25;
  this->fan_mode = climate::CLIMATE_FAN_AUTO;
  this->swing_mode = climate::CLIMATE_SWING_OFF;
  
  // Make sure we update the climate state
  this->publish_state();
}

void HitachiAC::loop() {
  // Handle IR reception if enabled
  if (this->ir_recv_->decode(&this->ir_results_)) {
    // Check if this is a Hitachi AC1 protocol
    if (this->ir_results_.decode_type == decode_type_t::HITACHI_AC1) {
      ESP_LOGD(TAG, "Received Hitachi AC1 message");
      // Update our internal state based on the received IR command
      this->decode_and_update_state_(this->ir_results_.state);
      this->publish_state();
    }
    
    // Prepare for next reception
    this->ir_recv_->resume();
  }
}

// Log the component configuration
void HitachiAC::dump_config() {
  ESP_LOGCONFIG(TAG, "Hitachi AC:");
  ESP_LOGCONFIG(TAG, "  Name: %s", this->name_.c_str());
  ESP_LOGCONFIG(TAG, "  Transmitter Pin: %u", this->transmitter_pin_);
  ESP_LOGCONFIG(TAG, "  Receiver Pin: %u", this->receiver_pin_);
  ESP_LOGCONFIG(TAG, "  Receiving: %s", this->receiving_ ? "YES" : "NO");
  ESP_LOGCONFIG(TAG, "  Last Received: %u", this->last_received_);
  ESP_LOGCONFIG(TAG, "  IR Hitachi: %s", this->hitachi_->toString().c_str());
  ESP_LOGCONFIG(TAG, "  IR Results: %s", this->ir_results_);

}

// Handle climate control calls
void HitachiAC::control(const climate::ClimateCall &call) {
  bool state_changed = false;
  
  // Handle mode changes
  if (call.get_mode().has_value()) {
    climate::ClimateMode mode = *call.get_mode();
    this->mode = mode;
    state_changed = true;
  }
  
  // Handle temperature changes
  if (call.get_target_temperature().has_value()) {
    float temp = *call.get_target_temperature();
    this->target_temperature = temp;
    state_changed = true;
  }
  
  // Handle fan mode changes
  if (call.get_fan_mode().has_value()) {
    climate::ClimateFanMode fan_mode = *call.get_fan_mode();
    this->fan_mode = fan_mode;
    state_changed = true;
  }
  
  // Handle swing mode changes
  if (call.get_swing_mode().has_value()) {
    climate::ClimateSwingMode swing_mode = *call.get_swing_mode();
    this->swing_mode = swing_mode;
    state_changed = true;
  }
  
  // Send the updated state to the AC if anything changed
  if (state_changed) {
    this->transmit_state_();
    this->publish_state();
  }
}

// Define climate traits (supported modes and features)
climate::ClimateTraits HitachiAC::traits() {
  auto traits = climate::ClimateTraits();
  
  // Temperature range
  traits.set_visual_min_temperature(16);
  traits.set_visual_max_temperature(30);
  traits.set_visual_temperature_step(1.0f);
  
  traits.set_supports_two_point_target_temperature(false);
  traits.set_supports_current_temperature(false);

  // Supported modes
  traits.set_supported_modes({
    climate::CLIMATE_MODE_OFF,
    climate::CLIMATE_MODE_COOL,
    climate::CLIMATE_MODE_DRY,
    climate::CLIMATE_MODE_FAN_ONLY,
    climate::CLIMATE_MODE_AUTO
  });
  // Optional: set fan modes if applicable.
  traits.set_supported_fan_modes({
    climate::CLIMATE_FAN_AUTO,
    climate::CLIMATE_FAN_LOW,
    climate::CLIMATE_FAN_MEDIUM,
    climate::CLIMATE_FAN_HIGH
  });
  traits.set_supported_swing_modes({
    climate::CLIMATE_SWING_VERTICAL,
    climate::CLIMATE_SWING_OFF
  });
  
  return traits;
}

// Send IR command based on current state
void HitachiAC::transmit_state_() {
  // Initialize state with off mode
  this->hitachi_->begin();
  
  // If the mode is not OFF, set all other parameters
  if (this->mode != climate::CLIMATE_MODE_OFF) {
    // Set mode
    this->hitachi_->setMode(this->climate_mode_to_hitachi_mode_(this->mode));
    
    // Set temperature
    this->hitachi_->setTemp(static_cast<uint8_t>(this->target_temperature));
    
    // Set fan speed
    this->hitachi_->setFan(this->climate_fan_to_hitachi_fan_(this->fan_mode.value()));
    
    // Set swing mode
    this->hitachi_->setSwingV(this->climate_swing_to_hitachi_swing_(this->swing_mode));

    // Turn on the unit
    this->hitachi_->setPower(true);

  } else {
    // Turn off the unit
    this->hitachi_->setPower(false);
  }
  
  // Send the IR command
  this->hitachi_->send();
  
  ESP_LOGD(TAG, "Sent Hitachi AC1 IR command");
}

// Decode received IR signal and update the internal state
void HitachiAC::decode_and_update_state_(const uint8_t *state) {
  // Create a temporary Hitachi AC object to decode the state
  IRHitachiAc1 temp_hitachi(this->transmitter_pin_->get_pin());
  temp_hitachi.setRaw(state);
  
  // Update our internal state
  this->mode = this->hitachi_mode_to_climate_mode_(temp_hitachi.getMode());
  this->target_temperature = temp_hitachi.getTemp();
  this->fan_mode = this->hitachi_fan_to_climate_fan_(temp_hitachi.getFan());
  
  this->swing_mode = this->hitachi_swing_to_climate_swing_(temp_hitachi.getSwingV());

}

// Conversion functions between ESPHome climate states and Hitachi AC states
climate::ClimateMode HitachiAC::hitachi_mode_to_climate_mode_(uint8_t mode) {
  switch (mode) {
    case kHitachiAc1Cool:
      return climate::CLIMATE_MODE_COOL;
    case kHitachiAc1Heat:
      return climate::CLIMATE_MODE_HEAT;
    case kHitachiAc1Auto:
      return climate::CLIMATE_MODE_AUTO;
    case kHitachiAc1Dry:
      return climate::CLIMATE_MODE_DRY;
    case kHitachiAc1Fan:
      return climate::CLIMATE_MODE_FAN_ONLY;
    default:
      return climate::CLIMATE_MODE_OFF;
  }
}

uint8_t HitachiAC::climate_mode_to_hitachi_mode_(climate::ClimateMode mode) {
  switch (mode) {
    case climate::CLIMATE_MODE_COOL:
      return kHitachiAc1Cool;
    case climate::CLIMATE_MODE_HEAT:
      return kHitachiAc1Heat;
    case climate::CLIMATE_MODE_AUTO:
      return kHitachiAc1Auto;
    case climate::CLIMATE_MODE_DRY:
      return kHitachiAc1Dry;
    case climate::CLIMATE_MODE_FAN_ONLY:
      return kHitachiAc1Fan;
    default:
      return kHitachiAc1Auto;  // Default to auto if unknown
  }
}

climate::ClimateFanMode HitachiAC::hitachi_fan_to_climate_fan_(uint8_t fan) {
  switch (fan) {
    case kHitachiAc1FanHigh:
      return climate::CLIMATE_FAN_HIGH;
    case kHitachiAc1FanMed:
      return climate::CLIMATE_FAN_MEDIUM;
    case kHitachiAc1FanLow:
      return climate::CLIMATE_FAN_LOW;
    case kHitachiAc1FanAuto:
    default:
      return climate::CLIMATE_FAN_AUTO;
  }
}

uint8_t HitachiAC::climate_fan_to_hitachi_fan_(climate::ClimateFanMode fan) {
  switch (fan) {
    case climate::CLIMATE_FAN_HIGH:
      return kHitachiAc1FanHigh;
    case climate::CLIMATE_FAN_MEDIUM:
      return kHitachiAc1FanMed;
    case climate::CLIMATE_FAN_LOW:
      return kHitachiAc1FanLow;
    case climate::CLIMATE_FAN_AUTO:
    default:
      return kHitachiAc1FanAuto;
  }
}

climate::ClimateSwingMode HitachiAC::hitachi_swing_to_climate_swing_(uint8_t swing) {
  if (swing == kHitachiAc1SwingVOff) {
    return climate::CLIMATE_SWING_OFF;
  } else {
    return climate::CLIMATE_SWING_VERTICAL;
  }
}

uint8_t HitachiAC::climate_swing_to_hitachi_swing_(climate::ClimateSwingMode swing) {
  if (swing == climate::CLIMATE_SWING_OFF) {
    return kHitachiAc1SwingVOff;
  } else {
    return kHitachiAc1SwingVOn;
  }
}

}  // namespace hitachi_ac
}  // namespace esphome