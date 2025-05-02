#pragma once

#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/climate/climate_mode.h"
#include "esphome/components/climate/climate_traits.h"
#include "esphome/core/gpio.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <ir_Hitachi.h>

namespace esphome {
namespace hitachi_ac {

class HitachiAC : public climate::Climate, public Component {
 public:
  HitachiAC(const std::string &name) : Climate(), name_(name) {}

  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

  // Set configuration options
  void set_transmitter_pin(InternalGPIOPin *pin) { transmitter_pin_ = pin; }
  void set_receiver_pin(InternalGPIOPin *pin) { receiver_pin_ = pin; }

 protected:
  // Override climate control functions
  void control(const climate::ClimateCall &call) override;
  climate::ClimateTraits traits() override;

 private:

 const uint8_t kHitachiAc1SwingVOff = 0;
 const uint8_t kHitachiAc1SwingVOn = 1;
  // Send IR command to the AC unit
  void transmit_state_();
  
  // Decode received IR signal
  void decode_and_update_state_(const uint8_t *state);
  
  // Convert between ESPHome climate states and Hitachi AC states
  climate::ClimateMode hitachi_mode_to_climate_mode_(uint8_t mode);
  uint8_t climate_mode_to_hitachi_mode_(climate::ClimateMode mode);
  
  climate::ClimateFanMode hitachi_fan_to_climate_fan_(uint8_t fan);
  uint8_t climate_fan_to_hitachi_fan_(climate::ClimateFanMode fan);
  
  climate::ClimateSwingMode hitachi_swing_to_climate_swing_(uint8_t swing);
  uint8_t climate_swing_to_hitachi_swing_(climate::ClimateSwingMode swing);

  // Configuration
  std::string name_;
  InternalGPIOPin *transmitter_pin_{nullptr};
  InternalGPIOPin *receiver_pin_{nullptr};
//   bool receiver_enabled_{false};
  
  // Hardware
  IRsend *ir_send_{nullptr};
  IRrecv *ir_recv_{nullptr};
  decode_results ir_results_;
  IRHitachiAc1 *hitachi_{nullptr};
  
  // Internal state tracking
  uint32_t last_received_{0};
  bool receiving_{false};
};

}  // namespace hitachi_ac
}  // namespace esphome