#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <iostream>


#include "roboRecive.h"

using std::cout;
using std::endl;

int sock, length, n;
socklen_t fromlen;
struct sockaddr_in server;
struct sockaddr_in from;


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

int roboRecive::run(vector<uint8_t>* buf)
{

    //memset(&buf, '\0',bufSize);
    *buf.erase(*buf.begin(),*buf.end());
    //http://stackoverflow.com/questions/4223711/a-more-elegant-way-to-use-recv-and-vectorunsigned-char
    n = recvfrom(sock,buf[0],*buf.size(),0,(struct sockaddr *)&from,&fromlen);
    if (n < 0) error("recvfrom");

    //write(1,"Received a datagram: ",21);
    //write(1,buf,n);
    /*
    n = sendto(sock,"Got your message\n",17,
               0,(struct sockaddr *)&from,fromlen);
    if (n  < 0) error("sendto");
    */
    return n;
}

roboRecive::~roboRecive()
{
    close(sock);
}
