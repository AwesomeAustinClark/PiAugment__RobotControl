#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <map>
#include <cstdio>
#include <bitset>
#include <vector>
#include <sys/time.h>
#include <arpa/inet.h>
#include <string>
#include <cstring>

#include "Motor.h"
#include "roboRecive.h"
#include "UltraSonicSensor.h"
#include "Global.h"

//http://hertaville.com/2012/11/18/introduction-to-accessing-the-raspberry-pis-gpio-in-c/

using namespace std;

bool inTerm = false;

void wait()
{
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

void stopMotors()
{
    frontLeft.stop();
    frontRight.stop();
    backLeft.stop();
    backRight.stop();
}

void setForward()
{
    frontLeft.setForward(255);
    frontRight.setForward(255);
    backLeft.setForward(255);
    backRight.setForward(255);
}

//Convert a struct sockaddr address to a string, IPv4 and IPv6:
void get_ip_str(const struct sockaddr *sa, vector<char>* str){
    char s[INET6_ADDRSTRLEN];
    switch(sa->sa_family) {
        case AF_INET:
            //char s[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
                    s, INET_ADDRSTRLEN);
            (*str).clear();
            for(int i=0;i<INET_ADDRSTRLEN;++i){
                (*str).push_back(s[i]);
            }
            break;

        case AF_INET6:

            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
                    s, INET6_ADDRSTRLEN);
            (*str).clear();
            for(int i=0;i<INET6_ADDRSTRLEN;++i){
                (*str).push_back(s[i]);
            }
            break;

        default:
        (*str).clear();
        // = "Unknown AF";
        (*str).push_back('U');
        (*str).push_back('n');
        (*str).push_back('k');
        (*str).push_back('n');
        (*str).push_back('o');
        (*str).push_back('w');
        (*str).push_back('n');
        (*str).push_back(' ');
        (*str).push_back('A');
        (*str).push_back('F');
    }
}

void printCharVector(vector<char>* cv){
    if(cv==NULL || (*cv).empty()){
        return;
    }
    for(unsigned int i=0;i<(*cv).size();++i){
        cout << (*cv)[i];
    }
}

sockaddr_in in;
vector<char> in_ip;
unsigned short in_port;
sockaddr_in to;
vector<char> to_ip;
unsigned short to_port;
uint64_t last = 0;
uint64_t current = -1;
bool connected = true;
int main (void)
{
    cout << "Setting up...";
    GPIOSETUP();
    /*
    UltraSonicSensor uss(17, 27);
    int i=0;
    while(true){
        current=-1;
        uss.send();
        while(current==-1){
            uss.recive(&current);
            usleep(10);
        }
        cout << "time: " << current << ' ' << i++ << endl;
        usleep((unsigned int) 500000);
    }
    return 1;
    */
    Motor frontLeft = Motor(5,6,Motor::ModePwm);
    Motor frontRight = Motor(24,25, Motor::ModePwm);
    Motor backLeft = Motor(3,2, Motor::ModePwm);
    Motor backRight = Motor(23,18, Motor::ModePwm);


    stopMotors();
    cout << "Done." <<endl;
    /*
    cout << "Testing Motors... ";
    stopMotors();
    sleep(2);
    setForward();
    sleep(2);
    stopMotors();
    cout << "Done" << endl;
    */
    if(isatty(fileno(stdin)))
    {
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
    stopMotors();
    bool exitLoops = false;

    //Handshake/Connect
    if(inTerm){
        cout << "Waiting for handshake" << endl;
    }
    while(!exitLoops){
        if(rr.run(buf,bufSize,&in, 1, 0) && buf[0]=='[' && buf[1]=='#' && buf[2]=='-' && buf[3]=='?' && buf[4]==']'){
            //cout << "Got connection from " << in.sin_addr.s_addr << ':' << in.sin_port<< endl;
            get_ip_str((sockaddr*)&in,&in_ip);
            in_port = htons(in.sin_port);
            to = in;
            to_ip = in_ip;
            to_port = in_port;
            if(inTerm)
            {


                cout << "Connect Request From: ";
                printCharVector(&in_ip);
                cout << ":" << in_port << endl;
            }
            for(int i=0;i<6 && !exitLoops;++i){
                rr.send(new string("[#-#]"),&to);
                if(rr.run(buf,bufSize,&to, 1, 0)  && buf[0]=='[' && buf[1]=='*' && buf[2]=='-' && buf[3]=='#' && buf[4]==']'){
                    //cout << "Locking connection" << endl;
                    for(int x=0;x<3 && !exitLoops;++x){
                        rr.send(new string("[*-*]"),&to);

                    }
                    //cout << "Done" << endl;
                    exitLoops = true;
                }
            }
        }
    }
    if(inTerm)
    {
        cout << "Connection locked in, to: ";
        printCharVector(&to_ip);
        cout << ":" << to_port << endl;
    }
    //Handshake/Connect
    while(run)
    {
        if(rr.run(buf,bufSize,&in, 0, 500) && in.sin_addr.s_addr==to.sin_addr.s_addr && in.sin_port==to.sin_port)
        {
            getTimeOfDayMills64(&last);
            if(buf[0]=='[' && buf[6]==']')
            {
                // "[01234]"
                //  0123456
                //  [1:modifiers, 2:mfl, 3:mfr, 4:mbl, 5:mbr]
                byte = bitset<8>(buf[1]);
                //cout << byte << endl;
                //cout << (uint8_t)byte[2] << endl;
                if(byte[0] == 1)  // mfl forward
                {
                    frontLeft.setForward(255-(uint8_t)buf[2]);
                }
                else   // mfl backward
                {
                    frontLeft.setBackward(255-(uint8_t)buf[2]);
                }
                if(byte[1] == 1)  // mfr forward
                {
                    frontRight.setForward(255-(uint8_t)buf[3]);
                }
                else   // mfr backward
                {
                    frontRight.setBackward(255-(uint8_t)buf[3]);
                }
                if(byte[2] == 1)  // mbl forward
                {
                    backLeft.setForward(255-(uint8_t)buf[4]);
                }
                else   // mbl backward
                {
                    backLeft.setBackward(255-(uint8_t)buf[4]);
                }
                if(byte[3] == 1)  // mbr forward
                {
                    backRight.setForward(255-(uint8_t)buf[5]);
                }
                else   // mbr backward
                {
                    backRight.setBackward(255-(uint8_t)buf[5]);
                }
                if(inTerm)  // Print msg
                {
                    //cout << "MSG: " << buf[0] << buf[1] << buf[2] << buf[3] << buf[4]
                    //<< buf[5] << buf[6] << endl;
                }
            }
            else if(buf[0]=='[' && buf[2]==']')
            {

                switch(buf[1])
                {
                case '-': // Exit
                    frontLeft.stop();
                    backLeft.stop();
                    frontRight.stop();
                    backRight.stop();
                    run = false;
                    break;

                }
            }
        }
        getTimeOfDayMills64 (&current);
        //cout <<  current << endl;
        if((current-last)>1100000)
        {
            if(inTerm && connected){
                cout << "Connection LOST!   > " << current << endl;
                connected = false;
            }
            stopMotors();
            frontLeft.stop();
            backLeft.stop();
            frontRight.stop();
            backRight.stop();
        }else{
            if(!connected){
                connected = true;
                if(inTerm){
                    cout << "Connection Gained! > " << current << endl;
                }
            }
        }
        usleep(1);
    }
    stopMotors();
    run = false;
    delete[]buf;
    if (inTerm)cout << "Stopped....." << endl;
    exit(0);
    return 0;
}
