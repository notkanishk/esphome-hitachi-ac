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

#include <map>


namespace esphome {
namespace lg_climate {

class LGClimate : public climate::Climate, public Component {
 public:
  void setup() override;
  void loop() override;
  void control(const climate::ClimateCall &call) override;
  climate::ClimateTraits traits() override;

  void set_transmitter_pin(InternalGPIOPin *pin) { transmitter_pin_ = pin; }
  void set_receiver_pin(InternalGPIOPin *pin) { receiver_pin_ = pin; }

 protected:
  IRLgAc *ac_{nullptr};
  IRsend *ir_send_{nullptr};
  
  IRrecv *ir_recv_{nullptr};
  decode_results results_{};
  InternalGPIOPin *transmitter_pin_{nullptr};
  InternalGPIOPin *receiver_pin_{nullptr};

  std::map<climate::ClimateFanMode,int> fan_mode_int_map = {
    {climate::CLIMATE_FAN_QUIET,0},
    {climate::CLIMATE_FAN_LOW,9},
    {climate::CLIMATE_FAN_MEDIUM,2},
    {climate::CLIMATE_FAN_HIGH,10},
    {climate::CLIMATE_FAN_AUTO,5},
  };

  std::map<int,climate::ClimateFanMode> fan_int_mode_map = {
    {0,climate::CLIMATE_FAN_QUIET},
    {9,climate::CLIMATE_FAN_LOW},
    {2,climate::CLIMATE_FAN_MEDIUM},
    {10,climate::CLIMATE_FAN_HIGH},
    {5,climate::CLIMATE_FAN_AUTO},
  };

  std::map<climate::ClimateMode,int> mode_ir_int_map = {
    {climate::CLIMATE_MODE_COOL,0},
    {climate::CLIMATE_MODE_DRY,1},
    {climate::CLIMATE_MODE_FAN_ONLY,2},
    {climate::CLIMATE_MODE_AUTO,3},
  };
  std::map<int,climate::ClimateMode> mode_int_ir_map = {
    {0,climate::CLIMATE_MODE_COOL},
    {1,climate::CLIMATE_MODE_DRY},
    {2,climate::CLIMATE_MODE_FAN_ONLY},
    {3,climate::CLIMATE_MODE_AUTO}
  };

  std::map<String,int> custom_fanmode_int_map = {
    {"Max",4}
  };
  std::map<int,String> custom_int_fanmode_map = {
    {4,"Max"}
  };

  std::map<climate::ClimatePreset,String> preset_hex_map = {
    {climate::CLIMATE_PRESET_BOOST,"0x88100DE"}
  };

  std::map<String,climate::ClimatePreset> hex_preset_map = {
    {"0x88100DE",climate::CLIMATE_PRESET_BOOST}
  };

  std::map<String,String> power_mode_hex_str_map = {
    {"0x88C07D0","Power - 80%"},
    {"0x88C07E1","Power - 60%"},
    {"0x88C0804","Power - 40%"},
    {"0x88C07F2","Power - 100%"}
  };
  std::map<String,String> power_mode_str_hex_map = {
    {"Power - 80%","0x88C07D0"},
    {"Power - 60%","0x88C07E1"},
    {"Power - 40%","0x88C0804"},        
    {"Power - 100%","0x88C07F2"}
  };

  String prev_custom_preset_;
  climate::ClimatePreset prev_preset_;

  // Handle decoded IR signals
  void decode_and_update();
};

}  // namespace lg_climate
}  // namespace esphome