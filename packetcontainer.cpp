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
   
   //ожидаем готовности пакета
   while ( !pc->canRead )
      continue;
   //блокируем область пакета
   pc->canRead--;
   
   Packet* ret = pc->packets + pc->readIndex;
   //переключить буфер
   ++pc->readIndex;
   if ( pc->readIndex == PacketContainer::BufferSize )
      pc->readIndex = 0;
   
   return ret;
}

void pcFinishReadPacket(PacketContainer* pc) {
   
   //область памяти стала доступной для записи очередного пакета
   pc->canWrite++;
   return;
}

Packet* pcStartWritePacket(PacketContainer* pc) {
   
   //выделяем свободную для записи область памяти
   //или направляем в мусорную корзину
   if ( pc->canWrite ) {
     --pc->canWrite;
     pc->toTrash = false;
   } else {
     pc->toTrash = true;
     return &pc->trashPacket;
   }

   
   Packet* ret = pc->packets + pc->writeIndex;
   //переключить буфер
   ++pc->writeIndex;
   if ( pc->writeIndex == PacketContainer::BufferSize )
      pc->writeIndex = 0;
   
   return ret;   
}

void pcFinishWritePacket(PacketContainer* pc) {
   
   //пакет готов
   if ( !pc->toTrash )
     ++pc->canRead;
      
   pc->toTrash = false;
   return;
}
