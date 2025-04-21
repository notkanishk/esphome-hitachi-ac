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
      std::map<String,String> power_mode_map = {
        {"0x88C07D0","Power - 80%"},{"0x88C07E1","Power - 60%"},{"0x88C0804","Power - 40%"},{"0x88C07F2","Power - 100%"}
      };

      std::map<String,String> special_mode_map = {
        {"0x88100DE","Turbo"}
      };

      std::vector<climate::ClimateMode> mode_vec = {
        climate::CLIMATE_MODE_COOL,
        climate::CLIMATE_MODE_DRY,
        climate::CLIMATE_MODE_FAN_ONLY,
        climate::CLIMATE_MODE_AUTO,
      };
      std::map<int,climate::ClimateFanMode> fan_map = {
        {0,climate::CLIMATE_FAN_QUIET},
        {9,climate::CLIMATE_FAN_LOW},
        {2,climate::CLIMATE_FAN_MEDIUM},
        {10,climate::CLIMATE_FAN_HIGH},
        {5,climate::CLIMATE_FAN_AUTO},
      };
      void control(const climate::ClimateCall &call) override;
      climate::ClimateTraits traits() override;

      decode_results *ir_results;
      IRrecv *ir_recv;
      IRLgAc *ac;
    };
  } // namespace climate_lg2
} // namespace esphome
