const int motorPowerPins[2] = {10,11};
const int motorDirectionPins[4] = {A5,A0,A3,A4};
const int signalLostPins[2] = {A1,A2};
bool motorForward[2] = {true,true};

void setMotorsLeft(unsigned char value, boolean forward){
  analogWrite(motorPowerPins[0], value);
  digitalWrite(motorDirectionPins[0], forward);
  digitalWrite(motorDirectionPins[1], !forward);
}

void setMotorsRight(unsigned char value, boolean forward){
  analogWrite(motorPowerPins[1], value);
  digitalWrite(motorDirectionPins[2], forward);
  digitalWrite(motorDirectionPins[3], !forward);
}

void setup(){
  Serial.begin(115200);
  Serial.println("Seting up.");
  pinMode(motorPowerPins[0], OUTPUT);
  pinMode(motorPowerPins[1], OUTPUT);
  pinMode(motorDirectionPins[0], OUTPUT);
  pinMode(motorDirectionPins[1], OUTPUT);
  pinMode(signalLostPins[0], OUTPUT);
  pinMode(signalLostPins[1], OUTPUT);
  setMotorsRight(0,true);
  setMotorsLeft(0,true);
  Serial.println("Done.");
}

void loop(){
  if(Serial.available() > 4){
    unsigned char check1 = (unsigned char) Serial.read();
    unsigned char mod = (unsigned char) Serial.read();
    unsigned char lm = (unsigned char) Serial.read();
    unsigned char rm  = (unsigned char) Serial.read();
    unsigned char check2 = (unsigned char) Serial.read();
    /*
    Serial.print(check1);
    Serial.print(mod);
    Serial.print(lm);
    Serial.print(rm);
    Serial.println(check2);
    */
    if(check1=='[' && check2==']'){
      Serial.println("ACK ");
      setMotorsLeft(lm, bitRead(mod,0));
      setMotorsRight(rm, bitRead(mod,1)==false);
    }else{
      while(Serial.available() > 0 && Serial.read() != ']'){
      }
    }
  }
}

