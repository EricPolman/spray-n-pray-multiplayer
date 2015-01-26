#pragma once
#include <map>
#include <WinSock2.h>
#include "ClientManager.h"
#include "MessageManager.h"
#include "Room.h"
#include "Lobby.h"
#include <string>

enum ServerState
{
  UNSTARTED,
  STARTING_UP,
  RUNNING,
  SHUTTING_DOWN,
  STOPPED
};

class GameServer
{
public:
  GameServer();
  ~GameServer();

  void Start(unsigned int maxTCPConnections = 8);
  void Shutdown();

  void Run();

  static GameServer* instance;

  void JoinOrCreateRoom(std::string name, Client* client);
  Lobby* GetLobby() { return &_lobby; }
  Room* GetRoom(char id) { return _rooms[id]; }
  const std::vector<Room*>& GetRooms() { return _rooms; }
  void RequestDeleteRoom(char id) { _pendingRoomDeletion.push_back(id); }
  Client* GetClient(unsigned char id) { return _clientManager.GetClient(id); }
  SOCKET _udpSocket;
  ClientManager _clientManager;
private:
  void AcceptTCPConnection();
  void HandleUDPMessage(char* a_buffer, unsigned int a_size, sockaddr_in& a_addr);
  void FilterSockets();

  void StartUDP();
  void StartTCP(unsigned int maxTCPConnections);

  SOCKET _tcpSocket;
  sockaddr_in _tcpAddress;

  sockaddr_in _udpAddress;
  sockaddr_in _udpClientAddress;

  ServerState _state;
  MessageManager _msgManager;

  fd_set _readfds;
  int _clientAddressSize;

  float _sendRate;
  std::vector<Room*> _rooms;
  std::vector<char> _roomsFreed;
  std::map<std::string, char> _roomIds;
  std::vector<char> _pendingRoomDeletion;
  Lobby _lobby;
};
