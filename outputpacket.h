#ifndef outputpacket_h
#define outputpacket_h

// ATD of output Item
struct OutputPacketItem
{
    unsigned localMaxPosition; //position of max item in group
    unsigned localMax; //max Item in group
    unsigned positionStartItem; //position of start item
    unsigned positionEndItem; // position of end item
};

#define OUTPUTPACKET_MAXCOUNT 100  //max count of items

// ATD of output packet of items
struct OutputPacketBody
{
	unsigned count; //count of items
	OutputPacketItem data[OUTPUTPACKET_MAXCOUNT];
};

#endif