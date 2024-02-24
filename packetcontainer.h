#ifndef packetcontainer_h
#define packetcontainer_h

#include "packet.h"

struct PacketContainer
{
   static const unsigned BufferSize = 4;
   Packet packets[BufferSize];
   unsigned readIndex;
   unsigned writeIndex;
   volatile unsigned canRead;
   volatile unsigned canWrite;
   Packet trashPacket;
   bool toTrash;
};

void pcInit(PacketContainer* ic);
void pcDestroy(PacketContainer* ic);
Packet* pcStartReadPacket(PacketContainer* ic);
void pcFinishReadPacket(PacketContainer* ic);
Packet* pcStartWritePacket(PacketContainer* ic);
void pcFinishWritePacket(PacketContainer* ic);

#endif
