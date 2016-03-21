#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <iostream>


#include "roboRecive.h"

using std::cout;
using std::endl;

int sock, length;
socklen_t fromlen;
struct sockaddr_in server;
struct sockaddr_in from;
//uint8_t buf[1024];

void error(const char *msg)
{
    perror(msg);
    exit(0);
}


using namespace std;
roboRecive::roboRecive()
{
    sock=socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) error("Opening socket");
    length = sizeof(server);
    bzero(&server,length);
    server.sin_family=AF_INET;
    server.sin_addr.s_addr=INADDR_ANY;
    server.sin_port=htons(4000);
    if (bind(sock,(struct sockaddr *)&server,length)<0)
        error("binding");
    fromlen = sizeof(struct sockaddr_in);
}

bool roboRecive::run(uint8_t* buf, int bufSize, int timeoutSec, int timeoutMills)
{
    fd_set rfds;
    fd_set efds;
    struct timeval tv;
    int retval;
    bool rb = true;
    //int n;

    /* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO(&rfds);
    FD_SET(sock, &rfds);

    /* Wait up to 1/10 of a second. */
    //tv.tv_usec = 10000;
    tv.tv_sec = timeoutSec;
    tv.tv_usec =  timeoutMills;
    //tv.tv_sec = 2;
    //tv.tv_usec =  1;
    memset(buf, '\0',bufSize);
    cout << "is: " << FD_ISSET(sock,&rfds);
    retval = select(sock+1, &rfds, NULL, &efds, &tv);
    cout << retval << endl;
    cout << "is2: " << FD_ISSET(sock,&rfds);
    //retval=1;
    //cout << "r: " << retval << endl;
    if (retval == -1){
        perror("select()");
        rb=false;
    }
    else if (retval)
    {
        recvfrom(sock,buf,bufSize,0,(struct sockaddr *)&from,&fromlen);
        //printf("Data is available now.\n");
        //cout << buf[0] << buf[1] << buf[2] << buf[3] << buf[4] << buf[5] << buf[6] << endl;
        rb = true;
        /* FD_ISSET(0, &rfds) will be true. */
    }
    else
    { // Nothing recived
        rb=false;
    }
    //if (n < 0) error("recvfrom");
    FD_CLR(sock,&rfds);
    return rb;
    //write(1,"Received a datagram: ",21);
    //write(1,buf,n);
    /*
    n = sendto(sock,"Got your message\n",17,
               0,(struct sockaddr *)&from,fromlen);
    if (n  < 0) error("sendto");
    */

}

bool roboRecive::run(uint8_t* buf, int bufSize, sockaddr_in* addr, int timeoutSec, int timeoutMills)
{
    bool rb = run(buf,bufSize, timeoutSec, timeoutMills);
    //*inAddr = inet_ntoa(from.sin_addr);
    *addr = from;
    return rb;
    //write(1,"Received a datagram: ",21);
    //write(1,buf,n);
    /*
    n = sendto(sock,"Got your message\n",17,
               0,(struct sockaddr *)&from,fromlen);
    if (n  < 0) error("sendto");
    */

}

bool roboRecive::send(string* msg, struct sockaddr_in* addr)
{
    //cout << "sendto: " << sendto(sock, (*msg).c_str(), (*msg).size(), 0, (sockaddr*)addr, sizeof(*addr)) << endl;;
    sendto(sock, (*msg).c_str(), (*msg).size(), 0, (sockaddr*)addr, sizeof(*addr));
    return true;
}

roboRecive::~roboRecive()
{
    close(sock);
}
