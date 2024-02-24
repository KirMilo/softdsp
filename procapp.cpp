#include "procapp.h"
#include "inputpacket.h"
#include "outputpacket.h"

static void procAppReceivePacket(ProcApp& app, InputPacket& packet);
static void procAppProcessing(ProcApp& app, InputPacket& input, OutputPacket& output);
static void procAppSendPacket(ProcApp& app, OutputPacket& packet);

int procAppRun(ProcApp& app)
{
	InputPacket inputPacket;	
	OutputPacket outputPacket;
	
	procAppReceivePacket(app,inputPacket);
	procAppProcessing(app,inputPacket,outputPacket);
	procAppSendPacket(app,outputPacket);
	return 0;
}

static void procAppReceivePacket(ProcApp& /*app*/, InputPacket& /*packet*/)
{

}

static void procAppProcessing(ProcApp& /*app*/, InputPacket& /*input*/, OutputPacket& /*output*/)
{

}

static void procAppSendPacket(ProcApp& /*app*/, OutputPacket& /*packet*/)
{

}

