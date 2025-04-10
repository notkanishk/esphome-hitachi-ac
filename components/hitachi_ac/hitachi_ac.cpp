#include "hitachi_ac.h"

namespace esphome {

namespace hitachi_ac {

HitachiACClimate::HitachiACClimate()
  : ac(IR_LED_PIN), irrecv(IR_RECV_PIN, 512) {}

void HitachiACClimate::setup() {
  // Initialize the IR transmitter.
  ac.begin();
  // Set a default state: power on, cooling mode, default temperature.
  ac.setPower(true);
  ac.setMode(kHitachiAcCool);
  ac.setTemp(24);
  ac.send();

  // Start the IR receiver.
  irrecv.enableIRIn();
}

void HitachiACClimate::loop() {
  // Check if an IR packet has been received.
  if (irrecv.decode(&results)) {
    // Decode the stateful IR packet.
    // if (ac.decodeState(results)) {
    ac.setRaw(results.state);

    Climate::mode = ac.getMode();
    Climate::target_temperature = ac.getTemp();

    // If successful, update Home Assistant with the new state.
    publish_state();

    // Ready the receiver for the next packet.
    irrecv.resume();
  }
}


void HitachiACClimate::control(const climate::ClimateCall &call) {
  // Process mode changes.
  if (call.get_mode().has_value()) {
    climate::ClimateMode mode = *call.get_mode();
    switch (mode) {
      case climate::CLIMATE_MODE_COOL:
        ac.setMode(kHitachiAcCool);
        break;
      case climate::CLIMATE_MODE_HEAT:
        ac.setMode(kHitachiAcHeat);
        break;
      case climate::CLIMATE_MODE_AUTO:
        ac.setMode(kHitachiAcAuto);
        break;
      case climate::CLIMATE_MODE_OFF:
        ac.setPower(false);
        break;
      default:
        break;
    }
  }
  // Process temperature changes.
  if (call.get_target_temperature().has_value()) {
    ac.setTemp(*call.get_target_temperature());
  }
  // If the AC is off and a command is issued, ensure it turns on.
  if (!ac.getPower()) {
    ac.setPower(true);
  }
  // Send the updated command.
  ac.send();
  // Publish the new state to Home Assistant.
  publish_state();
}

climate::ClimateTraits HitachiACClimate::traits() {
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
    climate::CLIMATE_FAN_LOW,
    climate::CLIMATE_FAN_MEDIUM,
    climate::CLIMATE_FAN_HIGH
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