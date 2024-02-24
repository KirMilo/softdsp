#ifndef imiapp_h
#define imiapp_h

struct ImiApp {
   int writeFd;
};

int imiAppRun(ImiApp& app);

#endif
