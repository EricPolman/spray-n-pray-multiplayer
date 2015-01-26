#pragma once
#include <vector>
#include <queue>
#include "Client.h"
#include "MessageManager.h"
#include "GameData.h"
#include "GameFlow.h"
#include "Job.h"

class Room
{
  friend class UpdateRoomGameFlowJob;
  friend class ParseRoomUdpMessages;
  friend class SendRoomUdpMessages;
public:
  Room(char a_id, std::string a_name);
  ~Room();

  void Update(float a_fDeltaTime);

  std::vector<Client*> clients;

  void BroadcastChat(char* message, unsigned int size, Client*);
  void BroadcastChatAsServer(std::string message);
  unsigned short InstantiateGameObject(NetworkInstantiatePacket* packet);
  void AddUnitToSpawnQueue(unsigned short objId);
  void AddUnitToDestroyQueue(unsigned short objId);
  void AddUnitHealthUpdateQueue(unsigned short objId);

  void AddClient(Client* client);
  void RemoveClient(Client* client);
  void SyncStateToClient(Client* client);

  void StoreUDPMessage(char* a_buffer, unsigned int a_size);
  void HandleUDPMessage(char* a_buffer, unsigned int a_size);
  void BroadcastClientList();

  unsigned char sequenceCounter;
  char id;
  std::string name;
  UpdateRoomJob _updateJob;
private:
  MessageManager _msgManager;
  GameData _gameData;
  GameFlow _gameFlow;
  bool _synchronized;
  std::vector<unsigned short> _objectsToSpawn;
  std::map<unsigned char, std::vector<unsigned short>> _objectsToSync;
  std::vector<unsigned short> _objectsToDestroy;
  std::vector<unsigned short> _objectsHealthUpdate;

  float _sendTimer, _parseTimer, _gameTimer;

  // Message sending functions
  bool SynchronizeClients();
  void BufferSpawnObjectsMessages();
  void BufferDestroyObjectsMessages();
  void BufferHealthObjectsMessages();
  void BufferPositionObjectsMessages();
  void BroadcastWaitingMessages();

  unsigned int messagesSent;
  unsigned int messagesReceived;
  unsigned int bytesSent;
  unsigned int bytesReceived;
};

