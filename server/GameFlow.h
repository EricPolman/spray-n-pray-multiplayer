#pragma once
#include "GameData.h"
#include "GameObject.h"
#include "UDPCommands.h"

class Client;
class Room;
struct NetworkInstantiatePacket;

class GameFlow
{
public:
  GameFlow(Room* r) : 
    state(GAME_UNINITIALIZED), room(r), enemiesAlive(0)
  {}
  ~GameFlow();

  void Initialise(GameData*);
  void Update(float a_fDeltaTime);
  void Pause();
  void Stop();
  void HandleInput(Client*, char, char);
  void Destroy(unsigned short unitId);
  void HealthUpdate(unsigned short unitId);
  void RemoveGameObjectForUnit(unsigned short unitId);
  void AwardPoints(unsigned int, unsigned char);
  void ParseSpawnRequests();
  void CheckCollisions();
  unsigned short InstantiateGameObject(NetworkInstantiatePacket* packet);
  std::vector<GameObject*> gameObjects;
  std::vector<std::pair<UnitType, unsigned short>> spawnRequests;
  float startingTime;
  GameFlowState state;
  GameData* data;
  Room* room;

  int enemiesAlive;
  float gameOverTimer;
};

