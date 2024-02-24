#ifndef inputpacket_h
#define inputpacket_h

struct InputPacketItem 
{
	int level;
};

#define INPUTPACKET_MAXCOUNT 100

struct InputPacket
{
	unsigned count;
	InputPacketItem data[INPUTPACKET_MAXCOUNT];
};

#endif