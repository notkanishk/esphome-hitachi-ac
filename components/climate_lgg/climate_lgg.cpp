#include "climate_lgg.h"

namespace esphome {

namespace climate_lgg {


static const char *const TAG = "climate_lgg";

ClimateLGG::ClimateLGG()
  : ac(IR_LED_PIN), irrecv(IR_RECV_PIN, 512) {}


void ClimateLGG::dump_config() {
  ESP_LOGCONFIG(TAG,"Climate LGG Configuration: \n");
}


void ClimateLGG::setup() {

}

void ClimateLGG::loop() {
  // Check if an IR packet has been received.
}


void ClimateLGG::control(const climate::ClimateCall &call) {
  // Publish the new state to Home Assistant.
  publish_state();
}

climate::ClimateTraits ClimateLGG::traits() {
  auto traits = climate::ClimateTraits();
  traits.set_supports_current_temperature(false);  // No current temperature sensor.
  traits.set_supported_modes({
    climate::CLIMATE_MODE_OFF,
    climate::CLIMATE_MODE_COOL,
    climate::CLIMATE_MODE_DRY,
    climate::CLIMATE_MODE_FAN_ONLY,
    climate::CLIMATE_MODE_AUTO
  });
  // Optional: set fan modes if applicable.
  traits.set_supported_fan_modes({
    climate::CLIMATE_FAN_QUIET,
    climate::CLIMATE_FAN_LOW,
    climate::CLIMATE_FAN_MIDDLE,
    climate::CLIMATE_FAN_MEDIUM,
    climate::CLIMATE_FAN_HIGH,
    climate::CLIMATE_FAN_AUTO
  });
  traits.set_supports_two_point_target_temperature(false);
  traits.set_supported_swing_modes({
    climate::CLIMATE_SWING_HORIZONTAL,
    climate::CLIMATE_SWING_OFF
  });
  return traits;
}

}  // namespace hitachi_ac
}  // namespace esphome