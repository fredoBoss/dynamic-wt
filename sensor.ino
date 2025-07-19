#include <EEPROM.h>
#include <HX711.h>

extern bool calibrated;
extern HX711 scale;
extern float balancePt[5];
extern int currentPlate;

float ZERO_OFFSET = 0.0;
float SCALE_FACTOR = 1.0;
const float BALANCE_PT_OFFSET = 815.46; // Based on prior validation error (1793.46g - 978g)

void clearEEPROM() {
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0);
  }
  ZERO_OFFSET = 0.0;
  SCALE_FACTOR = 1.0;
  calibrated = false;
  Serial.println("EEPROM cleared, calibration reset");
}

void saveBalancePts() {
  int offset = sizeof(float) * 2;
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
      balancePt[i] = 0.0;
    }
  }
  Serial.println("balancePt values loaded from EEPROM");
  for (int i = 0; i < 5; i++) {
    Serial.println("balancePt[" + String(i+1) + "] (g): " + String(balancePt[i], 2));
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
  balancePt[currentPlate-1] = weightValAvg();
  saveBalancePts();
  Serial.println("Initial balancePt for plate " + String(currentPlate) + " (g): " + String(balancePt[currentPlate-1], 2));
}

void saveCalibration(float zeroOffset, float scaleFactor) {
  if (isnan(zeroOffset) || isnan(scaleFactor) || scaleFactor > 0.02 || scaleFactor < 0.00005) {
    Serial.println("Error: Invalid calibration values, not saving. ZERO_OFFSET=" + String(zeroOffset, 2) + ", SCALE_FACTOR=" + String(scaleFactor, 6));
    calibrated = false;
    return;
  }
  int retries = 3;
  bool writeSuccess = false;
  while (retries > 0 && !writeSuccess) {
    EEPROM.put(0, zeroOffset);
    EEPROM.put(sizeof(float), scaleFactor);
    float tempZeroOffset, tempScaleFactor;
    EEPROM.get(0, tempZeroOffset);
    EEPROM.get(sizeof(float), tempScaleFactor);
    if (tempZeroOffset == zeroOffset && tempScaleFactor == scaleFactor) {
      writeSuccess = true;
      ZERO_OFFSET = zeroOffset;
      SCALE_FACTOR = scaleFactor;
      calibrated = true;
      Serial.println("Calibration saved to EEPROM: ZERO_OFFSET=" + String(zeroOffset, 2) + ", SCALE_FACTOR=" + String(scaleFactor, 6));
    } else {
      Serial.println("EEPROM write attempt " + String(4 - retries) + " failed. Retrying...");
      retries--;
      delay(100);
    }
  }
  if (!writeSuccess) {
    Serial.println("Error: EEPROM write verification failed after 3 attempts. ZERO_OFFSET=" + String(zeroOffset, 2) + ", SCALE_FACTOR=" + String(scaleFactor, 6));
    calibrated = false;
  }
}

void loadCalibration() {
  EEPROM.get(0, ZERO_OFFSET);
  EEPROM.get(sizeof(float), SCALE_FACTOR);
  if (isnan(ZERO_OFFSET) || isnan(SCALE_FACTOR) || ZERO_OFFSET == 0.0 || SCALE_FACTOR == 0.0 || SCALE_FACTOR > 0.02 || SCALE_FACTOR < 0.00005) {
    Serial.println("No valid calibration in EEPROM, using defaults.");
    ZERO_OFFSET = 0.0;
    SCALE_FACTOR = 1.0;
    calibrated = false;
  } else {
    calibrated = true;
    Serial.println("Calibration loaded: ZERO_OFFSET=" + String(ZERO_OFFSET, 2) + ", SCALE_FACTOR=" + String(SCALE_FACTOR, 6));
  }
}

float convertToWeight(long raw) {
  return (raw - ZERO_OFFSET) * SCALE_FACTOR;
}

