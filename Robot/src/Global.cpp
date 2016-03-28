#include "Global.h"
#include <sys/time.h>
#include <unistd.h>

void getTimeOfDayMills64(uint64_t* time){
    struct timeval tempTv;
    gettimeofday(&tempTv,NULL);
    (time[0]) = tempTv.tv_sec*(uint64_t)1000000+tempTv.tv_usec;
}
