int inches = 0;
int cm1 = 0;
int cm2 = 0;
boolean motorLastState = 0;



long readUltrasonicDistance(int triggerPin, int echoPin)
{
  pinMode(triggerPin, OUTPUT);  // Clear the trigger
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  // Sets the trigger pin to HIGH state for 10 microseconds
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  // Reads the echo pin, and returns the sound wave travel time in microseconds
  return pulseIn(echoPin, HIGH);
}

void startRotate(){
  runRotate(true);
}

void rotateNext(){
  // measure the ping time in cm
  
  int a = 0.01723 * readUltrasonicDistance(ut2, ue2);
  delay(50);
  int b = 0.01723 * readUltrasonicDistance(ut1, ue1);
  delay(50);
  digitalWrite(motorCtrlPin,LOW);
  if(a<=30 & b<=30){
    Serial.println("att");
    delay(1100);
    digitalWrite(motorCtrlPin,HIGH);
    scale.tare(20);
    balancePt = weightValAvg();
    // delay(2000);
    digitalWrite(motorCtrlPin,LOW);
  }
  Serial.println("start rotate");
  boolean isRunning = true;
  while (isRunning==true){
    int cm11 = 0.01723 * readUltrasonicDistance(ut1, ue1);
    delay(50);
    int cm21 = 0.01723 * readUltrasonicDistance(ut2, ue2);
    delay(50);
      if (cm21<=30){
        // delay(50);
        if (cm11<=30){
          digitalWrite(motorCtrlPin,HIGH);
          isRunning = false;  
        }
        else{
          digitalWrite(motorCtrlPin,LOW);
        }
      // if (cm2<=25){
        // Serial.println(cm21);
        // Serial.println("not run");
        // runRotate(false
        // delay(200);
        
        // break;
      }
      else{
        // Serial.println(cm1);
        // runRotate(true);
        // Serial.println("run");
        digitalWrite(motorCtrlPin,LOW);
      }
      // delay(100); // Wait for 100 millisecond(s)
  }
  Serial.println("end while");
}

void translateDistance(){
  // measure the ping time in cm
  cm1 = 0.01723 * readUltrasonicDistance(ut1, ue1);
  delay(100);
  cm2 = 0.01723 * readUltrasonicDistance(ut2, ue2);
  // if (cm1>=25 & cm2>=25){
  if (cm2>=25){
    Serial.println("stop");
    Serial.println("near camera");
    digitalWrite(motorCtrlPin,HIGH);
    // delay(5000);
  }
  else{
    Serial.println("run");
    digitalWrite(motorCtrlPin,LOW);
  }
  // Serial.print(cm1);
  // Serial.println("cm1");
  // Serial.print(cm2);
  // Serial.println("cm2");,
  delay(100); // Wait for 100 millisecond(s)
}

void runRotate(boolean en) {
  if (en!=motorLastState){
    if (en==true){
      Serial.println("run");
      digitalWrite(motorCtrlPin,LOW);
    }
    else{
      Serial.println("stop");
      digitalWrite(motorCtrlPin,HIGH);
    }
    motorLastState = en;
  }
}
