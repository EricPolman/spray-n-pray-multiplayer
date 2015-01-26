#pragma once
#include <map>
#include <vector>
#include "Client.h"

class ClientManager
{
public:
  ClientManager();
  ~ClientManager();

  void Update(float a_fDeltaTime, fd_set* a_readfds);
  void FillReadFDS(fd_set* rfds);
  void Shutdown();

  void HandleTCPMessage(Client* client, char*, unsigned int);
  bool AddClient(SOCKET, sockaddr_in);
  void RemoveClient(Client* client);
  bool ClientExists(unsigned char id){ return _clients.find(id) != _clients.end(); }
  unsigned char GetClientId(SOCKET s) { return _clientIDs[s]; }
  Client* GetClient(unsigned char id) { return _clients.find(id) != _clients.end() ? _clients[id] : nullptr; }
  std::map<unsigned char, Client*>& GetClientMap() { return _clients; }

  unsigned int messagesSent;
  unsigned int messagesReceived;
  unsigned int bytesSent;
  unsigned int bytesReceived;

private:
  std::map<SOCKET, unsigned char> _clientIDs;
  std::map<unsigned char, Client*> _clients;
  Client _clientVec[256];
  std::vector<unsigned char> _clientsToRemove;
  std::vector<unsigned char> _freedClients;

  unsigned char _clientId;
  unsigned char NextClientId()
  {
    // TODO: Fix occupied ID issue
    if (_freedClients.empty())
      return _clientId++;
    else
    {
      unsigned char id = *_freedClients.begin();
      _freedClients.erase(_freedClients.begin());
      return id;
    }
  }
};

