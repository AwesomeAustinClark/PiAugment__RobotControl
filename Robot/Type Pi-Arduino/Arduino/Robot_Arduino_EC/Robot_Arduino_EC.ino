#include <Servo.h>

// vars
Servo servos[3];
const int US = 4;
const int echoPin[US] = {2,4,6,8};
const int trigPin[US] = {3,5,7,12};
const int servoPins[3] = {9,10,11};
int pos = 0;
int input = 0;
bool values[5];
long inches, cm;
// vars

long microsecondsToInches(long microseconds) {
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

long getDuration(int tp, int ep){
  //long duration = 0;
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(tp, LOW);
  delayMicroseconds(2);
  digitalWrite(tp, HIGH);
  delayMicroseconds(5);
  digitalWrite(tp, LOW);
  //duration = pulseIn(echoPin, HIGH, 10000);
  return pulseIn(ep, HIGH, 10000);
}


void setup()
{
    Serial.begin(9600);
    for(int i=0;i<US;++i){
      pinMode(trigPin[i], OUTPUT);
      pinMode(echoPin[i], INPUT);
    }
    servos[0].attach(servoPins[0]);
    servos[1].attach(servoPins[1]);
    servos[2].attach(servoPins[2]);
    servos[0].write(0); 
    servos[1].write(0); 
    servos[2].write(0); 
}

long USDuration;
void loop()
{
  //USDuration = getDuration(trigPin[0], echoPin[0]);
  //values[0]=((USDuration=getDuration(trigPin[0], echoPin[0]))>0 &&  microsecondsToInches(USDuration)<=6);
  for(int i=0;i<US;++i){
    values[i]=((USDuration=getDuration(trigPin[i], echoPin[i]))>0 &&  microsecondsToInches(USDuration)<=3);  
  }
  //cm = microsecondsToCentimeters(duration);
  //myservo.write(pos);
  if(Serial.available()>4)
  {
    input=Serial.read();
    if(input=='g')
    {
      Serial.print(values[0]);
      Serial.print(values[1]);
      Serial.print(values[2]);
      Serial.print(values[3]);
      Serial.print(values[4]);
      Serial.print(';');
    }
    for(int i=0;i<3;++i){
      input=Serial.read();
      if(input=='q'){
        servos[i].write(0);
      }else if(input=='w'){
        servos[i].write(30);
      }else if(input=='e'){
        servos[i].write(60);
      }else if(input=='r'){
        servos[i].write(90);
      }else if(input=='t'){
        servos[i].write(120);
      }else if(input=='y'){
        servos[i].write(150);
      }else if(input=='u'){
        servos[i].write(170);
      }else if(input=='i'){
        servos[i].write(180);
      }
    }
  }
}
