#include "imiapp.h"
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>

#include "inputpacket.h"
#include "packet.h"

using namespace std;


static bool imiAppBuildPacket(ImiApp& app, Packet& packet);
static bool imiAppGetFromUser(ImiApp& app, Packet& packet);
static bool imiAppGeneratePacket(ImiApp& app, Packet& packet);

//расчет метки времени, отстоящей от заданной на указанный интервал
struct timespec add(struct timespec left, time_t tv_sec, long tv_nsec) {
    
	left.tv_sec += tv_sec;
    left.tv_nsec += tv_nsec;
    if ( left.tv_nsec >= 1000000000 ) {
        ++left.tv_sec;
        left.tv_nsec -= 1000000000;
    }
    return left;
}

int imiAppRun(ImiApp& app) {

	//формирование первой метки времени для генерации пакета
	clock_gettime(CLOCK_MONOTONIC, &app.actTime);

	Packet packet;
	while (imiAppBuildPacket(app, packet))	//формирование пакета
	{		
		//запись пакета в канал обмена
		write(app.writeFd, &packet.header, sizeof(packet.header));
		write(app.writeFd, packet.body, packet.header.size);
	}
	
	return 0;
}

bool imiAppBuildPacket(ImiApp& app, Packet& packet) {
	
   return app.fromUser ? imiAppGetFromUser(app, packet) : imiAppGeneratePacket(app, packet);
}

bool imiAppGetFromUser(ImiApp& /*app*/, Packet& packet) {
	
	InputPacketBody& body = *(InputPacketBody*)packet.body;
	cout << "count=";
	cin >> body.count;
	if ( body.count > INPUTPACKET_MAXCOUNT )
		return false;
	
	cout << "items=";
	for(unsigned i = 0; i < body.count; ++i)
		cin >> body.data[i].level;
	
	packet.header.size = body.count*sizeof(InputPacketItem) + sizeof(body.count);
	return true;
}

bool imiAppGeneratePacket(ImiApp& app, Packet& packet) {
	
    if ( !app.packetCount )
        return false;
        
    //ожидание момента времени
	while ( clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &app.actTime, NULL) != 0 )
	    continue;
	 
	//расчет следующей метки времени
    long jitter = long(rand() % (2*app.generationJitterLevel+1)) - app.generationJitterLevel;
	long timeDelay = 100000000 + jitter * 1000000;
	if ( timeDelay < 0 )
		timeDelay = 0;
	app.actTime = add(app.actTime,0,timeDelay);
	 
	//формирование пакета
	InputPacketBody& body = *(InputPacketBody*)packet.body;
    body.count = app.packetSize;
    for(unsigned i = 0; i < body.count; ++i )
        body.data[i].level =  rand() % app.maxLevel;
    
	packet.header.size = body.count*sizeof(InputPacketItem) + sizeof(body.count);

    //изменение количества генерации пакетов
	if( app.packetCount >0 )
        --app.packetCount;
    return true;
}