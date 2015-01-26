#include "Room.h"
#include "UDPCommands.h"
#include "GameServer.h"
#include <algorithm>
#include "InputManager.h"
#include <iostream>
#include "JobDelegator.h"


Room::Room(char a_id, std::string a_name)
: id(a_id), name(a_name), _sendTimer(0), _synchronized(true), _gameFlow(this), 
_parseTimer(0), _gameTimer(0), _updateJob(this)
{
  _gameFlow.Initialise(&_gameData);
  std::cout << "Started room \"" << name << "\"\n";
  messagesSent = 0;
  messagesReceived = 0;
  bytesSent = 0;
  bytesReceived = 0;
}

Room::~Room()
{
  printf("\nRoom\n");
  printf("Messages sent:    %u\n", messagesSent);
  printf("Bytes sent:    %u\n", bytesSent);
  printf("Messages received:    %u\n", messagesReceived);
  printf("Bytes received:    %u\n\n", bytesReceived);
  std::cout << "Deleting room \"" << name << "\"\n";
}

void Room::StoreUDPMessage(char* a_buffer, unsigned int a_size)
{
  bytesReceived += a_size;
  ++messagesReceived;

  memcpy(_msgManager._processBuffer + _msgManager.processBufferPosition, a_buffer, a_size);
  _msgManager.processBufferPosition += a_size;
}


void Room::HandleUDPMessage(char* a_buffer, unsigned int a_size)
{
  char* bufferPointer = a_buffer;
  while (bufferPointer < a_buffer + a_size)
  {
    BlankPacket* blankPacket = (BlankPacket*)bufferPointer;
    unsigned char messageSize = blankPacket->header.messageSize;

    Client* client = GameServer::instance->GetClient(blankPacket->header.id);
    if (!client)
    {
      bufferPointer += messageSize;
      continue;
    }
    unsigned short sequenceCounter = blankPacket->header.sequenceCounter;

    switch (blankPacket->header.command)
    {
    case UDPCommand::SET_POSITION:
    {
      SetPositionPacket* pPacket = (SetPositionPacket*)bufferPointer;
      unsigned int structCount = (pPacket->header.messageSize - sizeof(UdpPacketHeader)) / 8;
      SetPositionStruct* structs = (SetPositionStruct*)(bufferPointer + 4);
      for (unsigned int i = 0; i < structCount; ++i)
      {
        SetPositionStruct& setPosStruct = structs[i];
        unsigned short objId = setPosStruct.GetId();
        _gameData.unitData[objId].x = setPosStruct.GetX();
        _gameData.unitData[objId].y = setPosStruct.GetY();
        _gameData.unitData[objId].changed = true;
      }
      break;
    }
    case UDPCommand::TRANSLATE:
    {
      SetPositionPacket* pPacket = (SetPositionPacket*)bufferPointer;
      unsigned int structCount = (pPacket->header.messageSize - sizeof(UdpPacketHeader)) / 8;
      SetPositionStruct* structs = (SetPositionStruct*)(bufferPointer + 4);

      for (unsigned int i = 0; i < structCount; ++i)
      {
        SetPositionStruct& setPosStruct = structs[i];
        unsigned short objId = setPosStruct.GetId();
        _gameData.unitData[objId].x += setPosStruct.GetX();
        _gameData.unitData[objId].y += setPosStruct.GetY();
        _gameData.unitData[objId].changed = true;
      }
      break;
    }
    case UDPCommand::NETWORK_INSTANTIATE:
    {
      NetworkInstantiatePacket* pPacket = (NetworkInstantiatePacket*)bufferPointer;

      _gameFlow.spawnRequests.push_back(std::pair<UnitType, unsigned short>(PLAYER, client->id));
      break;
    }
    case UDPCommand::RECEIVED_UPDATE:
    {
      ReceivedServerUpdatePacket* pPacket = (ReceivedServerUpdatePacket*)bufferPointer;
      client->lastKnownSequenceId = pPacket->sequenceId;
    }
    break;
    case UDPCommand::CLIENT_INPUT:
    {
      InputPacket* pPacket = (InputPacket*)bufferPointer;
      client->sequenceCount = pPacket->header.sequenceCounter;
      if (pPacket->state == 0) // Down
      {
        Input->SetKeyPressed(client->id, (Keycode)pPacket->input);
      }
      else
      {
        Input->SetKeyUp(client->id, (Keycode)pPacket->input);
      }
    }
      break;

    case UDPCommand::MOUSE_INPUT:
    {
      MouseInputPacket* pPacket = (MouseInputPacket*)bufferPointer;
      client->sequenceCount = pPacket->header.sequenceCounter;
      if (pPacket->state == 0) // Down
      {
        Input->SetMouseButtonPressed(client->id, 0);
        Input->SetMousePosition(client->id, pPacket->x, pPacket->y);
      }
      else if (pPacket->state == 1)
      {
        Input->SetMouseButtonUp(client->id, 0);
        Input->SetMousePosition(client->id, pPacket->x, pPacket->y);
      }
      else if (pPacket->state == 2)
        Input->SetMousePosition(client->id, pPacket->x, pPacket->y);
    }
      break;
    }
    bufferPointer += messageSize;
  }
}

