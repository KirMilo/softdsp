#ifndef imiapp_h
#define imiapp_h

#include "procconfig.h"

#include <time.h>

struct ImiApp {
   int writeFd;

   struct timespec actTime; //время генерации пакета
	bool fromUser; //признак формирования данных от пользователя
	int packetCount; //оставшееся количество имитируемых пакетов
   unsigned packetSize; //размер пакета (в отсчетах)
   unsigned maxLevel; //максимальное значение отсчета
   unsigned generationJitterLevel;  //уровень дрожания темпа генерации пакетов имитатором в миллисекундах
   ProcConfig procConfig;
};

int imiAppRun(ImiApp& app);

#endif
