#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdint.h>

void getTimeOfDayMills64(uint64_t*);
void sleep(int sec=0, long nano=0);
void sleeps(int sec=0);
void sleepm(int mill=0);
void sleepmc(int micro=0);
void sleepn(int nano=0);
#endif // GLOBAL_H