void Room::BroadcastChat(char* message, unsigned int size, Client* client)
{
  static char broadcastMsg[256];
  sprintf_s(broadcastMsg, "%16s: %s\n", client->name.c_str(), message);
  printf(broadcastMsg, "%16s: %s\n", client->name.c_str(), message);
  for (auto i : clients)
  {
    i->SendServerMessage(broadcastMsg);
  }
}

void Room::BroadcastChatAsServer(std::string message)
{
  for (auto i : clients)
  {
    i->SendServerMessage("/ssay " + message);
  }
}

unsigned short Room::InstantiateGameObject(NetworkInstantiatePacket* packet)
{
  return _gameFlow.InstantiateGameObject(packet);
}

static SetPositionStruct posStructs[30];
static const float GAMEFLOW_UPDATE_TIME = 0.05f;
static const float MESSAGE_PARSE_TIME =   0.02f;
static const float SEND_MESSAGES_TIME =   0.02f;
void Room::Update(float a_fDeltaTime)
{
  _sendTimer += a_fDeltaTime;
  _parseTimer += a_fDeltaTime;
  _gameTimer += a_fDeltaTime;

  if (_gameTimer > GAMEFLOW_UPDATE_TIME)
  {
    _gameFlow.Update(GAMEFLOW_UPDATE_TIME);
    _gameTimer -= GAMEFLOW_UPDATE_TIME;
  }

  if (_parseTimer > MESSAGE_PARSE_TIME)
  {
    if (_msgManager.processBufferPosition > 0)
    {
      HandleUDPMessage(_msgManager._processBuffer, _msgManager.processBufferPosition);
      _msgManager.ClearProcessBuffer();
    }
    _parseTimer -= MESSAGE_PARSE_TIME;
  }

  if (_sendTimer > SEND_MESSAGES_TIME)
  {
    BroadcastWaitingMessages();
    _sendTimer -= SEND_MESSAGES_TIME;
  }
}

void Room::AddClient(Client* client)
{
  client->score = 0;
  std::string msg = client->name;
  msg.append(" has entered the room.");
  BroadcastChatAsServer(msg);

  clients.push_back(client);
  client->stateSyncedInRoom = false;
  char retmsg[128];
  sprintf_s(retmsg, "/joinedroom %s", name.c_str());


  client->SendServerMessage(retmsg);
  client->state = IN_ROOM;
  client->roomId = id;
  BroadcastClientList();

  SyncStateToClient(client);

  // Spawn player
  // Only time we're gonna use client id/owner as requester
  _gameFlow.spawnRequests.push_back(std::pair<UnitType, unsigned short>(PLAYER, client->id)); 
}

void Room::RemoveClient(Client* a_cli)
{
  for (auto obj = _gameData.unitData.begin(); obj != _gameData.unitData.end(); ++obj)
    if (obj->owner == a_cli->id)
      _objectsToDestroy.push_back(obj->id);

  if (clients.size() > 0)
  {
    for (auto cli = clients.begin(); cli != clients.end(); ++cli)
    {
      if (*cli == a_cli)
      {
        clients.erase(cli);
        break;
      }
    }
  }

  if (clients.empty())
  {
    GameServer::instance->RequestDeleteRoom(id);
  }
  else
  {
    std::string msg = a_cli->name;
    msg.append(" has left the room.");
    BroadcastChatAsServer(msg);
    BroadcastClientList();
  }
}

