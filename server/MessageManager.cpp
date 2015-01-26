#include "MessageManager.h"
#include "Room.h"
#include "GameServer.h"

MessageManager::~MessageManager()
{
}


void MessageManager::BufferMessage(char* message, unsigned int size)
{
  SetPositionStruct* str = (SetPositionStruct*)(message) + 1;

  // Buffer it.
  memcpy(&_sendBuffer[sendBufferPosition], message, size);
  sendBufferPosition += size;
}


void MessageManager::FlushToRoom(Room* room)
{
  if (sendBufferPosition > 0)
  {
    for (auto i : room->clients)
    {
      sendto(GameServer::instance->_udpSocket, _sendBuffer, sendBufferPosition, 0, (sockaddr*)&i->udpAddress, sizeof(i->udpAddress));
    }
  }
}

void MessageManager::FlushToClient(Client* client)
{
  if (sendBufferPosition > 0)
  {
    sendto(GameServer::instance->_udpSocket, _sendBuffer, sendBufferPosition, 0, (sockaddr*)&client->udpAddress, sizeof(client->udpAddress));
  }
}
