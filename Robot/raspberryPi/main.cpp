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
#include <time.h>
#include <pthread.h>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <signal.h>
#include <unistd.h>

#include "arduinoSPI.h"
#include "roboRecive.h"
#include "Global.h"

//http://hertaville.com/2012/11/18/introduction-to-accessing-the-raspberry-pis-gpio-in-c/

using namespace std;

bool inTerm = false;
bool run = true;
uint8_t motorsLeft = 0;
uint8_t motorsRight = 0;
bool motorsLeftForward;
bool motorsRightForward;

//Convert a struct sockaddr address to a string, IPv4 and IPv6:

void get_ip_str(const struct sockaddr *sa, vector<char>* str) {
    char s[INET6_ADDRSTRLEN];
    switch (sa->sa_family) {
        case AF_INET:
            //char s[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(((struct sockaddr_in *) sa)->sin_addr),
                    s, INET_ADDRSTRLEN);
            (*str).clear();
            for (int i = 0; i < INET_ADDRSTRLEN; ++i) {
                (*str).push_back(s[i]);
            }
            break;

        case AF_INET6:

            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *) sa)->sin6_addr),
                    s, INET6_ADDRSTRLEN);
            (*str).clear();
            for (int i = 0; i < INET6_ADDRSTRLEN; ++i) {
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

void printCharVector(vector<char>* cv) {
    if (cv == NULL || (*cv).empty()) {
        return;
    }
    for (unsigned int i = 0; i < (*cv).size(); ++i) {
        cout << (*cv)[i];
    }
}

void* arduino_run(void *ptr) {

    char *portname = (char*) "/dev/ttyACM0";
    int fd;
    cout << "fd = " << fd << endl;
    arduinoSPI device = arduinoSPI();
    if (inTerm) {
        cout << "Arduino starting..." << endl;
    }
    string sendPacket = "";
    string recivePacket = "";
    if (device.connect(&fd, portname, 115200, false)) {
        if (inTerm) {
            cout << "connected to arduino" << endl;
        }
        while (run) {
            sendPacket = "";
            recivePacket = "";
            bitset<8> byte = bitset<8>(0);
            byte[0] = motorsLeftForward;
            byte[1] = motorsRightForward;
            unsigned long temp = byte.to_ulong();
            sendPacket.push_back('[');
            sendPacket.push_back(static_cast<unsigned char> (temp));
            sendPacket.push_back((unsigned char)motorsLeft);
            sendPacket.push_back((unsigned char)motorsRight);
            sendPacket.push_back(']');
            device.send(fd, &sendPacket);
            sleep(0, 90000000);
            if (device.recivePacket(fd, &recivePacket)) {/*
                ultraSonicSensors[0] = recivePacket[0]=='1';
                ultraSonicSensors[1] = recivePacket[1]=='1';
                ultraSonicSensors[2] = recivePackeut[2]=='1';
                ultraSonicSensors[3] = recivePacket[3]=='1';
                                                       */
            }
        }
    } else {
        cout << "failed to connect to arduino" << endl;
    }
    sendPacket = "";
    recivePacket = "";
    bitset<8> byte = bitset<8>(0);
    byte[0] = true;
    byte[1] = true;
    unsigned long temp = byte.to_ulong();
    sendPacket.push_back('[');
    sendPacket.push_back(static_cast<unsigned char> (temp));
    sendPacket.push_back(0);
    sendPacket.push_back(0);
    sendPacket.push_back(']');
    device.send(fd, &sendPacket);
    sleep(0,5000);
    //device.close(fd);
    pthread_exit(ptr);
    return ptr;
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

int main(void) {
    cout << "Starting up" << endl;
    if (isatty(fileno(stdin))) {
        inTerm = true;
    }
    pthread_t arduino;

    roboRecive rr = roboRecive();

    sleep(0, 5000000);
    //sleep(0,500000);

    if (inTerm)cout << " Start " << endl;
    bitset<8> byte(0);
    int bufSize = 5;
    uint8_t* buf = new uint8_t[bufSize];
    bool exitLoops = false;

    //Handshake/Connect
    if (inTerm) {
        cout << "Waiting for handshake" << endl;
    }
    while (!exitLoops) {
        if (rr.run(buf, bufSize, &in, 1, 0) && buf[0] == '[' && buf[1] == '#' && buf[2] == '-' && buf[3] == '?' && buf[4] == ']') {
            //cout << "Got connection from " << in.sin_addr.s_addr << ':' << in.sin_port<< endl;
            get_ip_str((sockaddr*) & in, &in_ip);
            in_port = htons(in.sin_port);
            to = in;
            to_ip = in_ip;
            to_port = in_port;
            if (inTerm) {


                cout << "Connect Request From: ";
                printCharVector(&in_ip);
                cout << ":" << in_port << endl;
            }
            for (int i = 0; i < 6 && !exitLoops; ++i) {
                rr.send(new string("[#-#]"), &to);
                if (rr.run(buf, bufSize, &to, 1, 0) && buf[0] == '[' && buf[1] == '*' && buf[2] == '-' && buf[3] == '#' && buf[4] == ']') {
                    //cout << "Locking connection" << endl;
                    for (int x = 0; x < 3 && !exitLoops; ++x) {
                        rr.send(new string("[*-*]"), &to);

                    }
                    //cout << "Done" << endl;
                    exitLoops = true;
                }
            }
        }
    }
    if (inTerm) {
        cout << "Connection locked in, to: ";
        printCharVector(&to_ip);
        cout << ":" << to_port << endl;
    }
    pthread_create(&arduino, NULL, arduino_run, NULL);
    //Handshake/Connect
    while (run) {
        buf[0] = 0;
        buf[1] = 0;
        buf[2] = 0;
        buf[3] = 0;
        buf[4] = 0;
        
        if (rr.run(buf, bufSize, &in, 0, 500) && in.sin_addr.s_addr == to.sin_addr.s_addr && in.sin_port == to.sin_port) {
            getTimeOfDayMills64(&last);
            if (buf[0] == '[' && buf[4] == ']') {
                
                // "[01234]"
                //  0123456
                //  [1:modifiers, 2:motorsLeftPower,3:motorsRightPower]
                byte = bitset<8>(buf[1]);
                // byte[0] is the stop flag!
                motorsLeftForward = byte[1];
                motorsRightForward = byte[2];
                motorsLeft = buf[2];
                motorsRight = buf[3];
            } else if (buf[0] == '[' && buf[2] == ']') {

                switch (buf[1]) {
                    case '-': // Exit
                        motorsLeftForward = true;
                        motorsRightForward = true;
                        motorsLeft = 0;
                        motorsRight = 0;
                        run = false;
                        break;

                }
            }
        }
        getTimeOfDayMills64(&current);
        //cout <<  current << endl;
        if ((current - last) > 1100000) {
            if (inTerm && connected) {
                cout << "Connection LOST!   > " << current << endl;
                connected = false;
            }
            motorsLeftForward = true;
            motorsRightForward = true;
            motorsLeft = 0;
            motorsRight = 0;
        } else {
            if (!connected) {
                connected = true;
                if (inTerm) {
                    cout << "Connection Gained! > " << current << endl;
                }
            }
        }
        sleep(0, 1000000);
    }
    motorsLeftForward = true;
    motorsRightForward = true;
    motorsLeft = 0;
    motorsRight = 0;
    run = false;
    delete[]buf;
    sleep(1);
    void* result;
    cout << "stopping arduino thread" << endl;
    pthread_join(arduino, &result);
    if (inTerm)cout << "Stopped....." << endl;
    exit(0);
    return 0;
}