// Sync 10 units per packet
void Room::SyncStateToClient(Client* client)
{
  // sleep here for a few milliseconds?
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  client->stateSyncedInRoom = false;
  unsigned int unitsToSync = 0;
  std::vector<unsigned short> syncIds;
  for (auto iter = _gameData.unitData.begin(); iter != _gameData.unitData.end(); ++iter)
  {
    if (iter->alive)
    {
      ++unitsToSync;
      syncIds.push_back(iter->id);
    }
  }
  unsigned int numberOfMessages = (unitsToSync + 9) / 10;
  client->lastKnownSequenceId = sequenceCounter - numberOfMessages;

  for (unsigned int i = 0; i < numberOfMessages; ++i)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    _msgManager.ClearBuffer();
    unsigned int currentlyPreparedForSync = 0;
    while (currentlyPreparedForSync < 10 && !syncIds.empty())
    {
      UnitData& unit = _gameData.unitData[*syncIds.begin()];
      SyncStatePacket packet(0, sequenceCounter, unit.id, unit);
      _msgManager.BufferMessage((char*)&packet, packet.header.messageSize);
      ++currentlyPreparedForSync;
      syncIds.erase(syncIds.begin());
    }

    ReceivedServerUpdatePacket recPack(0, sequenceCounter - numberOfMessages + i + 1, sequenceCounter - numberOfMessages + i + 1, _gameFlow.state);
    _msgManager.BufferMessage((char*)&recPack, recPack.header.messageSize);

    _msgManager.FlushToClient(client);
  }

  client->stateSyncedInRoom = true;
}

void Room::AddUnitToDestroyQueue(unsigned short objId)
{
  for (auto it = _objectsToDestroy.begin(); it != _objectsToDestroy.end(); it++)
  {
    if (*it == objId)
    {
      return;
    }
  }

  _objectsToDestroy.push_back(objId);
}

void Room::AddUnitToSpawnQueue(unsigned short objId)
{
  _objectsToSpawn.push_back(objId);
}

void Room::AddUnitHealthUpdateQueue(unsigned short objId)
{
  _objectsHealthUpdate.push_back(objId);
}

void Room::BroadcastClientList()
{
  std::string msg = "/clilist ";

  for (auto cli : clients)
  {
    char namebuff[19];
    sprintf_s(namebuff, "%-18s", cli->name.c_str());
    msg.append(namebuff);
    msg.append(std::to_string(cli->score));
    msg += (char)0xFF;
  }

  for (auto cli : clients)
  {
    cli->SendServerMessage(msg);
  }
}

