#include <Servo.h>
#include <HX711.h>

#define loadcellDout 2
#define loadcellSck 3
#define motorCtrlPin 15
#define ue1 11
#define ut1 12
#define ut2 13
#define ue2 46

float balancePt[5] = {0, 0, 0, 0, 0}; // Array for five plates
int currentPlate = 1; // Default to plate 1 (1–5)

Servo servoY1;
Servo servoY2;
Servo servoY3;
Servo servoY4;
Servo servoY5;
Servo servoY6;

HX711 scale;

String readStr = "";
bool calibrated = false; // Start uncalibrated
int curMotorState = 0;

int pos = 0;
int degF = 90;
int degR = 90;
int rotateDelay = 750;
int servoValRot2 = 180;
int servoValRot1 = 0;
int servoValStop = 90;
boolean motorLastState = 0;

void saveBalancePts(); // Forward declaration

void setup() {
  Serial.begin(9600);
  initLoadCell();
  setupServo();
  pinMode(motorCtrlPin, OUTPUT);
  digitalWrite(motorCtrlPin, HIGH); // Active low
  Serial.println("starting");
  // initPosServo();
  // motorRotateFunc(1);
  Serial.println("Starting. Send 'calibrate:<1-5>' to calibrate plate, 'readWt:' to read weight, 'tare:<1-5>' to tare plate, 'setPlate:<1-5>' to select plate, 'trayPos:<1-6>', 'next:', 'next1:', or 'printCal:' to check calibration.");
}

void loop() {
  // Serial.println(weightVal());
  // translateDistance();
  // digitalWrite(motorCtrlPin, LOW); //active low
  // Serial.println("low");
  // delay(5000);
  // digitalWrite(motorCtrlPin, HIGH); //active low
  // Serial.println("high");
  // delay(5000);
  mainLoop();
  // startRotate();
  // rotateNext();
  // delay(1000);
}

void mainLoop() {
  while (Serial.available()) {
    readStr = Serial.readString();
    if (readStr.indexOf("readWt:") >= 0) {
      if (!calibrated) {
        Serial.println("Error: Run 'calibrate:<1-5>' for plate " + String(currentPlate) + " first!");
      } else {
        float rawWeight = weightValAvg();
        Serial.println("Raw weight (g): " + String(rawWeight));
        Serial.println("balancePt for plate " + String(currentPlate) + " (g): " + String(balancePt[currentPlate-1]));
        Serial.println("readWt:" + String(rawWeight - balancePt[currentPlate-1]));
      }
    }
    else if (readStr.indexOf("trayPos:") >= 0) {
      String trayPosVal = readStr.substring(8);
      trayPos(trayPosVal.toInt());
      Serial.println("trayPos:" + trayPosVal);
    }
    else if (readStr.indexOf("tare:") >= 0) {
      String plateStr = readStr.substring(5);
      int plate = plateStr.toInt();
      if (plate < 1 || plate > 5) {
        Serial.println("Error: Plate must be 1-5");
        return;
      }
      digitalWrite(motorCtrlPin, LOW);
      delay(400);
      scale.tare(20);
      balancePt[plate-1] = weightValAvg(); // Set balancePt for selected plate
      saveBalancePts(); // Save to EEPROM
      currentPlate = plate; // Update current plate
      delay(100);
      digitalWrite(motorCtrlPin, HIGH);
      Serial.println("Tare done for plate " + String(plate));
      Serial.println("New balancePt for plate " + String(plate) + " (g): " + String(balancePt[plate-1]));
    }
    else if (readStr.indexOf("calibrate:") >= 0) {
      String plateStr = readStr.substring(10);
      int plate = plateStr.toInt();
      if (plate < 1 || plate > 5) {
        Serial.println("Error: Plate must be 1-5");
        return;
      }
      calibrateWeight(plate);
      Serial.println("Calibration complete for plate " + String(plate));
    }
    else if (readStr.indexOf("setPlate:") >= 0) {
      String plateStr = readStr.substring(9);
      int plate = plateStr.toInt();
      if (plate < 1 || plate > 5) {
        Serial.println("Error: Plate must be 1-5");
        return;
      }
      currentPlate = plate;
      Serial.println("Selected plate " + String(currentPlate));
    }
    else if (readStr.indexOf("next:") >= 0) {
      rotateNext();
    }
    else if (readStr.indexOf("next1:") >= 0) {
      rotateNext1();
    }
    else if (readStr.indexOf("printCal:") >= 0) {
      printCalibration();
    }
    else if (readStr.indexOf("clearEEPROM:") >= 0) {
      clearEEPROM();
    }
    else return;
  }
  readStr = "";
  // Serial.println("motor state:" + String(curMotorState));
}