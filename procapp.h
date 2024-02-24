#ifndef procapp_h
#define procapp_h

#include "procconfig.h"
#include "dispatcher.h"
#include "packetcontainer.h"

struct ProcApp
{
    ProcConfig procConfig;
    int readFd;
    int consumerFd;
    unsigned procDelay; //задержка для имитации обработки указанной длительности
    Dispatcher dispatcher; //диспетчер пакетов
    PacketContainer ic; //кольцевой буфер приема
    PacketContainer oc; //кольцевой буфер выдачи
};

int procAppRun(ProcApp& app);


#endif
