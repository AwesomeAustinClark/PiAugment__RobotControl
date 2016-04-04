#ifndef ROBORECIVE_H
#define ROBORECIVE_H

#include <vector>
#include <stdint.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
class roboRecive
{
public:
    roboRecive();
    bool run(uint8_t*,int, int, int);
    bool run(uint8_t*,int,sockaddr_in*, int, int);
    bool send(std::string*, sockaddr_in*);
    virtual ~roboRecive();
protected:
private:
};

#endif // ROBORECIVE_H
