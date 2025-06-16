void motorRotateFunc(int en){
  if(en!=curMotorState){
    if(en==0){
      digitalWrite(motorCtrlPin,LOW);
      curMotorState = en;
    }
    else{
      digitalWrite(motorCtrlPin,HIGH);
      curMotorState = en;
    }
  }
  Serial.println("motor state:"+String(curMotorState));
}
