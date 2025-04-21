#pragma once

#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

#include "IRremoteESP8266.h"
#include "IRrecv.h"
#include "IRac.h"
#include "ir_LG.h"
#include "IRutils.h"
#include "IRtext.h"

#include <map>



namespace esphome
{
  namespace climate_lg2
  {

    class ClimateLG : public climate::Climate, public Component
    {
    public:
      ClimateLG(InternalGPIOPin *ir_led_pin,InternalGPIOPin *ir_recv_pin);
      void setup() override;
      void loop() override;
      void dump_config() override;

    protected:
      InternalGPIOPin *ir_led_pin;
      InternalGPIOPin *ir_recv_pin;
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

      std::map<String,String> special_mode_map = {
        {"0x88100DE","Turbo"}
      };
      std::map<climate::ClimateMode,int> mode_ir_mode_map = {
        {climate::CLIMATE_MODE_COOL,0},
        {climate::CLIMATE_MODE_DRY,1},
        {climate::CLIMATE_MODE_FAN_ONLY,2},
        {climate::CLIMATE_MODE_AUTO,3},
      };
      std::vector<climate::ClimateMode> mode_vec = {
        climate::CLIMATE_MODE_COOL,
        climate::CLIMATE_MODE_DRY,
        climate::CLIMATE_MODE_FAN_ONLY,
        climate::CLIMATE_MODE_AUTO,
      };
      std::map<int,climate::ClimateFanMode> int_fan_map = {
        {0,climate::CLIMATE_FAN_QUIET},
        {9,climate::CLIMATE_FAN_LOW},
        {2,climate::CLIMATE_FAN_MEDIUM},
        {10,climate::CLIMATE_FAN_HIGH},
        {5,climate::CLIMATE_FAN_AUTO},
      };
      std::map<climate::ClimateFanMode,int> fan_int_map = {
        {climate::CLIMATE_FAN_QUIET,0},
        {climate::CLIMATE_FAN_LOW,9},
        {climate::CLIMATE_FAN_MEDIUM,2},
        {climate::CLIMATE_FAN_HIGH,10},
        {climate::CLIMATE_FAN_AUTO,5},
      };
      std::map<String,int> custom_fan_int_map = {
        {"Max",4}
      };

      void control(const climate::ClimateCall &call) override;
      climate::ClimateTraits traits() override;

      void onReceive(decode_results *results);
      void transmit();

      decode_results *ir_results;
      IRrecv *ir_recv;
      IRLgAc *ac;
    };
  } // namespace climate_lg2
} // namespace esphome
