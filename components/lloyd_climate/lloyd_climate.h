#pragma once


#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"
#include "esphome/core/gpio.h"

// Include IRremoteESP8266 library headers
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <ir_Voltas.h>

#include <map>


namespace esphome {
namespace voltas_climate {

class VoltasClimate : public climate::Climate, public Component {
 public:
  void setup() override;
  void loop() override;
  void control(const climate::ClimateCall &call) override;
  climate::ClimateTraits traits() override;

  void set_transmitter_pin(InternalGPIOPin *pin) { transmitter_pin_ = pin; }
  void set_receiver_pin(InternalGPIOPin *pin) { receiver_pin_ = pin; }

 protected:
  IRVoltas *ac_{nullptr};
  IRsend *ir_send_{nullptr};
  IRrecv *ir_recv_{nullptr};
  decode_results results_{};
  InternalGPIOPin *transmitter_pin_{nullptr};
  InternalGPIOPin *receiver_pin_{nullptr};

  std::map<climate::ClimateFanMode,int> fan_mode_int_map = {
    {climate::CLIMATE_FAN_QUIET,4},
    {climate::CLIMATE_FAN_LOW,4},
    {climate::CLIMATE_FAN_MEDIUM,2},
    {climate::CLIMATE_FAN_HIGH,1},
    {climate::CLIMATE_FAN_AUTO,7},
  };

  std::map<String,int> custom_fan_mode_int_map = {
    {"Max",1},
    {"Medium", 2},
    {"Low",4},
  };

  
  

  // Handle decoded IR signals
  void decode_and_update();
};

}  // namespace lg_climate
}  // namespace esphome