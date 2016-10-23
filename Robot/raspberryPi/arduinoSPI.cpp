#include "arduinoSPI.h"


#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <iostream>

using namespace std;

int
arduinoSPI::set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                cout << "error %d from tcgetattr" << endl;
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                cout << "error %d from tcsetattr" << endl;
                return -1;
        }
        return 0;
}

void
arduinoSPI::set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                cout << "error %d from tggetattr" << endl;
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                cout << "error %d setting term attributes" << endl;
}


bool arduinoSPI::connect(int* fd, char* portname, int speed, bool blocking){
    *fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
            //cout << "error %d opening %s: %s" << endl;//, errno, portname, strerror (errno));
            return 0;
    }
    set_interface_attribs (*fd, speed, 0);
    set_blocking (*fd, blocking);
    return true;
}

bool arduinoSPI::send(int fd, string *msg){
    write (fd, msg->c_str(), msg->length());           // send ### character greeting
    usleep ((msg->length() + 25) * 100);             // sleep enough to transmit the ### plus
    return true;
}

/* OUTDATED (probbly still works though...)
bool recive(int fd, string* msg, int length=100, bool blocking=false,
    int timeout=10){
                                 // receive 25:  approx 100 uS per char transmit
    char buf [length];
    for(int i=0;i<sizeof buf;++i){
        buf[i] = (char) 33;
    }
    int n = 0;
    while(n<length && timeout>0){
        n = read (fd, buf, sizeof buf);  // read up to 100 characters if ready to read
        for(int i=0;i<n;++i){
            *msg+=buf[i];
        }
        usleep(25);
        timeout-=1;
    }

    return true;
}
*/

bool arduinoSPI::recivePacket(int fd, string* msg){
                                 // receive 25:  approx 100 uS per char transmit
    char buf [1];
    buf[0]=' ';
    int n = 0;
    n = read (fd, buf, 1);  // read up to 100 characters if ready to read
    if(n>0 && buf[0]==';'){
        return true;
    }else{
        msg->push_back(buf[0]);
        return false;
    }
}

bool arduinoSPI::close(int fd){
    close(fd);
    return true;
}


arduinoSPI::arduinoSPI()
{
    //ctor
}

arduinoSPI::~arduinoSPI()
{
    //dtor
}