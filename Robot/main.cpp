#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <cstdio>
#include <bitset>
#include <vector>

#include "Motor.h"
#include "roboRecive.h"


//http://hertaville.com/2012/11/18/introduction-to-accessing-the-raspberry-pis-gpio-in-c/

using namespace std;

bool inTerm = false;

void wait(){
    usleep(500000);
    usleep(500000);
    usleep(500000);
    usleep(500000);
    usleep(500000);

}

Motor frontLeft;
    Motor frontRight;
    Motor backLeft;
    Motor backRight;

void stopMotors(){
    frontLeft.stop();
    frontRight.stop();
    backLeft.stop();
    backRight.stop();
}

void setForward(){
    frontLeft.setForward(255);
    frontRight.setForward(255);
    backLeft.setForward(255);
    backRight.setForward(255);
}

int main (void)
{std::cout << (getuid()==0) << std::endl;
    GPIOSETUP();
    Motor frontLeft = Motor(5,6,Motor::ModePwm);
    Motor frontRight = Motor(24,25, Motor::ModePwm);
    Motor backLeft = Motor(3,2, Motor::ModePwm);
    Motor backRight = Motor(23,18, Motor::ModePwm);
    stopMotors();
    sleep(2);
    setForward();
    sleep(2);
    stopMotors();
    sleep(2);
    if(isatty(fileno(stdin))){
        inTerm=true;
    }
    bool run = true;
    roboRecive rr = roboRecive();

    usleep(500000);
    //usleep(500000);

    if (inTerm)cout << " Start " << endl;
    bitset<8> byte(0);
    int bufSize = 10;
    uint8_t* buf = new uint8_t[bufSize];
    while(run){
        rr.run(buf,bufSize);
        if(buf[0]=='[' && buf[6]==']'){
            // "[01234]"
            //  0123456
            //  [1:modifiers, 2:mfl, 3:mfr, 4:mbl, 5:mbr]
            byte = bitset<8>(buf[1]);
            //cout << byte << endl;
            if(byte[0] == 0){ // mfl forward
                cout << (int)buf[2] << endl;
                frontLeft.setForward(255-(uint8_t)buf[2]);
            }else{ // mfl backward
                frontLeft.setBackward(255-(uint8_t)buf[2]);
            }
            if(byte[1] == 0){ // mfr forward
                frontRight.setForward(255-(uint8_t)buf[3]);
            }else{ // mfr backward
                frontRight.setBackward(255-(uint8_t)buf[3]);
            }
            if(byte[2] == 0){ // mbl forward
                backLeft.setForward(255-(uint8_t)buf[4]);
            }else{ // mbl backward
                backLeft.setBackward(255-(uint8_t)buf[4]);
            }
            if(byte[3] == 0){ // mbr forward
                backRight.setForward(255-(uint8_t)buf[5]);
            }else{ // mbr backward
                backRight.setBackward(255-(uint8_t)buf[5]);
            }
            if(inTerm){ // Print msg
                //cout << "MSG: " << buf[0] << buf[1] << buf[2] << buf[3] << buf[4]
                //<< buf[5] << buf[6] << endl;
            }
        }else if(buf[0]=='[' && buf[2]==']'){
            switch(buf[1]){
                case '-': // Exit
                    frontLeft.stop();
                    backLeft.stop();
                    frontRight.stop();
                    backRight.stop();
                    run = false;
                break;
                case '@': // Connect //WIP
                break;
            }
        }
    }
    frontLeft.stop();
                    backLeft.stop();
                    frontRight.stop();
                    backRight.stop();
                    run = false;
    delete[]buf;
    if (inTerm)cout << "Stopped....." << endl;
    return 0;
}
