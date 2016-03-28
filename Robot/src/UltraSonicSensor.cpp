#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include "wiringPi.h"
#include "UltraSonicSensor.h"
#include "Global.h"

UltraSonicSensor::UltraSonicSensor(int sigPin,int recvPin)
{
    this->signalPin = sigPin;
    this->recivePin = recvPin;
    pinMode(this->signalPin, OUTPUT);
    pinMode(this->recivePin, INPUT);
    pullUpDnControl(this->recivePin, PUD_DOWN);
    digitalWrite(this->recivePin, LOW);
}

void UltraSonicSensor::send(){
    digitalWrite(this->signalPin, HIGH);
    delay(10);
    digitalWrite(this->signalPin, LOW);
    getTimeOfDayMills64(&this->lastSend);
}

void UltraSonicSensor::recive(uint64_t* rt){
    uint64_t time = 0;
    *rt = 0;
    getTimeOfDayMills64(&time);
    if(digitalRead(this->recivePin)==true){
        *rt = time-this->lastSend;
        //std::cout << ": " << *rt << std::endl;
    }else{
        //std::cout << "NOTHING" << std::endl;
        *rt = -1;
    }
}

UltraSonicSensor::~UltraSonicSensor()
{
    //dtor
}
