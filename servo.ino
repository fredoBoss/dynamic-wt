int pos = 0;    // variable to store the servo position

int degF = 90;
int degR = 90;
int rotateDelay = 750;

int servoValRot2 = 180;
int servoValRot1 =0;
int servoValStop = 90;

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

void stopallServo(){
  servoY1.write(90);
  servoY2.write(90);
  servoY3.write(90);
  servoY4.write(90);
  servoY5.write(90);
  servoY6.write(90);
}

void initPosServo(){
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

void trayPos(int sortResult){
  Serial.println("trayPos:"+String(sortResult));
  switch(sortResult){
    case 1:
      digitalWrite(motorCtrlPin,LOW);
      delay(4600); //travel time from start point to servo 1
      digitalWrite(motorCtrlPin,HIGH);
      delay(1000);
      servoY1.write(servoValRot2);
      delay(rotateDelay);
      servoY1.write(servoValRot1);
      delay(rotateDelay);
      servoY1.write(90);
      break;
    case 2:
      digitalWrite(motorCtrlPin,LOW);
      delay(5600);
      digitalWrite(motorCtrlPin,HIGH);
      delay(1000);
      servoY2.write(servoValRot2);
      delay(rotateDelay);
      servoY2.write(servoValRot1);
      delay(rotateDelay);
      servoY2.write(90);
      break;
    case 3:
      digitalWrite(motorCtrlPin,LOW);
      delay(6900);
      digitalWrite(motorCtrlPin,HIGH);
      delay(1000);
      servoY3.write(servoValRot2);
      delay(rotateDelay);
      servoY3.write(servoValRot1);
      delay(rotateDelay);
      servoY3.write(90);
      break;
    case 4:
      digitalWrite(motorCtrlPin,LOW);
      delay(8800);
      digitalWrite(motorCtrlPin,HIGH);
      delay(1000);
      servoY4.write(servoValRot2);
      delay(rotateDelay);
      servoY4.write(servoValRot1);
      delay(rotateDelay);
      servoY4.write(90);
      break;
    case 5:
      digitalWrite(motorCtrlPin,LOW);
      delay(1000);
      digitalWrite(motorCtrlPin,HIGH);
      delay(1000);
      servoY5.write(servoValRot2);
      delay(rotateDelay);
      servoY5.write(servoValRot1);
      delay(rotateDelay);
      servoY5.write(90);
      break;  
    case 6:
      digitalWrite(motorCtrlPin,LOW);
      delay(11600);
      digitalWrite(motorCtrlPin,HIGH);
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

void testServo(){
      delay(rotateDelay);
      for (pos = 0; pos <= degF; pos += 1) { // goes from 0 degrees to 180 degrees
        // in steps of 1 degree
        servoY1.write(pos);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15 ms for the servo to reach the position
      }
      delay(rotateDelay);
        for (pos = degR; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
        servoY1.write(pos);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15 ms for the servo to reach the position
      }
}
