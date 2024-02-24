#include "imiapp.h"
#include "inputpacket.h"

#include <unistd.h>
#include <iostream>
using namespace std;

static bool imiAppBuildPacket(ImiApp& app, InputPacket& packet);
static bool imiAppGetFromUser(ImiApp& app, InputPacket& packet);
static bool imiAppGeneratePacket(ImiApp& app, InputPacket& packet);

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

	InputPacket packet;
	while (imiAppBuildPacket(app, packet))	//формирование пакета
	{		
		//запись пакета в канал обмена
		write(app.writeFd, &packet.count, sizeof(packet.count));
		write(app.writeFd, packet.data, packet.count * sizeof(InputPacketItem));
	}
	
	return 0;
}

bool imiAppBuildPacket(ImiApp& app, InputPacket& packet) {
	
   return app.fromUser ? imiAppGetFromUser(app,packet) : imiAppGeneratePacket(app,packet);
}

bool imiAppGetFromUser(ImiApp& /*app*/, InputPacket& packet) {
	
	cout << "count=";
	cin >> packet.count;
	if ( packet.count > INPUTPACKET_MAXCOUNT )
		return false;
	
	cout << "items=";
	for(unsigned i = 0; i < packet.count; ++i)
		cin >> packet.data[i].level;
	
	return true;
}

bool imiAppGeneratePacket(ImiApp& app, InputPacket& packet) {
	
    if ( !app.packetCount )
        return false;
        
    //ожидание момента времени
	 while ( clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &app.actTime, NULL) != 0 )
	    continue;
	 
	 //расчет следующей метки времени
    app.actTime = add(app.actTime, 0, 100000000);
	 
	 //формирование пакета
    packet.count = app.packetSize;
    for(unsigned i = 0; i<packet.count; ++i )
        packet.data[i].level =  rand() % app.maxLevel;
    
    //изменение количества генерации пакетов
	 if( app.packetCount >0 )
        --app.packetCount;
    return true;
}