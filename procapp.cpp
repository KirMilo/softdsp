#include "procapp.h"
#include "inputpacket.h"
#include "outputpacket.h"
#include "packetcontainer.h"
#include "packet.h"
#include "messageid.h"
#include "configpacket.h"

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <cstring>

using namespace std;

static bool procAppReceivePacket(ProcApp& app, Packet& packet);
static void procAppProcessing(ProcApp& app, Packet& input, Packet& output);
static void procAppSendPacket(ProcApp& app, Packet& packet);

static void procAppConfig(ProcApp& app, Packet& packet);

static void processingImpl(const Packet& input, Packet& output, const ProcConfig& config);

static void fillingGroup(unsigned count,unsigned maxPos,unsigned localMax, unsigned start, unsigned end ,OutputPacketBody& output);

//функции, исполняемые потоками
static void* processingExecute(void* arg);
static void* inputExecute(void* arg);
static void* outputExecute(void*);

//функции-обработчики для вызова из диспетчера
static void inputHandler(Packet* packet, void* clientData);
static void procConfigHandler(Packet* packet, void* clientData);
static void sendError(Packet* packet, PacketContainer& oc);


//структурные типы данных для передачи параметров в функцию потока
struct InputThreadData {
   ProcApp* app;
   PacketContainer* ic;
};

struct OutputThreadData {
   ProcApp* app;
   PacketContainer* oc;
};

struct ProcessingThreadData {
   ProcApp* app;
   PacketContainer* ic;
   PacketContainer* oc;
};

int procAppRun(ProcApp& app){
   //создание буферов совместного доступа потоками
   pcInit(&app.ic);
   pcInit(&app.oc);
   
   //инициализация диспетчера
   dispInit(app.dispatcher);
   dispAddHandler(app.dispatcher,MESSAGE_INPUTPACKET,inputHandler,&app);
   dispAddHandler(app.dispatcher,MESSAGE_PROCCONFIG,procConfigHandler,&app);

   //заполнение структур-параметров функций потоков
   InputThreadData itData;
   itData.app = &app;
   itData.ic = &app.ic;
   ProcessingThreadData ptData;
   ptData.app = &app;
   ptData.ic = &app.ic;
   ptData.oc = &app.oc;
   OutputThreadData otData;
   otData.app = &app;
   otData.oc = &app.oc;

   //объявление дескрипторов потоков
   pthread_t inputThread;
   pthread_t processingThread;
   pthread_t outputThread;
   
   //порождение потоков управления
   pthread_create(&inputThread,NULL,inputExecute,&itData);
   pthread_create(&processingThread,NULL,processingExecute,&ptData);
   pthread_create(&outputThread,NULL,outputExecute,&otData);

   //ожидание завершения потоков
   pthread_join(inputThread,0);
   pthread_join(processingThread,0);
   pthread_join(outputThread,0);

   //освобождение ресурсов буферов
   pcDestroy(&app.oc);
   pcDestroy(&app.ic);
   return 0;
}

static bool procAppReceivePacket(ProcApp& app, Packet& packet){
	//прием количества отсчетов
	int ret = read(app.readFd, &packet.header, sizeof(packet.header));
	if ( ret == 0 || ret == -1 )
		return false;
	if ( packet.header.size == 0 )
		return true;


	//прием набора отсчетов
	ret = read(app.readFd, packet.body, packet.header.size);
	if ( ret == 0 || ret == -1 )
		return false;

	return true;
}

static void procAppProcessing(ProcApp& app, Packet& input, Packet& output)
{
   struct timespec procTime;
   clock_gettime(CLOCK_MONOTONIC,&procTime);
   procTime.tv_nsec += app.procDelay * 1000000;
   if ( procTime.tv_nsec >= 1000000000 ) {
        ++procTime.tv_sec;
        procTime.tv_nsec -= 1000000000;
   }
   
   processingImpl(input,output,app.procConfig);
   
   clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&procTime,NULL);  
   return;
}

static void procAppSendPacket(ProcApp& app, Packet& packet){

   write(app.consumerFd, &packet.header,sizeof(packet.header));
	write(app.consumerFd, packet.body, packet.header.size);
	return;
}

