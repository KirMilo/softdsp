#ifndef inputpacket_h
#define inputpacket_h

struct InputPacketItem 
{
	unsigned level;
};

#define INPUTPACKET_MAXCOUNT 100

struct InputPacket
{
	unsigned count; 
	InputPacketItem data[INPUTPACKET_MAXCOUNT];
};

#endif