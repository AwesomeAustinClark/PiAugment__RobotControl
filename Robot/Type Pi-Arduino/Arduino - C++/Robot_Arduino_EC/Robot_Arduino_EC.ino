

#include <Servo.h>

// vars
Servo servo;
const int echoPin = 2;
const int trigPin = 3;
const int servoPin = 9;
int pos = 0;
int input = 0;
bool values[5];
long duration, inches, cm;
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


void setup()
{
    Serial.begin(9600);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    servo.attach(servoPin);  // attaches the servo on pin 9 to the servo object
    servo.write(0); 
}

unsigned long lastSend;
void loop()
{
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH, 10000);
  inches = microsecondsToInches(duration);
  values[0]=(duration>0 &&  inches<=6);
  //cm = microsecondsToCentimeters(duration);
  //myservo.write(pos);
  if(Serial.available()>0)
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
    }else if(input=='q'){
      servo.write(0);
    }else if(input=='w'){
      servo.write(30);
    }else if(input=='e'){
      servo.write(60);
    }else if(input=='r'){
      servo.write(90);
    }else if(input=='t'){
      servo.write(120);
    }else if(input=='y'){
      servo.write(150);
    }else if(input=='u'){
      servo.write(180);
      Serial.print(';');
    }
  }
}
