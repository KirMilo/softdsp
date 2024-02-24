#include "imiapp.h"
#include "inputpacket.h"

#include <unistd.h>
#include <iostream>
using namespace std;

static bool imiAppBuildPacket(ImiApp& app, InputPacket& packet);

int imiAppRun(ImiApp& app) {
	
	InputPacket packet;
	while (imiAppBuildPacket(app, packet))	//формирование пакета
	{		
		//запись пакета в канал обмена
		write(app.writeFd, &packet.count, sizeof(packet.count));
		write(app.writeFd, packet.data, packet.count * sizeof(InputPacketItem));
	}
	
	return 0;
}

bool imiAppBuildPacket(ImiApp& /*app*/, InputPacket& packet) {
	
	cout << "count=";
	cin >> packet.count;
	if ( packet.count > INPUTPACKET_MAXCOUNT )
		return false;
	
	cout << "items=";
	for(unsigned i = 0; i < packet.count; ++i)
		cin >> packet.data[i].level;
	
	return true;
}

