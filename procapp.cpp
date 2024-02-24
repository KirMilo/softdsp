#include "procapp.h"
#include "inputpacket.h"
#include "outputpacket.h"

#include <iostream>
using namespace std;

static bool procAppReceivePacket(ProcApp& app, InputPacket& packet);
static void procAppProcessing(ProcApp& app, InputPacket& input, OutputPacket& output);
static void procAppSendPacket(ProcApp& app, OutputPacket& packet);

static void fillingGroup(unsigned count,unsigned maxPos,unsigned localMax, unsigned start, unsigned end ,OutputPacket& output);

int procAppRun(ProcApp& app)
{
	InputPacket inputPacket;	
	OutputPacket outputPacket;
	
	while(procAppReceivePacket(app,inputPacket))
    {
	    procAppProcessing(app,inputPacket,outputPacket);
	    procAppSendPacket(app,outputPacket);
    }
	return 0;
}

static bool procAppReceivePacket(ProcApp& /*app*/, InputPacket& packet)
{
    cout << "count=";
	cin >> packet.count;
	if ( packet.count > INPUTPACKET_MAXCOUNT )
		return false;
	cout << "items=";
	for(unsigned i = 0; i<packet.count; ++i)
		cin >> packet.data[i].level;
	return true;
}

static void procAppProcessing(ProcApp& app, InputPacket& input, OutputPacket& output)
{
    unsigned start = 0;
    unsigned currentMaxPosition = 0;
    unsigned currentMax = 0;

    unsigned outCount = 0;
    unsigned flag = 0;

    for(unsigned i = 0; i < input.count; ++i){
        
        if(app.procConfig.A <= input.data[i].level && input.data[i].level <= app.procConfig.B){

            if (flag == 0){
                start = currentMaxPosition = i;
                currentMax = input.data[i].level;
                flag = 1;
            }

            else{
                if (input.data[i].level > currentMax){
                    currentMax = input.data[i].level;
                    currentMaxPosition = i;
                }
            }
        }
        else{
            if (flag == 1){

                fillingGroup(outCount, currentMaxPosition, currentMax, start, i-1, output);
                outCount++;
                flag = 0;
            }
        }
    }
    if (flag == 1){

        fillingGroup(outCount, currentMaxPosition, currentMax, start, input.count-1, output);
        outCount++;
    }
    output.count = outCount;
}

static void procAppSendPacket(ProcApp& /*app*/, OutputPacket& output)
{
    cout << "Output Packet:" << endl;
	for(unsigned i = 0; i < output.count; ++i){
		cout << i+1 << " group: (| position of local max: " << output.data[i].localMaxPosition << "| local max level: " << output.data[i].localMax;
        cout << "| start: " << output.data[i].positionStartItem << "| end: " << output.data[i].positionEndItem <<  "|) ";
	    cout << endl;
    }
    cout << "GG WP GL HF" << endl;
	return;
}

static void fillingGroup(unsigned count,unsigned maxPos,unsigned localMax, unsigned start, unsigned end ,OutputPacket& output){
    output.data[count].localMaxPosition = maxPos;
    output.data[count].localMax = localMax;
    output.data[count].positionStartItem = start;
    output.data[count].positionEndItem = end;
}