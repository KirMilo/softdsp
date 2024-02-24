#ifndef packet_h
#define packet_h

#define PACKET_MAXSIZE 4096

struct PacketHeader
{
    unsigned message;
    unsigned size;
};

struct Packet
{
   PacketHeader header;
   unsigned body[PACKET_MAXSIZE];
};

#endif
