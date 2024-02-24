#include "procapp.h"
#include "procapp.cpp"

using namespace std;

int main()
{
	ProcApp app;

	app.procConfig.A = 1;
	app.procConfig.B = 3;

	int ret = procAppRun(app);
	return ret;
}