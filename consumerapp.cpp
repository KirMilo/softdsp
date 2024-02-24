#include "consumerapp.h"
#include "outputpacket.h"
#include <unistd.h>
#include <cstdlib>
#include <iostream>
using namespace std;

static void consumerAppOutPacket(ConsumerApp& app, Packet& packet);
static bool consumerAppReceivePacket(ConsumerApp& app, Packet& packet);

int consumerAppRun(ConsumerApp& app)
{
	Packet packet;
	while (consumerAppReceivePacket(app,packet))
		consumerAppOutPacket(app,packet);
	return 0;
}

static void consumerAppOutPacket(ConsumerApp& /*app*/, Packet& packet) {
	
	OutputPacketBody& body = *(OutputPacketBody*)packet.body;
	cout << "Output Packet:" << endl;
	for(unsigned i = 0; i < body.count; ++i)
		cout << i+1 << " group: (| position of local max: " << body.data[i].localMaxPosition <<
		"| local max level: " << body.data[i].localMax << "| start: " <<
		 body.data[i].positionStartItem << "| end: " << body.data[i].positionEndItem <<  "|) " << endl;

	return;
}

static bool consumerAppReceivePacket(ConsumerApp& app, Packet& packet)
{
	int ret = read(app.readFd, &packet.header, sizeof(packet.header));
	if ( ret == 0 || ret == -1 )
		return false;
	if ( packet.header.size == 0 )
		return true;
	ret = read(app.readFd, packet.body, packet.header.size);
	if ( ret == 0 || ret == -1 )
		return false;
	return true;
}
