#ifndef outputpacket_h
#define outputpacket_h

// ATD of output Item
struct OutputPacketItem{
    
    unsigned numberItemMax; //number of max item in group
    int maxItem; //max Item in group
    unsigned numberStartItem; //number of start item
    unsigned numberEndItem; // number of end item
};

#define OUTPUTPACKET_MAXCOUNT 100  //max count of items

// ATD of output packet of items
struct OutputPacket
{
	unsigned count; //count of items
	OutputPacketItem data[OUTPUTPACKET_MAXCOUNT];
};

#endif