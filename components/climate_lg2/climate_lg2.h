#pragma once

#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"
#include "esphome/core/gpio.h"

// Include IRremoteESP8266 library headers
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <ir_LG.h>

namespace esphome {
namespace lg_climate {

class LGClimate : public climate::Climate, public Component {
 public:
  void setup() override;
  void loop() override;
  void control(const climate::ClimateCall &call) override;
  climate::ClimateTraits traits() override;

  void set_transmitter_pin(GPIOPin *pin) { transmitter_pin_ = pin; }
  void set_receiver_pin(GPIOPin *pin) { receiver_pin_ = pin; }

 protected:
  IRLgAc *ac_{nullptr};
  IRsend *ir_send_{nullptr};
  IRrecv *ir_recv_{nullptr};
  decode_results results_{};
  GPIOPin *transmitter_pin_{nullptr};
  GPIOPin *receiver_pin_{nullptr};

  // Handle decoded IR signals
  void decode_and_update();
};

}  // namespace lg_climate
}  // namespace esphome