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

  std::map<climate::ClimateFanMode, uint8_t> fan_mode_int_map = {
    {climate::CLIMATE_FAN_LOW, kVoltasFanLow},
    {climate::CLIMATE_FAN_MEDIUM, kVoltasFanMedium},
    {climate::CLIMATE_FAN_HIGH, kVoltasFanHigh},
    {climate::CLIMATE_FAN_AUTO, kVoltasFanAuto},
  };

  std::map<uint8_t, climate::ClimateFanMode> fan_int_mode_map = {
    {kVoltasFanLow, climate::CLIMATE_FAN_LOW},
    {kVoltasFanMedium, climate::CLIMATE_FAN_MEDIUM},
    {kVoltasFanHigh, climate::CLIMATE_FAN_HIGH},
    {kVoltasFanAuto, climate::CLIMATE_FAN_AUTO},
  };

  std::map<climate::ClimateMode, uint8_t> mode_ir_int_map = {
    {climate::CLIMATE_MODE_COOL, kVoltasCool},
    {climate::CLIMATE_MODE_DRY, kVoltasDry},
    {climate::CLIMATE_MODE_FAN_ONLY, kVoltasFan},
    {climate::CLIMATE_MODE_AUTO, kVoltasAuto},
    {climate::CLIMATE_MODE_HEAT, kVoltasHeat},
  };
  
  std::map<uint8_t, climate::ClimateMode> mode_int_ir_map = {
    {kVoltasCool, climate::CLIMATE_MODE_COOL},
    {kVoltasDry, climate::CLIMATE_MODE_DRY},
    {kVoltasFan, climate::CLIMATE_MODE_FAN_ONLY},
    {kVoltasAuto, climate::CLIMATE_MODE_AUTO},
    {kVoltasHeat, climate::CLIMATE_MODE_HEAT},
  };

  std::map<climate::ClimateSwingMode, uint8_t> swing_mode_int_map = {
    {climate::CLIMATE_SWING_OFF, kVoltasSwingOff},
    {climate::CLIMATE_SWING_VERTICAL, kVoltasSwingOn},
  };

  std::map<uint8_t, climate::ClimateSwingMode> swing_int_mode_map = {
    {kVoltasSwingOff, climate::CLIMATE_SWING_OFF},
    {kVoltasSwingOn, climate::CLIMATE_SWING_VERTICAL},
  };

  // Handle decoded IR signals
  void decode_and_update();
};

}  // namespace voltas_climate
}  // namespace esphome