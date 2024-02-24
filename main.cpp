#include "procapp.h"
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
#include "procapp.cpp"
#include "iostream"

int main()
{
	int pipeFd[2];	
	if ( pipe(pipeFd) == -1 ) { 	// создает комуникац. канал и два дискриптора 0 - чтение, 1 - запись в случае успеха
		perror("pipe");
		return 2;
	}
	
	pid_t pid = fork();	// клонируем процесс
	if ( pid < 0 ) {	// -1 - ошибка
		perror("fork");
		return 1;
	}
	if ( pid == 0 ) {	// 0 - код потомка
		close(pipeFd[1]);

		ProcApp app;
		app.procConfig.A = 1;
		app.procConfig.B = 3;
		app.readFd = pipeFd[0];		
		int ret = procAppRun(app);
		close(app.readFd);
		return ret;	
	}
	
	close(pipeFd[0]);
	close(pipeFd[1]);	
	int status;
	while ( wait(&status) == -1 )	// while not id daughter
		continue;
	return 0;
}