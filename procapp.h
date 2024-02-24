#ifndef procapp_h
#define procapp_h

#include "procconfig.h"

struct ProcApp
{
    ProcConfig procConfig;
    int readFd;
    int consumerFd;
    unsigned procDelay; //задержка для имитации обработки указанной длительности
};

int procAppRun(ProcApp& app);


#endif
