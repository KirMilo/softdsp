#include "procapp.h"
#include "imiapp.h"
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>

#include "procapp.cpp"
#include "iostream"
#include "imiapp.cpp"

int main()
{
	int pipeFd[2];
	if ( pipe(pipeFd) == -1 ) { 	// создает комуникац. канал и два дискриптора 0 - чтение, 1 - запись в случае успеха
		perror("pipe");
		return 2;
	}
	
	//порождение процесса обработки
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
	
	//порождение процесса-имитатора
	pid_t imiPid = fork();
	if ( imiPid < 0 ) {
		perror("fork");
		return 1;
	}
	if ( imiPid == 0 ) {
		close(pipeFd[0]);
		
		ImiApp app;
		app.writeFd = pipeFd[1];
		app.fromUser = false;
		app.packetCount = 4;
		app.packetSize = 16;
		app.maxLevel = 4;

		int ret = imiAppRun(app);
		close(app.writeFd);
		return ret;
	}

	close(pipeFd[0]);
	close(pipeFd[1]);	
	
	//ожидание завершения всех потомков
	for( unsigned i = 0; i < 2; ++i) {
		int status;
		while ( wait(&status) == -1 )
			continue;
	}
	return 0;
}