bool Room::SynchronizeClients()
{
  bool synced = true;
  for (auto client : clients)
  {
    if (client->lastKnownSequenceId != sequenceCounter)
    {
      synced = false;
      if (client->stateSyncedInRoom)
      {
        client->syncTimer += SEND_MESSAGES_TIME;
        if (client->syncTimer > 0.1f)
        {
          client->syncTimer = 0;
          bytesSent += _msgManager.sendBufferPosition;
          _msgManager.FlushToClient(client);
        }
      }
    }
  }
  return synced;
}
void Room::BufferSpawnObjectsMessages()
{
  for (auto iter = _objectsToSpawn.begin(); iter != _objectsToSpawn.end();)
  {
    if (_msgManager.sendBufferPosition > 480)
      break;

    UnitData& unit = _gameData.unitData[*iter];
    if (!unit.alive)
    {
      iter = _objectsToSpawn.erase(iter);
      continue;
    }
    NetworkInstantiatePacket packet(0, sequenceCounter, *iter);
    packet.header.id = 0;
    packet.header.sequenceCounter = sequenceCounter;
    packet.header.command = NETWORK_INSTANTIATE;

    packet.unitType = unit.type;
    BulletData data;
    switch (unit.type)
    {
    case BULLET:
    {
      data.xpos = unit.x;
      data.ypos = unit.y;
      data.owner = unit.owner; 
      data.xdir = cos(unit.rotation);
      data.ydir = sin(unit.rotation);
      *(BulletData*)packet.data = data;
      break;
    }
    case PLAYER:
      data.xpos = unit.x;
      data.ypos = unit.y;
      data.owner = unit.owner;
      *(BulletData*)packet.data = data;
      break;
    case ENEMY:
      data.xpos = unit.x;
      data.ypos = unit.y;
      data.owner = unit.owner;
      *(BulletData*)packet.data = data;
      break;
    }
    memcpy(packet.data, &data, sizeof(BulletData));
    packet.header.messageSize = sizeof(BulletData)+sizeof(NetworkInstantiatePacket)-32;

    _msgManager.BufferMessage((char*)&packet, packet.header.messageSize);
    ++messagesSent;
    iter = _objectsToSpawn.erase(iter);
  }
}
void Room::BufferDestroyObjectsMessages()
{
  for (auto iter = _objectsToDestroy.begin(); iter != _objectsToDestroy.end();)
  {
    if (_msgManager.sendBufferPosition > 480)
      break;

    DestroyObjectPacket packet(0, sequenceCounter, *iter);
    _msgManager.BufferMessage((char*)&packet, packet.header.messageSize);
    _gameFlow.RemoveGameObjectForUnit(*iter);
    _gameData.unitData[*iter].alive = false;
    if (std::find(_gameData.freedUnits.begin(), _gameData.freedUnits.end(), *iter) == _gameData.freedUnits.end())
      _gameData.freedUnits.push_back(*iter);
    ++messagesSent;
    iter = _objectsToDestroy.erase(iter);
  }
}
void Room::BufferHealthObjectsMessages()
{
  for (auto iter = _objectsHealthUpdate.begin(); iter != _objectsHealthUpdate.end();)
  {
    if (_msgManager.sendBufferPosition > 480)
      break;

    UnitData& unit = _gameData.unitData[*iter];
    SetHealthPacket packet(0, sequenceCounter, unit.id, (unsigned char)unit.health);
    _msgManager.BufferMessage((char*)&packet, packet.header.messageSize);
    ++messagesSent;
    iter = _objectsHealthUpdate.erase(iter);
  }
}
void Room::BufferPositionObjectsMessages()
{
  const unsigned int size = _gameData.unitData.size();
  for (unsigned int i = 0; i < size; ++i)
  {
    UnitData* data = &_gameData.unitData[i];
    if (data->changed && data->alive)
    {
      if (std::find(_objectsToSync[data->owner].begin(), _objectsToSync[data->owner].end(), data->id) == _objectsToSync[data->owner].end())
      {
        _objectsToSync[data->owner].push_back(data->id);
      }
    }
  }

  bool clearSync = false;
  // Max IP packet reassembly size is 576, to leave room for more headers, we stop at ~500
  while (_msgManager.sendBufferPosition < 480 && !clearSync)
  {
    if (!_objectsToSync.empty())
    {
      unsigned int curPosStruct = 0;
      for (auto vec = _objectsToSync.begin(); vec != _objectsToSync.end(); ++vec)
      {
        if (!vec->second.empty())
        {
          for (auto u = vec->second.begin(); u != vec->second.end();)
          {
            if (_msgManager.sendBufferPosition > 480 || curPosStruct > 29)
            {
              break;
            }
            UnitData& unit = _gameData.unitData[*u];
            SetPositionStruct& thisUnitsStruct = posStructs[curPosStruct++];
            thisUnitsStruct.Set(unit.id, unit.x, unit.y);
            unit.changed = false;
            u = vec->second.erase(u);
          }
        }
        if (curPosStruct > 0)
        {
          SetPositionPacket posPacket(0, sequenceCounter);
          if (vec->first != 0)
            posPacket.header.sequenceCounter = GameServer::instance->GetClient(vec->first)->sequenceCount;
          posPacket.header.messageSize = sizeof(SetPositionPacket)+curPosStruct * sizeof(SetPositionStruct);
          _msgManager.BufferMessage((char*)&posPacket, sizeof(SetPositionPacket));
          _msgManager.BufferMessage((char*)&posStructs, curPosStruct * sizeof(SetPositionStruct));
          curPosStruct = 0;
        }
      }

      if (curPosStruct == 0)
      {
        ++messagesSent;
        clearSync = true;
      }
    }
    else
    {
      break;
    }
  }

  if (clearSync)
    _objectsToSync.clear();
}

void Room::BroadcastWaitingMessages()
{

  // Check if in sync
  _synchronized = SynchronizeClients();
  if (!_synchronized)
    return;

  _msgManager.ClearBuffer();
  ++sequenceCounter;

  BufferDestroyObjectsMessages();
  BufferHealthObjectsMessages();
  BufferSpawnObjectsMessages();
  BufferPositionObjectsMessages();

  if (_msgManager.sendBufferPosition > 0)
  {
    ReceivedServerUpdatePacket recPack(0, sequenceCounter, sequenceCounter, _gameFlow.state);
    _msgManager.BufferMessage((char*)&recPack, recPack.header.messageSize);
    bytesSent += _msgManager.sendBufferPosition * clients.size();
    _msgManager.FlushToRoom(this);
  }
  else
  {
    --sequenceCounter;
  }
}
