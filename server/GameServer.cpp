#include "GameServer.h"
#include "UDPCommands.h"
#include <time.h>
#include "JobDelegator.h"
#include "InputManager.h"
#include <iostream>

const static unsigned short TCP_PORT = 3742;
const static unsigned short UDP_PORT = 2473;

GameServer* GameServer::instance;

GameServer::GameServer()
: _state(ServerState::UNSTARTED), _clientAddressSize(sizeof(_tcpAddress)),
_sendRate(0.1f)
{
  instance = this;
}


GameServer::~GameServer()
{
  for (auto room : _rooms)
  {
    delete room;
  }
}

void GameServer::Start(unsigned int a_maxTCPConnections)
{
  printf("\n   ************************************************* \n");
  printf("  **                                               **\n");
  printf(" **            SPRAY & PRAY - GAMESERVER            **\n");
  printf("  **                                               **\n");
  printf("   ************************************************* \n\n");
  printf("Starting server...\n");

  _state = ServerState::STARTING_UP;
  JobSys = new JobDelegator();
  JobSys->CreateThreads();

  // Windows stuff
  WSAData data;
  WSAStartup(MAKEWORD(2, 2), &data);
  
  StartTCP(a_maxTCPConnections);
  StartUDP();
  _state = ServerState::RUNNING;
}

void GameServer::Shutdown()
{
  printf("Shutting down server\n");
  _clientManager.Shutdown();
  closesocket(_tcpSocket);
  closesocket(_udpSocket);
  WSACleanup();
}

static char buffer[4096];
void GameServer::Run()
{
  UpdateClientManagerJob ucmj(&_clientManager, &_readfds);
  UpdateInputManagerJob uipm;

  clock_t startTime, endTime;
  clock_t clockTime;
  startTime = endTime = clockTime = 0;
  float fFrameTimer = 0;
  float fDeltaTime =  0;

  while (_state == ServerState::RUNNING)
  {
    endTime = clockTime;
    clockTime = clock();
    fDeltaTime = (float)(clockTime - endTime) / CLOCKS_PER_SEC;

    FilterSockets();
    if (FD_ISSET(_tcpSocket, &_readfds)) // If connection is pending
    {
      AcceptTCPConnection();
    }

    if (FD_ISSET(_udpSocket, &_readfds))
    {
      int messageSize = recvfrom(_udpSocket, buffer, 2048, 0, (sockaddr*)&_udpClientAddress, &_clientAddressSize);

      // Make job out of this?
      HandleUDPMessage(buffer, messageSize, _udpClientAddress);
      memset(buffer, 0, sizeof(buffer));
    }

    JobSys->Queue(&ucmj); // Update client manager

    // Job per room?
    for (auto room : _rooms)
    {
      if (room)
      {
        room->_updateJob.deltaTime = fDeltaTime;
        JobSys->Queue(&room->_updateJob);
      }
    }

    JobSys->Finish();
    Input->Update(fDeltaTime);

    if (!_pendingRoomDeletion.empty())
    {
      while (!_pendingRoomDeletion.empty())
      {
        Room* room = _rooms[*_pendingRoomDeletion.begin()];

        std::cout << "Cleaning up empty room \"" << room->name << "\"\n";
        _roomsFreed.push_back(room->id);
        for (auto it = _roomIds.begin(); it != _roomIds.end(); ++it)
        {
          if (it->second == room->id)
          {
            _roomIds.erase(it);
            break;
          }
        }
        _rooms[*_pendingRoomDeletion.begin()] = nullptr;
        delete room;
        _pendingRoomDeletion.erase(_pendingRoomDeletion.begin());
      }

      _lobby.BroadcastRoomList();
    }
  }
  JobSys->Terminate();
}

void GameServer::JoinOrCreateRoom(std::string name, Client* client)
{
  if (client->state == IN_ROOM)
    _rooms[client->roomId]->RemoveClient(client);
  else
    _lobby.Leave(client);  

  bool found = false;
  for (auto roomId : _roomIds)
    if (roomId.first == name)
      found = true;
   
  if (found)
  {
    _rooms[_roomIds[name]]->AddClient(client);
  }
  else
  {
    char id = 0;
    if (!_roomsFreed.empty())
    {
      id = *_roomsFreed.begin();
      _roomsFreed.erase(_roomsFreed.begin());
      _roomIds[name] = id;
      _rooms[id] = new Room(id, name);
    }
    else
    {
      id = _rooms.size();
      _roomIds[name] = id;
      _rooms.push_back(new Room(id, name));
    }
    _rooms[id]->AddClient(client);
  }

  _lobby.BroadcastRoomList();
}

void GameServer::FilterSockets()
{
  static timeval t;
  t.tv_usec = 500;
  FD_ZERO(&_readfds);
  FD_SET(_tcpSocket, &_readfds);
  FD_SET(_udpSocket, &_readfds);
  _clientManager.FillReadFDS(&_readfds);
  select(2, &_readfds, 0, 0, &t);
}


void GameServer::StartTCP(unsigned int maxTCPConnections)
{
  _tcpSocket = socket(AF_INET, SOCK_STREAM, 0);

  _tcpAddress.sin_family = AF_INET;
  _tcpAddress.sin_addr.s_addr = INADDR_ANY;
  _tcpAddress.sin_port = htons(TCP_PORT);

  bind(_tcpSocket, (sockaddr*)&_tcpAddress, sizeof(_tcpAddress));

  listen(_tcpSocket, maxTCPConnections);

  printf("Initialized TCP on port %hu\n", TCP_PORT);
}

void GameServer::StartUDP()
{
  _udpSocket = socket(AF_INET, SOCK_DGRAM, 0);

  _udpAddress.sin_family = AF_INET;
  _udpAddress.sin_addr.s_addr = INADDR_ANY;
  _udpAddress.sin_port = htons(UDP_PORT);

  bind(_udpSocket, (sockaddr*)&_udpAddress, sizeof(_udpAddress));
  printf("Initialized UDP on port %hu\n", UDP_PORT);
}

void GameServer::AcceptTCPConnection()
{
  sockaddr_in cliAddr;
  SOCKET sock = accept(_tcpSocket, (sockaddr*)&cliAddr, &_clientAddressSize);
  bool accepted = _clientManager.AddClient(sock, cliAddr);
}

void GameServer::HandleUDPMessage(char* a_buffer, unsigned int a_size, sockaddr_in& a_addr)
{
  char* bufferPointer = a_buffer;
  while (bufferPointer < a_buffer + a_size)
  {
    BlankPacket* blankPacket = (BlankPacket*)bufferPointer;
    unsigned char messageSize = blankPacket->header.messageSize;
    if (a_buffer + a_size - bufferPointer < messageSize)
      __debugbreak();

    if (_clientManager.ClientExists(blankPacket->header.id))
    {
      Client* client = _clientManager.GetClient(blankPacket->header.id);
      unsigned short sequenceCounter = blankPacket->header.sequenceCounter;
      //if (sequenceCounter != client->sequenceIncoming)
      //{
      //  // Packet lost. 
      //  __debugbreak();
      //}

      if (blankPacket->header.command == UDPCommand::SET_ADDRESS)
      {
        client->udpAddress = a_addr;
        client->isUdpSet = true;
      }
      else
      {
        if (!client->isUdpSet)
          client->udpAddress = a_addr, client->isUdpSet = true;

        if (client->state == IN_ROOM)
        {
          GetRoom(client->roomId)->StoreUDPMessage(bufferPointer, messageSize);
        }
      }
    }
    bufferPointer += messageSize;
  }
}
