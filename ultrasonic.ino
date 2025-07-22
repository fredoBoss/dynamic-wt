#include <HX711.h>

extern HX711 scale;
extern float balancePt[5];
extern int currentPlate;
extern boolean motorLastState;

int inches = 0;
int cm1 = 0;
int cm2 = 0;

long readUltrasonicDistance(int triggerPin, int echoPin) {
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin, HIGH);
}

void startRotate() {
  runRotate(true);
}

void rotateNext1() {
  int a = 0.01723 * readUltrasonicDistance(ut2, ue2);
  delay(50);
  int b = 0.01723 * readUltrasonicDistance(ut1, ue1);
  delay(50);
  digitalWrite(motorCtrlPin, LOW);
  delay(300);
  if (a <= 5 && b <= 5) {
    Serial.println("att");
    delay(800);
    digitalWrite(motorCtrlPin, HIGH);
    scale.tare(20);
    currentPlate = (currentPlate % 5) + 1; // Cycle through plates 1–5
    // balancePt[currentPlate-1] = weightValAvg();
    // saveBalancePts();
    // Serial.println("New balancePt for plate " + String(currentPlate) + " (g): " + String(balancePt[currentPlate-1]));
    delay(4000);
    digitalWrite(motorCtrlPin, LOW);
  }
  Serial.println("start rotate");
  boolean isRunning = true;
  while (isRunning) {
    Serial.println("rotating");
    int cm11 = 0.01723 * readUltrasonicDistance(ut1, ue1);
    delay(50);
    int cm21 = 0.01723 * readUltrasonicDistance(ut2, ue2);
    delay(50);
    if (cm21 <= 3) {
      if (cm11 <= 3) {
        digitalWrite(motorCtrlPin, HIGH);
        isRunning = false;
        scale.tare(20);
        currentPlate = (currentPlate % 5) + 1; // Cycle through plates
        // balancePt[currentPlate-1] = weightValAvg();
        // saveBalancePts();
        // Serial.println("New balancePt for plate " + String(currentPlate) + " (g): " + String(balancePt[currentPlate-1]));
      } else {
        digitalWrite(motorCtrlPin, LOW);
      }
    } else {
      digitalWrite(motorCtrlPin, LOW);
    }
  }
  Serial.println("end while");
}

void rotateNext() {
  Serial.println("start rotate");
  digitalWrite(motorCtrlPin, LOW);
  delay(700);
  digitalWrite(motorCtrlPin, HIGH);
  delay(3000);
  digitalWrite(motorCtrlPin, LOW);
  boolean isRunning = true;
  int cm11 = 10;
  int cm21 = 10;
  while (isRunning) {
    Serial.println("rotating");
    cm21 = 0.01723 * readUltrasonicDistance(ut2, ue2);
    delay(50);
    if (cm21 <= 10) {
      digitalWrite(motorCtrlPin, HIGH);
      cm11 = 0.01723 * readUltrasonicDistance(ut1, ue1);
      delay(50);
      digitalWrite(motorCtrlPin, LOW);
      delay(100);
      if (cm11 <= 10) {
        digitalWrite(motorCtrlPin, HIGH);
        isRunning = false;
        scale.tare(20);
        currentPlate = (currentPlate % 5) + 1; // Cycle through plates
        // balancePt[currentPlate-1] = weightValAvg();
        // saveBalancePts();
        // Serial.println("New balancePt for plate " + String(currentPlate) + " (g): " + String(balancePt[currentPlate-1]));
      } else {
        digitalWrite(motorCtrlPin, LOW);
      }
    } else {
      digitalWrite(motorCtrlPin, LOW);
    }
  }
  Serial.println("end while");
}

void translateDistance() {
  cm1 = 0.01723 * readUltrasonicDistance(ut1, ue1);
  delay(100);
  cm2 = 0.01723 * readUltrasonicDistance(ut2, ue2);
  if (cm2 >= 25) {
    Serial.println("stop");
    Serial.println("near camera");
    digitalWrite(motorCtrlPin, HIGH);
  } else {
    Serial.println("run");
    digitalWrite(motorCtrlPin, LOW);
  }
  delay(100);
}

void runRotate(boolean en) {
  if (en != motorLastState) {
    if (en) {
      Serial.println("run");
      digitalWrite(motorCtrlPin, LOW);
    } else {
      Serial.println("stop");
      digitalWrite(motorCtrlPin, HIGH);
    }
    motorLastState = en;
  }
}