#include "consumerapp.h"
#include "outputpacket.h"
#include <unistd.h>
#include <cstdlib>
#include <iostream>
using namespace std;

static void consumerAppOutPacket(ConsumerApp& app, OutputPacket& output);
static bool consumerAppReceivePacket(ConsumerApp& app, OutputPacket& packet);

int consumerAppRun(ConsumerApp& app)
{
	OutputPacket packet;

	while (consumerAppReceivePacket(app,packet))
		consumerAppOutPacket(app,packet);
	return 0;
}

static void consumerAppOutPacket(ConsumerApp& /*app*/, OutputPacket& output) {
	
	cout << "Output Packet:" << endl;
	for(unsigned i = 0; i < output.count; ++i)
		cout << i+1 << " group: (| position of local max: " << output.data[i].localMaxPosition <<
		"| local max level: " << output.data[i].localMax << "| start: " <<
		 output.data[i].positionStartItem << "| end: " << output.data[i].positionEndItem <<  "|) " << endl;

	return;
}

static bool consumerAppReceivePacket(ConsumerApp& app, OutputPacket& packet)
{
	int ret = read(app.readFd, &packet.count, sizeof(packet.count));
	if ( ret == 0 || ret == -1 )
		return false;
	if ( packet.count == 0 )
		return true;
	ret = read(app.readFd, packet.data, packet.count * sizeof(OutputPacketItem));
	if ( ret == 0 || ret == -1 )
		return false;
	return true;
}
