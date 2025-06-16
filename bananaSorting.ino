#include <Servo.h>
#include "HX711.h"

#define loadcellDout 2
#define loadcellSck 3

#define motorCtrlPin 15
#define ue1 11
#define ut1 12

#define ut2 13
#define ue2 46

float balancePt = 0;
const float plateWeight = 251.09;

Servo servoY1;
Servo servoY2;
Servo servoY3;
Servo servoY4;
Servo servoY5;
Servo servoY6;

HX711 scale;

String readStr="";
bool calibrated = true;
int curMotorState = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  initLoadCell();
  setupServo();
  pinMode(motorCtrlPin,OUTPUT);
  digitalWrite(motorCtrlPin,HIGH); //active low
  Serial.println("starting");
  // initPosServo();
  // motorRotateFunc(1);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Serial.println(weightVal());
  // translateDistance();
  // digitalWrite(motorCtrlPin,LOW); //active low
  // Serial.println("low");
  // delay(5000);
  // digitalWrite(motorCtrlPin,HIGH); //active low
  // Serial.println("high");
  // delay(5000);
  mainLoop();
  // startRotate();
  // rotateNext();
  // delay(1000);
}

void mainLoop(){
  while (Serial.available()){
    readStr = Serial.readString();
    if (readStr.indexOf("readWt:")>=0){  //read weight command
      Serial.println("readWt:"+String(weightValAvg()-balancePt-plateWeight));
    }
    else if (readStr.indexOf("trayPos:")>=0){  //command for servo
      String trayPosVal = readStr.substring(8);
      trayPos(trayPosVal.toInt());
      Serial.println("trayPos:"+trayPosVal);
    }
    // else if (readStr.indexOf("mtrCtrl:")>=0){
    //   String val = readStr.substring(8);
    //   motorRotateFunc(val.toInt());
    //   Serial.println("mtrCtrl:"+val);
    // }
    else if (readStr.indexOf("next:")>=0){
      // String val = readStr.substring(8);
      rotateNext();
    }
    else if (readStr.indexOf("tare:")>=0){
      // String val = readStr.substring(8);
      digitalWrite(motorCtrlPin,LOW);
      delay(400);
      scale.tare(20);
      delay(100);
      digitalWrite(motorCtrlPin,HIGH);
    }
    else return;
  }
  readStr="";
    // Serial.println("motor state:"+String(curMotorState));
}
