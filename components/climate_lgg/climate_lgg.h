
#ifndef CLIMATE_LGG_H
#define CLIMATE_LGG_H


#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"
#include "esphome/core/hal.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <ir_LG.h>

// Define GPIO pins – adjust these as needed.
#define IR_LED_PIN 5     // Transmitter pin
#define IR_RECV_PIN 10    // Receiver pin

namespace esphome {
namespace climate_lgg {

/**
 * ClimateLGG implements a climate component for a Hitachi AC using the HITACHI_AC1 protocol.
 * It inherits from esphome::climate::Climate (to integrate with Home Assistant) and Component (for ESPHome lifecycle).
 */
class ClimateLGG : public climate::Climate, public Component {
 public:
  ClimateLGG(InternalGPIOPin *ir_led_pin, InternalGPIOPin *ir_recv_pin);

  // Lifecycle: called once at setup.
  void setup() override;

  // Lifecycle: called continuously.
  void loop() override;

  // Handle control commands from Home Assistant.
  void control(const climate::ClimateCall &call) override;

  void dump_config();

  void set_min_temp(float min_temp) { this->min_temp = min_temp;}
  void set_max_temp(float max_temp) { this->max_temp = max_temp;}

  // Declare supported features (modes, fan speeds, etc.).
  climate::ClimateTraits traits() override;

 protected:
  InternalGPIOPin *_ir_led_pin;
  InternalGPIOPin *_ir_recv_pin;
  float min_temp;
  float max_temp;
  // IR transmitter and receiver objects for controlling the AC.
  IRLgAc *ac;
  IRrecv *irrecv;
  decode_results *results;
};

}  // namespace hitachi_ac
}  // namespace esphome

#endif  // HITACHI_AC_H