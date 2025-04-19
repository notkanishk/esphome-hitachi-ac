#pragma once

#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

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

      void control(const climate::ClimateCall &call) override;
      climate::ClimateTraits traits() override;
      
    };
  } // namespace climate_lgg
} // namespace esphome
