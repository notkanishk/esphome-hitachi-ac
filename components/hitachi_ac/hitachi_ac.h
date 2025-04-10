
#ifndef HITACHI_AC_H
#define HITACHI_AC_H


#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <ir_Hitachi.h>  // Ensure you have the protocol header available

// Define GPIO pins – adjust these as needed.
#define IR_LED_PIN 5     // Transmitter pin
#define IR_RECV_PIN 10    // Receiver pin

namespace esphome {
namespace hitachi_ac {

/**
 * HitachiACClimate implements a climate component for a Hitachi AC using the HITACHI_AC1 protocol.
 * It inherits from esphome::climate::Climate (to integrate with Home Assistant) and Component (for ESPHome lifecycle).
 */
class HitachiACClimate : public climate::Climate, public Component {
 public:
  HitachiACClimate();

  // Lifecycle: called once at setup.
  void setup() override;

  // Lifecycle: called continuously.
  void loop() override;

  // Handle control commands from Home Assistant.
  void control(const climate::ClimateCall &call) override;

  // Declare supported features (modes, fan speeds, etc.).
  climate::ClimateTraits traits() override;

 protected:
  // IR transmitter and receiver objects for controlling the AC.
  IRHitachiAc1 ac;
  IRrecv irrecv;
  decode_results results;
};

}  // namespace hitachi_ac
}  // namespace esphome

#endif  // HITACHI_AC_H