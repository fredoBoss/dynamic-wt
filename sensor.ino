
// === Calibration Constants ===
#define ZERO_OFFSET  -16515.32
#define SCALE_FACTOR 0.00234



extern bool calibrated;

// float plateWeight = 90;

void initLoadCell() {
  scale.begin(loadcellDout, loadcellSck);
  scale.set_scale();  // We'll use manual conversion
  delay(100);
  scale.tare();       // Optional tare
}

float convertToWeight(long raw) {
  return (raw - ZERO_OFFSET) * SCALE_FACTOR;
}

float weightValAvg() {
  long raw = scale.get_value(2);  // Average of 2 samples

  scale.power_down();
  delay(1000);
  scale.power_up();

  return convertToWeight(raw);  // in grams
}

float weightVal() {
  long raw;
  if (calibrated) {
    raw = scale.get_value();
    return convertToWeight(raw);
  } else {
    return scale.get_value();  // raw value if not calibrated
  }
}

long calibrateWeight() {
  if (scale.is_ready()) {
    scale.set_scale();
    delay(5000);
    scale.tare();
    delay(5000);

    long reading = scale.get_units(20);  // With known weight
    return reading;
  } else {
    Serial.println("HX711 not found.");
    return 0;
  }
}

