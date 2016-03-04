#ifndef ROBORECIVE_H
#define ROBORECIVE_H

#include <vector>

class roboRecive
{
public:
    roboRecive();
    int run(std::vector<uint8_t>*);
    virtual ~roboRecive();
protected:
private:
};

#endif // ROBORECIVE_H
