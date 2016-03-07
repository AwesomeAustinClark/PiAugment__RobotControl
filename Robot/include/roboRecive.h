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
    int run(uint8_t*,int);
    int run(uint8_t*,int,std::string*,int*);
    virtual ~roboRecive();
protected:
private:
};

#endif // ROBORECIVE_H
