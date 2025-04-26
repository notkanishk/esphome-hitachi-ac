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
#include <IRac.h>

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

  std::map<climate::ClimateFanMode, uint8_t> fan_mode_int_map = {
    {climate::CLIMATE_FAN_LOW, 4},
    {climate::CLIMATE_FAN_MEDIUM, 2},
    {climate::CLIMATE_FAN_HIGH, 1},
    {climate::CLIMATE_FAN_AUTO, 7},
  };

  std::map<uint8_t, climate::ClimateFanMode> fan_int_mode_map = {
    {4, climate::CLIMATE_FAN_LOW},
    {2, climate::CLIMATE_FAN_MEDIUM},
    {1, climate::CLIMATE_FAN_HIGH},
    {7, climate::CLIMATE_FAN_AUTO},
  };

  std::map<climate::ClimateMode, uint8_t> mode_ir_int_map = {
    {climate::CLIMATE_MODE_COOL, 8},
    {climate::CLIMATE_MODE_DRY, 4},
    {climate::CLIMATE_MODE_FAN_ONLY, 1},
    {climate::CLIMATE_MODE_HEAT,2}
  };
  
  std::map<uint8_t, climate::ClimateMode> mode_int_ir_map = {
    {8, climate::CLIMATE_MODE_COOL},
    {4, climate::CLIMATE_MODE_DRY},
    {1, climate::CLIMATE_MODE_FAN_ONLY},
    {2,climate::CLIMATE_MODE_HEAT}
  };

  std::map<climate::ClimateSwingMode, uint8_t> swing_mode_int_map = {
    {climate::CLIMATE_SWING_OFF, 0},
    {climate::CLIMATE_SWING_VERTICAL, 1},
  };

  std::map<uint8_t, climate::ClimateSwingMode> swing_int_mode_map = {
    {0, climate::CLIMATE_SWING_OFF},
    {1, climate::CLIMATE_SWING_VERTICAL},
  };

  // Handle decoded IR signals
  void dump_readable_ir_recv();
  void decode_and_update();
};

}  // namespace voltas_climate
}  // namespace esphome