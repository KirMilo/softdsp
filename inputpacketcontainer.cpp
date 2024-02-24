#include "inputpacketcontainer.h"

void icInit(InputPacketContainer* ic) {
   
	ic->canRead = 0;
	ic->canWrite = InputPacketContainer::BufferSize;
	ic->readIndex = 0;
	ic->writeIndex = 0;
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
   
   //ожидаем свободной для записи области памяти
   while ( !ic->canWrite )
      continue;
   //блокируем область пакета
   ic->canWrite--;
   
   InputPacket* ret = ic->packets + ic->writeIndex;
   //переключить буфер
   ++ic->writeIndex;
   if ( ic->writeIndex == InputPacketContainer::BufferSize )
      ic->writeIndex = 0;
   
   return ret;   
}

void icFinishWritePacket(InputPacketContainer* ic) {
   
   //пакет готов
   ic->canRead++;
   return;
}