static void processingImpl(const Packet& input, Packet& output, const ProcConfig& config){
   
   InputPacketBody& iBody = *(InputPacketBody*)input.body;
	OutputPacketBody& oBody = *(OutputPacketBody*)output.body;

    unsigned start = 0;
    unsigned currentMaxPosition = 0;
    unsigned currentMax = 0;

    unsigned outCount = 0;
    unsigned startGroup = 0;

    for(unsigned i = 0; i < iBody.count; ++i){
        
        if(config.A <= iBody.data[i].level && iBody.data[i].level <= config.B){

            if (startGroup == 0){
                start = currentMaxPosition = i;
                currentMax = iBody.data[i].level;
                startGroup = 1;
            }

            else{
                if (iBody.data[i].level > currentMax){
                    currentMax = iBody.data[i].level;
                    currentMaxPosition = i;
                }
            }
        }
        else{
            if (startGroup == 1){

                fillingGroup(outCount, currentMaxPosition, currentMax, start, i-1, oBody);
                outCount++;
                startGroup = 0;
            }
        }
    }
    if (startGroup == 1){

        fillingGroup(outCount, currentMaxPosition, currentMax, start, iBody.count-1, oBody);
        outCount++;
    }
    oBody.count = outCount;
    output.header.size = oBody.count * sizeof(OutputPacketItem) + sizeof(oBody.count);
    output.header.message = MESSAGE_OUTPUTPACKET;
    return;
}

static void fillingGroup(unsigned count, unsigned maxPos, unsigned localMax, unsigned start, unsigned end ,OutputPacketBody& output){
    
    output.data[count].localMaxPosition = maxPos;
    output.data[count].localMax = localMax;
    output.data[count].positionStartItem = start;
    output.data[count].positionEndItem = end;
}

static void* processingExecute(void* arg) {

	ProcessingThreadData* params = (ProcessingThreadData*) (arg);
	PacketContainer* ic = params->ic;
	Dispatcher* dispatcher = &params->app->dispatcher;
	while (1) {
		Packet* input = pcStartReadPacket(ic);
      int ret = dispProcess(*dispatcher,input);
      if ( ret == -1 ) {
         sendError(input,params->app->oc);
      }
		pcFinishReadPacket(ic);
	}
	return 0;
}

static void* inputExecute(void* arg) {

   InputThreadData* params = (InputThreadData*)(arg);
   PacketContainer* ic = params->ic;
   while ( 1 ) {
      Packet* input = pcStartWritePacket(ic);
      if ( !procAppReceivePacket(*params->app,*input) )
         break;
      pcFinishWritePacket(ic);
   }
   return 0;
}

static void* outputExecute(void* arg) {

   OutputThreadData* params = (OutputThreadData*)(arg);
   PacketContainer* oc = params->oc;
   while ( 1 ) {
      Packet* output = pcStartReadPacket(oc);
      procAppSendPacket(*params->app,*output);
      pcFinishReadPacket(oc);
   }
   return 0;
}

static void procAppConfig(ProcApp& app, Packet& packet) {
   
   ConfigPacketBody* cfgBody = (ConfigPacketBody*)packet.body;
   app.procConfig.A = cfgBody->A;
   app.procConfig.B = cfgBody->B;
   return;
}

static void inputHandler(Packet* packet, void* clientData) {

   ProcApp* app = (ProcApp*)clientData;

   Packet* output = pcStartWritePacket(&app->oc);
   procAppProcessing(*app,*packet,*output);
   pcFinishWritePacket(&app->oc);

   return;
}

static void procConfigHandler(Packet* packet, void* clientData) {

   ProcApp* app = (ProcApp*)clientData;

   procAppConfig(*app,*packet);
   return;
}

static void sendError(Packet* packet, PacketContainer& oc) {
   
   Packet* output = pcStartWritePacket(&oc);
   
   //формирование заголовка
   output->header.size = packet->header.size + sizeof(packet->header);
   output->header.message = MESSAGE_BADPACKET;
   
   //копирование плохого пакета 
   memcpy(output->body,packet,output->header.size);
   
   pcFinishWritePacket(&oc);
   return;
}