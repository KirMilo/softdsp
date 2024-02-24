#include "inputpacketcontainer.h"

void icInit(InputPacketContainer* ic) {
   
	ic->canRead = 0;
	ic->canWrite = InputPacketContainer::BufferSize;
	ic->readIndex = 0;
	ic->writeIndex = 0;
	ic->toTrash = false;
	return;
}

void icDestroy(InputPacketContainer* /*ic*/) {
   
}

InputPacket* icStartReadPacket(InputPacketContainer* ic) {
   
   //ожидаем готовности пакета
   while ( !ic->canRead )
      continue;
   //блокируем область пакета
   ic->canRead--;
   
   InputPacket* ret = ic->packets + ic->readIndex;
   //переключить буфер
   ++ic->readIndex;
   if ( ic->readIndex == InputPacketContainer::BufferSize )
      ic->readIndex = 0;
   
   return ret;
}

void icFinishReadPacket(InputPacketContainer* ic) {
   
   //область памяти стала доступной для записи очередного пакета
   ic->canWrite++;
   return;
}

InputPacket* icStartWritePacket(InputPacketContainer* ic) {
   
   //выделяем свободную для записи область памяти
   //или направляем в мусорную корзину
   if ( ic->canWrite ) {
     --ic->canWrite;
     ic->toTrash = false;
   } else {
     ic->toTrash = true;
     return &ic->trashPacket;
   }

   
   InputPacket* ret = ic->packets + ic->writeIndex;
   //переключить буфер
   ++ic->writeIndex;
   if ( ic->writeIndex == InputPacketContainer::BufferSize )
      ic->writeIndex = 0;
   
   return ret;   
}

void icFinishWritePacket(InputPacketContainer* ic) {
   
   //пакет готов
   if ( !ic->toTrash )
     ++ic->canRead;
      
   ic->toTrash = false;
   return;
}
