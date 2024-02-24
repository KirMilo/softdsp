#include "outputpacketcontainer.h"

void ocInit(OutputPacketContainer* oc) {
   
   oc->canRead = 0;
   oc->canWrite = OutputPacketContainer::BufferSize;
   oc->readIndex = 0;
   oc->writeIndex = 0;
   return;
}

void ocDestroy(OutputPacketContainer* /*oc*/) {
   
}

OutputPacket* ocStartReadPacket(OutputPacketContainer* oc) {
   
   //ожидаем готовности пакета
   while ( !oc->canRead )
      continue;
   //блокируем область пакета
   oc->canRead--;
   
   OutputPacket* ret = oc->packets + oc->readIndex;
   //переключить буфер
   ++oc->readIndex;
   if ( oc->readIndex == OutputPacketContainer::BufferSize )
      oc->readIndex = 0;
   
   return ret;
}

void ocFinishReadPacket(OutputPacketContainer* oc) {
   
   //область памяти стала доступной для записи очередного пакета
   oc->canWrite++;
   return;
}

OutputPacket* ocStartWritePacket(OutputPacketContainer* oc) {
   
   //ожидаем свободной для записи области памяти
   while ( !oc->canWrite )
      continue;
   //блокируем область пакета
   oc->canWrite--;
   
   OutputPacket* ret = oc->packets + oc->writeIndex;
   //переключить буфер
   ++oc->writeIndex;
   if ( oc->writeIndex == OutputPacketContainer::BufferSize )
      oc->writeIndex = 0;
   
   return ret;   
}

void ocFinishWritePacket(OutputPacketContainer* oc) {
   
   //пакет готов
   oc->canRead++;
   return;
}
