#include "procapp.h"
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
#include "procapp.cpp"
#include "iostream"

int main() {
	
	pid_t pid = fork();  // клонируем процесс
	if ( pid < 0 ) {	// -1 - ошибка
		perror("fork");
		return 1;
	}
	if ( pid == 0 ) {  // 0 - код потомка
		ProcApp app;
		
		app.procConfig.A = 1;
		app.procConfig.B = 3;

		int ret = procAppRun(app);
		return ret;		// call ret
	}
	
	int status;
	while ( wait(&status) == -1 ){	// while not id daughter
		continue;}
	return 0;
}