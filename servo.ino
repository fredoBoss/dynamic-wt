#include <Servo.h>

extern Servo servoY1;
extern Servo servoY2;
extern Servo servoY3;
extern Servo servoY4;
extern Servo servoY5;
extern Servo servoY6;
extern int pos;
extern int degF;
extern int degR;
extern int rotateDelay;
extern int servoValRot2;
extern int servoValRot1;
extern int servoValStop;

void setupServo() {
  servoY1.attach(5);
  servoY2.attach(4);
  servoY3.attach(6);
  servoY4.attach(8);
  servoY5.attach(9);
  servoY6.attach(7);
  delay(15);
  stopallServo();
  delay(15);
}

void stopallServo() {
  servoY1.write(90);
  servoY2.write(90);
  servoY3.write(90);
  servoY4.write(90);
  servoY5.write(90);
  servoY6.write(90);
}

void initPosServo() {
  servoY1.write(servoValRot1);
  delay(rotateDelay);
  servoY1.write(90);
  servoY2.write(servoValRot1);
  delay(rotateDelay);
  servoY2.write(90);
  servoY3.write(90);
  servoY4.write(servoValRot1);
  delay(rotateDelay);
  servoY4.write(90);
  servoY5.write(servoValRot1);
  delay(rotateDelay);
  servoY5.write(90);
  servoY6.write(servoValRot1);
  delay(rotateDelay);
  servoY6.write(90);
}

void trayPos(int sortResult) {
  Serial.println("trayPos:" + String(sortResult));
  switch (sortResult) {
    case 1:
      digitalWrite(motorCtrlPin, LOW);
      delay(4600);
      digitalWrite(motorCtrlPin, HIGH);
      delay(1000);
      servoY1.write(servoValRot2);
      delay(rotateDelay);
      servoY1.write(servoValRot1);
      delay(rotateDelay);
      servoY1.write(90);
      break;
    case 2:
      digitalWrite(motorCtrlPin, LOW);
      delay(6500);
      digitalWrite(motorCtrlPin, HIGH);
      delay(1000);
      servoY2.write(servoValRot2);
      delay(rotateDelay);
      servoY2.write(servoValRot1);
      delay(rotateDelay);
      servoY2.write(90);
      break;
    case 3:
      digitalWrite(motorCtrlPin, LOW);
      delay(6900);
      digitalWrite(motorCtrlPin, HIGH);
      delay(1000);
      servoY3.write(servoValRot2);
      delay(rotateDelay);
      servoY3.write(servoValRot1);
      delay(rotateDelay);
      servoY3.write(90);
      break;
    case 4:
      digitalWrite(motorCtrlPin, LOW);
      delay(8800);
      digitalWrite(motorCtrlPin, HIGH);
      delay(1000);
      servoY4.write(servoValRot2);
      delay(rotateDelay);
      servoY4.write(servoValRot1);
      delay(rotateDelay);
      servoY4.write(90);
      break;
    case 5:
      digitalWrite(motorCtrlPin, LOW);
      delay(11000);
      digitalWrite(motorCtrlPin, HIGH);
      delay(1000);
      servoY5.write(servoValRot2);
      delay(rotateDelay);
      servoY5.write(servoValRot1);
      delay(rotateDelay);
      servoY5.write(90);
      break;
    case 6:
      digitalWrite(motorCtrlPin, LOW);
      delay(12000);
      digitalWrite(motorCtrlPin, HIGH);
      delay(1000);
      servoY6.write(servoValRot2);
      delay(rotateDelay);
      servoY6.write(servoValRot1);
      delay(rotateDelay);
      servoY6.write(90);
      break;
  }
  stopallServo();
  delay(rotateDelay);
  Serial.println("Rotate done");
}

void testServo() {
  delay(rotateDelay);
  for (pos = 0; pos <= degF; pos += 1) {
    servoY1.write(pos);
    delay(15);
  }
  delay(rotateDelay);
  for (pos = degR; pos >= 0; pos -= 1) {
    servoY1.write(pos);
    delay(15);
  }
}