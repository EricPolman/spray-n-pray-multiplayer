#include "ClientManager.h"
#include "GameServer.h"
#include "InputManager.h"

ClientManager::ClientManager()
: _clientId(1)
{
  messagesSent = 0;
  messagesReceived = 0;
  bytesSent = 0;
  bytesReceived = 0;
}


ClientManager::~ClientManager()
{
}

void ClientManager::FillReadFDS(fd_set* rfds)
{
  for (auto client : _clients)
  {
    FD_SET(client.second->tcpSocket, rfds);
  }
}

static char buffer[4096];
static int messagesReceived;
void ClientManager::Update(float a_fDeltaTime, fd_set* a_readfds)
{
  for (auto client : _clients)
  {
    if (FD_ISSET(client.second->tcpSocket, a_readfds))
    {
      memset(&buffer, 0, 4096);
      int messageSize = 0;

      messageSize += recv(client.second->tcpSocket, &(buffer[messageSize]), 256 - messageSize, 0);
      if (messageSize <= 0)
      {
        // Connection lost
        printf("Lost connection to %s (ID:%u)\n", client.second->name.c_str(), client.second->id);
        RemoveClient(client.second);
        continue;
      }
      else
      {
        bytesReceived += messageSize;
        char* p = buffer;
        while (p < buffer + messageSize)
        {
          HandleTCPMessage(client.second, p, strlen(p)+1);
          p += strlen(p) + 1;
          ++messagesReceived;
        }
      }
    }
  }

  for (auto i : _clientsToRemove)
  { // Remove disconnected TCP clients
    Client* cl = _clients[i];
    Input->RemoveClient(cl->id);
    cl->Disconnect();
    if(cl->state == IN_ROOM) 
      GameServer::instance->GetRoom(cl->roomId)->RemoveClient(cl);
    if (cl->state == IN_LOBBY)
      GameServer::instance->GetLobby()->Leave(cl);
    _clients.erase(i);

    _freedClients.push_back(cl->id);
  }
  _clientsToRemove.clear();
}

void ClientManager::Shutdown()
{
  for (auto i : _clientVec)
    i.Disconnect();
}

bool ClientManager::AddClient(SOCKET socket, sockaddr_in address)
{
  inet_ntop(AF_INET, &(address.sin_addr), buffer, INET_ADDRSTRLEN);

  printf("Connection from %s:%d\n", buffer, ntohs(address.sin_port));

  // Get ClientID
  unsigned char id = NextClientId();
  if (id == 0)
  {
    // Server full, send message and close connection
    return false;
  }
  else
  {
    Client client;
    client.id = id;
    sprintf_s(client.ipAddress, "%s", buffer);
    client.name = "Unnamed";
    client.tcpAddress = address;
    client.tcpSocket = socket;
    _clientVec[id] = client;

    _clients.insert(std::pair<unsigned char, Client*>(id, &(_clientVec[id])));
    _clientIDs.insert(std::pair<SOCKET, unsigned char>(socket, id));

    // Send back ClientID
    char setIdStr[9] = "/setid ";
    setIdStr[7] = *(char*)&id;
    setIdStr[8] = '\0';
    _clientVec[id].SendServerMessage(setIdStr);
    GameServer::instance->GetLobby()->Join(&_clientVec[id]);
    return true;
  }
}
void ClientManager::RemoveClient(Client* a_client)
{
  _clientsToRemove.push_back(a_client->id);
}

void ClientManager::HandleTCPMessage(Client* client, char* bufferPointer, unsigned int msgSize)
{
  if (strncmp((const char*)bufferPointer, "/say ", 5) == 0)
  {
    const char* msg = bufferPointer + 4;
    if (client->state == IN_ROOM)
    {
      GameServer::instance->GetRoom(client->roomId)->BroadcastChat((char*)msg, strlen(msg) + 1, client);
    }
    else if (client->state == IN_LOBBY)
    {
      GameServer::instance->GetLobby()->BroadcastChat((char*)msg, strlen(msg) + 1, client);
    }
  }
  else if (strncmp((const char*)bufferPointer, "/name ", 6) == 0)
  {
    const char* msg = bufferPointer + 6;
    auto oldname = client->name;
    client->name = msg;
    if (client->name.size() > 16)
      client->name.resize(16);

    char newConBuff[128];
    sprintf_s(newConBuff, "%s has renamed to %s\n", oldname.c_str(), client->name.c_str());
    printf(newConBuff);
    for (auto i : _clients)
    {
      i.second->SendServerMessage(newConBuff);
    }

    if (client->state == IN_LOBBY)
    {
      GameServer::instance->GetLobby()->BroadcastClientList();
    }
  }
  else if (strncmp((const char*)bufferPointer, "/joinroom ", 10) == 0)
  {
    const char* msg = bufferPointer + 10;
    GameServer::instance->JoinOrCreateRoom(msg, client);

  }
}

