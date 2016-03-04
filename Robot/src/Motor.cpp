#include "Motor.h"

#include "wiringPi.h"

int GPIOSETUP()
{
    return wiringPiSetupGpio();
}

// Motor

Motor::Motor(int pf, int pb)
{
    this->pinForward = pf;
    this->pinBackward = pb;
    pinMode(this->pinForward, PWM_OUTPUT);
    delay(100);
    pinMode(this->pinBackward, PWM_OUTPUT);
    delay(100);
    pwmWrite(this->pinForward, motorStopValue);
    pwmWrite(this->pinBackward, motorStopValue);

}

void Motor::setForward(int v)
{
    pwmWrite(this->pinBackward, motorStopValue);
    pwmWrite(this->pinForward, v);
}

void Motor::setBackward(int v)
{
    pwmWrite(this->pinForward, motorStopValue);
    pwmWrite(this->pinBackward, v);
}

Motor::~Motor()
{

}
