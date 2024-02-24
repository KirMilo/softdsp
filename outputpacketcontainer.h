#ifndef outputpacketcontainer_h
#define outputpacketcontainer_h

#include "outputpacket.h"

struct OutputPacketContainer
{
   static const unsigned BufferSize = 4;
   OutputPacket packets[BufferSize];
   unsigned readIndex;
   unsigned writeIndex;
   volatile unsigned canRead;
   volatile unsigned canWrite;
   OutputPacket trashPacket;
   bool toTrash;
};

void ocInit(OutputPacketContainer* oc);
void ocDestroy(OutputPacketContainer* oc);
OutputPacket* ocStartReadPacket(OutputPacketContainer* oc);
void ocFinishReadPacket(OutputPacketContainer* oc);
OutputPacket* ocStartWritePacket(OutputPacketContainer* oc);
void ocFinishWritePacket(OutputPacketContainer* oc);

#endif
