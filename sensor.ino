#include <EEPROM.h>
#include <HX711.h>

extern bool calibrated;
extern HX711 scale;
extern float balancePt[5];
extern int currentPlate;

float ZERO_OFFSET = 0.0;
float SCALE_FACTOR = 1.0;

void clearEEPROM() {
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
  Serial.println("EEPROM cleared");
}

void saveBalancePts() {
  int offset = sizeof(float) * 2; // After ZERO_OFFSET and SCALE_FACTOR
  for (int i = 0; i < 5; i++) {
    EEPROM.put(offset + i * sizeof(float), balancePt[i]);
  }
  Serial.println("balancePt values saved to EEPROM");
}

void loadBalancePts() {
  int offset = sizeof(float) * 2;
  for (int i = 0; i < 5; i++) {
    EEPROM.get(offset + i * sizeof(float), balancePt[i]);
    if (isnan(balancePt[i]) || balancePt[i] == 0.0) {
      balancePt[i] = 0.0; // Default if invalid
    }
  }
  Serial.println("balancePt values loaded from EEPROM");
  for (int i = 0; i < 5; i++) {
    Serial.println("balancePt[" + String(i+1) + "] (g): " + String(balancePt[i]));
  }
}

void initLoadCell() {
  scale.begin(loadcellDout, loadcellSck);
  int retries = 5;
  while (!scale.is_ready() && retries > 0) {
    Serial.println("HX711 not found, retrying...");
    delay(1000);
    retries--;
  }
  if (!scale.is_ready()) {
    Serial.println("HX711 failed to initialize. Halting.");
    while (true);
  }
  delay(100);
  loadCalibration();
  loadBalancePts();
  scale.tare(20);
  balancePt[currentPlate-1] = weightValAvg(); // Set for current plate
  saveBalancePts();
  Serial.println("Initial balancePt for plate " + String(currentPlate) + " (g): " + String(balancePt[currentPlate-1]));
}

void saveCalibration(float zeroOffset, float scaleFactor) {
  EEPROM.put(0, zeroOffset);
  EEPROM.put(sizeof(float), scaleFactor);
  Serial.println("Calibration saved to EEPROM");
}

void loadCalibration() {
  EEPROM.get(0, ZERO_OFFSET);
  EEPROM.get(sizeof(float), SCALE_FACTOR);
  if (isnan(ZERO_OFFSET) || isnan(SCALE_FACTOR) || ZERO_OFFSET == 0.0 || SCALE_FACTOR == 0.0) {
    Serial.println("No valid calibration in EEPROM, using defaults.");
    ZERO_OFFSET = 0.0;
    SCALE_FACTOR = 1.0;
    calibrated = false;
  } else {
    calibrated = true;
    Serial.println("Calibration loaded: ZERO_OFFSET=" + String(ZERO_OFFSET) + ", SCALE_FACTOR=" + String(SCALE_FACTOR));
  }
}

float convertToWeight(long raw) {
  return (raw - ZERO_OFFSET) * SCALE_FACTOR;
}

float weightValAvg() {
  long readings[10];
  for (int i = 0; i < 10; i++) {
    readings[i] = scale.get_value();
    Serial.println("Raw reading " + String(i) + ": " + String(readings[i]));
    delay(10);
  }
  sort(readings, 10);
  long raw = readings[5]; // Median of 10 readings
  Serial.println("Median raw: " + String(raw));
  scale.power_down();
  delay(100);
  scale.power_up();
  return convertToWeight(raw);
}

float weightVal() {
  long raw = scale.get_value();
  Serial.println("Raw reading: " + String(raw));
  if (calibrated) {
    return convertToWeight(raw);
  }
  return raw;
}

void sort(long arr[], int n) {
  for (int i = 0; i < n-1; i++)
    for (int j = 0; j < n-i-1; j++)
      if (arr[j] > arr[j+1]) {
        long temp = arr[j];
        arr[j] = arr[j+1];
        arr[j+1] = temp;
      }
}

