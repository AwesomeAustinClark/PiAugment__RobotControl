#ifndef MOTOR_H
#define MOTOR_H

static int GPIOSETUP();

class Motor
{
public:
    static const int motorStopValue = 0;
    Motor(int,int);
    void setForward(int);
    void setBackward(int);
    virtual ~Motor();
protected:
private:
    int pinForward = -1;
    int pinBackward = -1;
};

#endif // MOTOR_H
