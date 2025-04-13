#pragma once

#include <utility>

#include "esphome/components/climate/climate.h"

namespace esphome {
namespace climate_lg2 {

class ClimateLG2 : public Component,public climate::Climate
{
 public:
  ClimateLG2() {
    this->minimum_temperature_ = 16;
    this->maximum_temperature_ = 30;
    this->temperature_step_ = 1;
    this->supports_dry_ = true;
    this->supports_fan_only_ = true;
    this->fan_modes_ = std::set<climate::ClimateFanMode>{};
    this->swing_modes_ = std::set<climate::ClimateSwingMode>{};
    this->presets_ = std::set<climate::ClimatePreset>{};
  }

  void setup() override;
  void dump_config() override;
  void set_supports_cool(bool supports_cool) { this->supports_cool_ = supports_cool; }
  void set_supports_heat(bool supports_heat) { this->supports_heat_ = supports_heat; }

 protected:
  int minimum_temperature_, maximum_temperature_, temperature_step_;

  /// Override control to change settings of the climate device.
  void control(const climate::ClimateCall &call) override;
  /// Return the traits of this controller.
  climate::ClimateTraits traits() override;

  /// Transmit via IR the state of this climate controller.
  virtual void transmit_state() = 0;

  bool supports_cool_{true};
  bool supports_heat_{true};
  bool supports_dry_{false};
  bool supports_fan_only_{false};
  std::set<climate::ClimateFanMode> fan_modes_ = {};
  std::set<climate::ClimateSwingMode> swing_modes_ = {};
  std::set<climate::ClimatePreset> presets_ = {};

};

}  // namespace climate_lg2
}  // namespace esphome
