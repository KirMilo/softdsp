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
};

int procAppRun(ProcApp& app);


#endif