#include "procapp.h"
#include "procapp.cpp"

using namespace std;

int main()
{
	ProcApp app;
	int ret = procAppRun(app);
	return ret;
}