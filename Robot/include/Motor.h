#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

int GPIOSETUP();

class Motor
{
public:
    int motorStopValue = 0;
    int motorMaxValue = 255;
    const static int ModeDigital = 0; // zero or full power
    const static int ModePwm = 1; // soft PWM
    int motorMode = 1;
    Motor(int,int,int);
    Motor();
    void setForward(uint8_t);
    void setBackward(uint8_t);
    void stop();
    virtual ~Motor();
protected:
private:
    int pinForward = -1;
    int pinBackward = -1;
};

#endif // MOTOR_H