void waitForUserConfirmation() {
  Serial.println("Press 'y' to continue...");
  while (true) {
    if (Serial.available() && Serial.read() == 'y') break;
    delay(100);
  }
}

float calculateScaleFactor() {
  float knownWeights[] = {100.0, 406.0, 978.0, 1220.0}; // Reliable weights
  float scaleFactors[4];
  long zeroOffset = scale.read_average(10);
  for (int i = 0; i < 4; i++) {
    Serial.print("Place weight: "); Serial.println(knownWeights[i]);
    waitForUserConfirmation();
    long reading = scale.read_average(10);
    scaleFactors[i] = (float)(reading - zeroOffset) / knownWeights[i];
    Serial.print("Raw reading for ");
    Serial.print(knownWeights[i]);
    Serial.print("g: ");
    Serial.println(reading);
  }
  return (scaleFactors[0] + scaleFactors[1] + scaleFactors[2] + scaleFactors[3]) / 4.0;

  // For single weight (978.0g), uncomment below and comment out above:
  /*
  float knownWeight = 978.0;
  long zeroOffset = scale.read_average(10);
  Serial.print("Place weight: "); Serial.println(knownWeight);
  waitForUserConfirmation();
  long reading = scale.read_average(10);
  float scaleFactor = (float)(reading - zeroOffset) / knownWeight;
  Serial.print("Raw reading for ");
  Serial.print(knownWeight);
  Serial.print("g: ");
  Serial.println(reading);
  return scaleFactor;
  */
}

void validateCalibration(float knownWeight) {
  Serial.println("Validating calibration...");
  Serial.print("Place known weight ("); 
  Serial.print(knownWeight); 
  Serial.println("g) for validation.");
  waitForUserConfirmation();
  float measured = weightValAvg();
  if (abs(measured - knownWeight) / knownWeight <= 0.01) {
    Serial.println("Calibration successful!");
  } else {
    Serial.print("Calibration error: Measured ");
    Serial.print(measured);
    Serial.print("g, expected ");
    Serial.print(knownWeight);
    Serial.println("g.");
  }
}

long calibrateWeight(int plate) {
  if (plate < 1 || plate > 5) {
    Serial.println("Error: Plate must be 1-5");
    return 0;
  }
  if (scale.is_ready()) {
    Serial.println("Taring for plate " + String(plate) + "... Remove all weights (leave plate)");
    waitForUserConfirmation();
    scale.tare(20);
    long zeroOffset = scale.read_average(10);
    Serial.print("Calculated ZERO_OFFSET: ");
    Serial.println(zeroOffset);

    Serial.println("Calculating SCALE_FACTOR with multiple weights...");
    float scaleFactor = calculateScaleFactor();

    Serial.println("======================");
    Serial.print("Final ZERO_OFFSET: ");
    Serial.println(zeroOffset);
    Serial.print("Final SCALE_FACTOR: ");
    Serial.println(scaleFactor, 6);
    Serial.println("======================");

    ZERO_OFFSET = zeroOffset;
    SCALE_FACTOR = scaleFactor;
    saveCalibration(zeroOffset, scaleFactor);
    calibrated = true;

    balancePt[plate-1] = weightValAvg(); // Set balancePt for selected plate
    saveBalancePts();
    currentPlate = plate; // Update current plate
    Serial.println("New balancePt for plate " + String(plate) + " (g): " + String(balancePt[plate-1]));

    validateCalibration(978.0);
    return scale.read_average(10);
  } else {
    Serial.println("HX711 not found.");
    return 0;
  }
}

void printCalibration() {
  Serial.println("ZERO_OFFSET: " + String(ZERO_OFFSET));
  Serial.println("SCALE_FACTOR: " + String(SCALE_FACTOR));
  Serial.println("calibrated: " + String(calibrated));
  for (int i = 0; i < 5; i++) {
    Serial.println("balancePt[" + String(i+1) + "] (g): " + String(balancePt[i]));
  }
}