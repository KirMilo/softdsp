#include "procapp.h"
#include "inputpacket.h"
#include "outputpacket.h"

#include <iostream>
using namespace std;

static bool procAppReceivePacket(ProcApp& app, InputPacket& packet);
static void procAppProcessing(ProcApp& app, InputPacket& input, OutputPacket& output);
static void procAppSendPacket(ProcApp& app, OutputPacket& packet);

static void fillingGroup(unsigned count,unsigned maxPos,unsigned localMax, unsigned start, unsigned end ,OutputPacket& output);

int procAppRun(ProcApp& app){

	InputPacket inputPacket;	
	OutputPacket outputPacket;
	
	while(procAppReceivePacket(app,inputPacket))
    {
	    procAppProcessing(app,inputPacket,outputPacket);
	    procAppSendPacket(app,outputPacket);
    }
	return 0;
}

static bool procAppReceivePacket(ProcApp& app, InputPacket& packet){

	//прием количества отсчетов
	int ret = read(app.readFd, &packet.count, sizeof(packet.count));
	if ( ret == 0 || ret == -1 )
		return false;
	if ( packet.count == 0 )
		return true;
	
	//прием набора отсчетов
	ret = read(app.readFd, packet.data, packet.count * sizeof(InputPacketItem));
	if ( ret == 0 || ret == -1 )
		return false;
	
	return true;
}

static void procAppProcessing(ProcApp& app, InputPacket& input, OutputPacket& output){

    unsigned start = 0;
    unsigned currentMaxPosition = 0;
    unsigned currentMax = 0;

    unsigned outCount = 0;
    unsigned startGroup = 0;

    for(unsigned i = 0; i < input.count; ++i){
        
        if(app.procConfig.A <= input.data[i].level && input.data[i].level <= app.procConfig.B){

            if (startGroup == 0){
                start = currentMaxPosition = i;
                currentMax = input.data[i].level;
                startGroup = 1;
            }

            else{
                if (input.data[i].level > currentMax){
                    currentMax = input.data[i].level;
                    currentMaxPosition = i;
                }
            }
        }
        else{
            if (startGroup == 1){

                fillingGroup(outCount, currentMaxPosition, currentMax, start, i-1, output);
                outCount++;
                startGroup = 0;
            }
        }
    }
    if (startGroup == 1){

        fillingGroup(outCount, currentMaxPosition, currentMax, start, input.count-1, output);
        outCount++;
    }
    output.count = outCount;
}

static void procAppSendPacket(ProcApp& app, OutputPacket& output){

    write(app.consumerFd, &output.count,sizeof(output.count));
	write(app.consumerFd, output.data, output.count * sizeof(OutputPacketItem));
	return;
}

static void fillingGroup(unsigned count, unsigned maxPos, unsigned localMax, unsigned start, unsigned end ,OutputPacket& output){
    
    output.data[count].localMaxPosition = maxPos;
    output.data[count].localMax = localMax;
    output.data[count].positionStartItem = start;
    output.data[count].positionEndItem = end;
}
