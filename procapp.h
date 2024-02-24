#ifndef procapp_h
#define procapp_h

struct ProcConfig
{
   // Limits
    unsigned A; 
    unsigned B;
};


struct ProcApp
{
    ProcConfig procConfig;
    int readFd;
    int consumerFd;
    unsigned procDelay; //задержка для имитации обработки указанной длительности
};

int procAppRun(ProcApp& app);


#endif
