#ifndef ULTRASONICSENSOR_H
#define ULTRASONICSENSOR_H

#include <stdint.h>

class UltraSonicSensor
{
    public:
        uint64_t lastSend=-1; // Mills
        int signalPin = -1;
        int recivePin = -1;
        // [signal/trigger pin, recive/echo pin]
        UltraSonicSensor(int, int);
        void send();
        void recive(uint64_t*);
        virtual ~UltraSonicSensor();
    protected:
    private:

};

#endif // ULTRASONICSENSOR_H
