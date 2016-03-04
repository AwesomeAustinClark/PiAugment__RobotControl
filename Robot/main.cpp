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

int main (void)
{
    if(isatty(fileno(stdin))){
        inTerm=true;
    }
    bool run = true;
    roboRecive rr = roboRecive();
    GPIOSETUP();
    usleep(500000);
    Motor frontLeft = Motor(5,6);
    Motor frontRight = Motor(24,25);
    Motor backLeft = Motor(3,2);
    Motor backRight = Motor(23,18);
    //usleep(500000);

    if (inTerm)cout << " Start " << endl;
    bitset<8> byte(0);
    vector<uint8_t>buffer(20);
    while(run){
        rr.run(&buffer);
        if(buffer[0]=='[' && buffer[6]==']'){
            // "[01234]"
            //  0123456
            //  [1:modifiers, 2:mfl, 3:mfr, 4:mbl, 5:mbr]
            byte = bitset<8>(buffer[1]);
            if(byte[0] == 1){ // mfl forward

                frontLeft.setForward(((((int)buffer[2])/255)*1023));
            }else{ // mfl backward
                frontLeft.setBackward(((((int)buffer[2])/255)*1023));
            }
            if(byte[1] == 1){ // mfr forward
                frontRight.setForward(((((int)buffer[3])/255)*1023));
            }else{ // mfr backward
                frontRight.setBackward(((((int)buffer[3])/255)*1023));
            }
            if(byte[2] == 1){ // mbl forward
                backLeft.setForward(((((int)buffer[4])/255)*1023));
            }else{ // mbl backward
                backLeft.setBackward(((((int)buffer[4])/255)*1023));
            }
            if(byte[3] == 1){ // mbr forward
                backRight.setForward(((((int)buffer[5])/255)*1023));
            }else{ // mbr backward
                backRight.setBackward(((((int)buffer[5])/255)*1023));
            }
            if(inTerm){ // Print msg
                cout << "MSG: " << buffer[0] << buffer[1] << buffer[2] << buffer[3] << buffer[4]
                << buffer[5] << buffer[6] << endl;
            }
        }
    }
    if (inTerm)cout << "Stopped....." << endl;
    return 0;
}
