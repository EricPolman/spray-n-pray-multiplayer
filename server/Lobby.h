#pragma once
#include <vector>
#include <string>

class Client;
class Room;

class Lobby
{
public:
  Lobby();
  ~Lobby();

  void Join(Client*);
  void Leave(Client*);
  void SetName(Client*, std::string);
  void BroadcastChat(char* message, unsigned int size, Client*);
  void BroadcastMessage(std::string);
  void BroadcastRoomList();
  void BroadcastClientList();

  std::vector<Client*> clients;
  std::vector<Room*> activeRooms;
};