float weightValAvg() {
  delay(4000); // Increased to 4 seconds for stability
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
    float weight = convertToWeight(raw) - BALANCE_PT_OFFSET;
    if (currentPlate > 0 && currentPlate <= 5) {
      weight -= balancePt[currentPlate-1];
    }
    return weight;
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
  float knownWeight = 978.0;
  long zeroOffset = scale.read_average(10);
  Serial.println("Zero offset raw: " + String(zeroOffset));
  Serial.print("Place weight: "); Serial.println(knownWeight);
  waitForUserConfirmation();
  long reading = scale.read_average(10);
  float rawDiff = reading - zeroOffset;
  float scaleFactor = knownWeight / rawDiff;
  Serial.print("Raw reading for ");
  Serial.print(knownWeight);
  Serial.print("g: ");
  Serial.println(reading);
  Serial.print("Raw difference: ");
  Serial.println(rawDiff);
  Serial.print("Calculated scale factor: ");
  Serial.println(scaleFactor, 6);
  return scaleFactor;
}

void validateCalibration(float knownWeight) {
  Serial.println("Re-taring before validation...");
  scale.tare(20);
  long zeroOffset = scale.read_average(10);
  Serial.println("Validation ZERO_OFFSET: " + String(zeroOffset));
  Serial.println("Validating calibration...");
  Serial.print("Place known weight ("); 
  Serial.print(knownWeight); 
  Serial.println("g) for validation.");
  waitForUserConfirmation();
  float measured = weightValAvg();
  Serial.print("Validation measured: ");
  Serial.print(measured);
  Serial.println("g");
  if (abs(measured - knownWeight) / knownWeight <= 0.01) {
    Serial.println("Calibration successful!");
  } else {
    Serial.print("Calibration error: Measured ");
    Serial.print(measured);
    Serial.print("g, expected ");
    Serial.print(knownWeight);
    Serial.println("g.");
    calibrated = false;
  }
}

long calibrateWeight(int plate) {
  if (plate < 1 || plate > 5) {
    Serial.println("Error: Plate must be 1-5");
    return 0;
  }
  if (!scale.is_ready()) {
    Serial.println("HX711 not found.");
    return 0;
  }

  Serial.println("Taring for plate " + String(plate) + "... Remove all weights (leave plate)");
  waitForUserConfirmation();
  scale.tare(20);
  long zeroOffset = scale.read_average(10);
  Serial.println("Calculated ZERO_OFFSET: " + String(zeroOffset));

  Serial.println("Calculating SCALE_FACTOR with single weight...");
  float scaleFactor = calculateScaleFactor();

  if (scaleFactor > 0.02 || scaleFactor < 0.00005) {
    Serial.println("Error: Invalid SCALE_FACTOR: " + String(scaleFactor, 6));
    calibrated = false;
    return 0;
  }

  Serial.println("======================");
  Serial.println("Final ZERO_OFFSET: " + String(zeroOffset));
  Serial.println("Final SCALE_FACTOR: " + String(scaleFactor, 6));
  Serial.println("======================");

  saveCalibration(zeroOffset, scaleFactor);
  if (!calibrated) {
    Serial.println("Calibration save failed. Please retry.");
    return 0;
  }

  Serial.println("Setting balancePt for plate " + String(plate) + "... Remove all weights (leave plate)");
  waitForUserConfirmation();
  balancePt[plate-1] = weightValAvg();
  saveBalancePts();
  currentPlate = plate;
  Serial.println("New balancePt for plate " + String(plate) + " (g): " + String(balancePt[plate-1], 2));

  validateCalibration(978.0);
  if (!calibrated) {
    Serial.println("Calibration failed. Please retry.");
    return 0;
  }
  return scale.read_average(10);
}

void printCalibration() {
  Serial.println("ZERO_OFFSET: " + String(ZERO_OFFSET, 2));
  Serial.println("SCALE_FACTOR: " + String(SCALE_FACTOR, 6));
  Serial.println("calibrated: " + String(calibrated));
  for (int i = 0; i < 5; i++) {
    Serial.println("balancePt[" + String(i+1) + "] (g): " + String(balancePt[i], 2));
  }
}