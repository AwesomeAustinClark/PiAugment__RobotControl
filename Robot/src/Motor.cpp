#include "Motor.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include "wiringPi.h"
#include "softPwm.h"



int GPIOSETUP()
{

    return wiringPiSetupGpio();
}

Motor::Motor()
{
}

Motor::Motor(int pf, int pb, int mode)
{
    this->pinForward = pf;
    this->pinBackward = pb;
    this->motorMode = mode;
    switch(this->motorMode)
    {
    case Motor::ModeDigital:
        pinMode(this->pinForward, OUTPUT);
        delay(100);
        pinMode(this->pinBackward, OUTPUT);
        delay(100);
        this->stop();
        break;
    case Motor::ModePwm:
        softPwmCreate(this->pinForward, motorStopValue, motorMaxValue);
        softPwmCreate(this->pinBackward, motorStopValue, motorMaxValue);
        break;
    case Motor::ModeAnalog:
        pinMode(this->pinForward, OUTPUT);
        delay(100);
        pinMode(this->pinBackward, OUTPUT);
        delay(100);
        break;
    }
    std::cout << "Done." << std::endl;

}

void Motor::stop()
{
    switch(this->motorMode)
    {
    case ModeDigital:
        digitalWrite(this->pinForward, motorStopValue);
        digitalWrite(this->pinBackward, motorStopValue);
        break;
    case ModePwm:
        softPwmWrite(this->pinBackward, motorStopValue);
        softPwmWrite(this->pinForward, motorStopValue);
        break;
    case ModeAnalog:
        analogWrite(this->pinBackward, motorStopValue);
        analogWrite(this->pinForward, motorStopValue);
        break;
    }

}

void Motor::setForward(uint8_t v)
{
    switch(this->motorMode)
    {
    case ModeDigital:
        digitalWrite(this->pinBackward, motorStopValue);
        delay(1);
        digitalWrite(this->pinForward, 1);
        break;
    case ModePwm:
        softPwmWrite(this->pinBackward, motorStopValue);
        softPwmWrite(this->pinForward, v);
        break;
    case ModeAnalog:
        analogWrite(this->pinBackward, motorStopValue);
        analogWrite(this->pinForward, v);
        break;
    }

}

void Motor::setBackward(uint8_t v)
{
    switch(this->motorMode)
    {
    case ModeDigital:
        digitalWrite(this->pinForward, motorStopValue);
        delay(1);
        digitalWrite(this->pinBackward, 1);
        break;
    case ModePwm:
        softPwmWrite(this->pinForward, motorStopValue);
        softPwmWrite(this->pinBackward, v);
        break;
    case ModeAnalog:
        analogWrite(this->pinForward, motorStopValue);
        analogWrite(this->pinBackward, v);
        break;
    }

}

Motor::~Motor()
{

}
