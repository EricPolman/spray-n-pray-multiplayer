#pragma once
#include <list>
#include "UDPCommands.h"

class Client;
struct sockaddr_in;

class UDPMessageHandler
{
public:
  UDPMessageHandler();
  ~UDPMessageHandler();

  void ParseUDPMessage(char* a_buffer, unsigned int a_size, Client* a_Cli);
  void SendUDPMessage(char* a_buffer, Client* a_Cli);
  void InstantiateGameObject(NetworkInstantiatePacket* a_iPacket, Client* a_Cli);
  void SyncStatePacketHandler(SyncStatePacket* a_sPacket, Client* a_Cli);

private:
  std::list<unsigned short> m_Incommingsequences;
  std::list<unsigned short> m_Outgoingsequences;
  unsigned short m_lastReceivedSequence;
};

