#include "Global.h"
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

void getTimeOfDayMills64(uint64_t* time)
{
    struct timeval tempTv;
    gettimeofday(&tempTv,NULL);
    (time[0]) = tempTv.tv_sec*(uint64_t)1000000+tempTv.tv_usec;
}

void sleep(int sec, long nano)
{
    struct timespec tv;
    tv.tv_sec = sec;
    tv.tv_nsec = nano;
    clock_nanosleep(CLOCK_REALTIME, 0, &tv, NULL);
}

void sleeps(int sec)
{
    struct timespec tv;
    tv.tv_sec = sec;
    tv.tv_nsec = 0;
    nanosleep(&tv,NULL);
}

void sleepm(int mill)
{
    struct timespec tv;
    tv.tv_sec = 0;
    tv.tv_nsec = mill*(long)1000000;
    nanosleep(&tv,NULL);
}

void sleepmc(int micro)
{
    struct timespec tv;
    tv.tv_sec = 0;
    tv.tv_nsec = micro*(long)1000;
    nanosleep(&tv,NULL);
}

void sleepn(int nano)
{
    struct timespec tv;
    tv.tv_sec = 0;
    tv.tv_nsec = nano;
    nanosleep(&tv,NULL);
}
