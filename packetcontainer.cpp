#include "packetcontainer.h"

void pcInit(PacketContainer* pc) {
   
	pc->canRead = 0;
	pc->canWrite = PacketContainer::BufferSize;
	pc->readIndex = 0;
	pc->writeIndex = 0;
	pc->toTrash = false;
	return;
}

void pcDestroy(PacketContainer* /*pc*/) {
   
}

Packet* pcStartReadPacket(PacketContainer* pc) {
   
   //О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫
   while ( !pc->canRead )
      continue;
   //О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫
   pc->canRead--;
   
   Packet* ret = pc->packets + pc->readIndex;
   //О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫
   ++pc->readIndex;
   if ( pc->readIndex == PacketContainer::BufferSize )
      pc->readIndex = 0;
   
   return ret;
}

void pcFinishReadPacket(PacketContainer* pc) {
   
   //О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫
   pc->canWrite++;
   return;
}

Packet* pcStartWritePacket(PacketContainer* pc) {
   
   //О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫
   //О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫ О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫О©╫О©╫
   if ( pc->canWrite ) {
     --pc->canWrite;
     pc->toTrash = false;
   } else {
     pc->toTrash = true;
     return &pc->trashPacket;
   }

   
   Packet* ret = pc->packets + pc->writeIndex;
   //О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫
   ++pc->writeIndex;
   if ( pc->writeIndex == PacketContainer::BufferSize )
      pc->writeIndex = 0;
   
   return ret;   
}

void pcFinishWritePacket(PacketContainer* pc) {
   
   //О©╫О©╫О©╫О©╫О©╫ О©╫О©╫О©╫О©╫О©╫
   if ( !pc->toTrash )
     ++pc->canRead;
      
   pc->toTrash = false;
   return;
}
