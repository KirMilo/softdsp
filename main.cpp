#include "procapp.h"
#include "imiapp.h"
#include "consumerapp.h"
#include "parsparams.cpp"

#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>


#include "procapp.cpp"
#include "imiapp.cpp"
#include "consumerapp.cpp"
#include "packetcontainer.cpp"

bool optionsParse(int argc, char** argv, ControlParameters& params);

int main(int argc, char** argv)
{
	ProcConfig defaultConfig;
	defaultConfig.A = 1;
	defaultConfig.B = 3;

	ControlParameters params;
	params.fromUser = false;
	params.packetCount = 8;
	params.packetSize = 16;
	params.maxLevel = 4;
	params.procConfig = defaultConfig;
	params.procDelay = 0;
	params.generationJitterLevel = 0;

	if ( !optionsParse(argc, argv, params) ) 
		return 5;

	int pipeFd[2];
	if ( pipe(pipeFd) == -1 ) { 	// создает комуникац. канал и два дискриптора 0 - чтение, 1 - запись в случае успеха
		perror("pipe");
		return 2;
	}

	int pipeConsumer[2];
	if ( pipe(pipeConsumer) == -1 ) {
		perror("pipe");
		return 3;
	}
	
	//порождение процесса обработки
	pid_t pid = fork();	// клонируем процесс
	if ( pid < 0 ) {	// -1 - ошибка
		perror("fork");
		return 1;
	}
	if ( pid == 0 ) {	// 0 - код потомка
		close(pipeFd[1]);
		close(pipeConsumer[0]);

		ProcApp app;
		app.procConfig = defaultConfig;
		app.readFd = pipeFd[0];
		app.consumerFd = pipeConsumer[1];
		app.procDelay = params.procDelay;

		int ret = procAppRun(app);
		close(app.readFd);
		close(app.consumerFd);
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
		close(pipeConsumer[0]);
		close(pipeConsumer[1]);

		ImiApp app;
		app.writeFd = pipeFd[1];
		app.fromUser = params.fromUser;
		app.packetCount = params.packetCount;
		app.packetSize = params.packetSize;
		app.maxLevel = params.maxLevel;
		app.generationJitterLevel = params.generationJitterLevel;
		app.procConfig = params.procConfig;

		int ret = imiAppRun(app);
		close(app.writeFd);
		return ret;
	}

	//порождение процесса-потребителя
	pid_t consumerPid = fork();
	if ( consumerPid < 0 ) {
		perror("fork");
		return 1;
	}
	if ( consumerPid == 0 ) {
		close(pipeFd[0]);
		close(pipeFd[1]);
		close(pipeConsumer[1]);
		
		ConsumerApp app;
		app.readFd = pipeConsumer[0];
		int ret = consumerAppRun(app);
		close(app.readFd);
		return ret;
	}

	close(pipeFd[0]);
	close(pipeFd[1]);	
	close(pipeConsumer[0]);
	close(pipeConsumer[1]);
	
	//ожидание завершения всех потомков
	for( unsigned i = 0; i < 3; ++i) {
		int status;
		while ( wait(&status) == -1 )
			continue;
	}
	return 0;
}

bool optionsParse(int argc, char **argv, ControlParameters& params)
{
	while ( 1 ) {
		int c = getopt (argc, argv, "ic:s:l:a:b:p:j:");
		if ( c == -1 )
			break;
		switch (c) {
			case 'i':
				params.fromUser = false;
				break;
			case 'c':
				params.packetCount = atoi(optarg);
				break;
			case 's':
				params.packetSize = atoi(optarg);
				break;
			case 'l':
				params.maxLevel = atoi(optarg);
				break;
			case 'a':
				params.procConfig.A = atoi(optarg);
				break;
			case 'b':
				params.procConfig.B = atoi(optarg);
				break;
			case 'p':
				return parsParams(optarg, params);
			case 't':
				params.procDelay = atoi(optarg);
				break;
			case 'j':
                params.generationJitterLevel = atoi(optarg);
                break;
			default:
				return false;
      }	
	}
	return true;
}

