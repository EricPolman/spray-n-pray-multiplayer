#include "GameFlow.h"
#include "Client.h"
#include "GameServer.h"
#include "Room.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Player.h"
#include "glm/glm.hpp"
#include "InputManager.h"

GameFlow::~GameFlow()
{
}

void GameFlow::Initialise(GameData* a_data)
{
  data = a_data;
  state = GAME_STARTING;
  startingTime = 0;
  gameOverTimer = 0;
}

float enemySpawnTimer = 0;
void GameFlow::Update(float a_fDeltaTime)
{
  ParseSpawnRequests();
  switch (state)
  {
  case GAME_STARTING:
    startingTime += a_fDeltaTime;
    if (startingTime >= 3.0f)
    {
      startingTime = 0;
      state = GAME_RUNNING;
    }
    break;
  case GAME_RUNNING:
    //spawn enemy
    if (enemiesAlive < 30)
    {
      enemySpawnTimer += a_fDeltaTime;
      if (enemySpawnTimer >= 1.0f)
      {
        spawnRequests.push_back(std::pair<UnitType, unsigned short>(ENEMY, 0));
        enemySpawnTimer = 0;
      }
    }
    CheckCollisions();

    for (auto obj : gameObjects)
    {
      obj->Update(a_fDeltaTime);
    }
    break;
  case GAME_OVER:
    gameOverTimer += a_fDeltaTime;
    if (gameOverTimer > 10)
    {
      for (auto cli : room->clients)
      {
        room->RemoveClient(cli);
        GameServer::instance->GetLobby()->Join(cli);
      }
    }
    break;
  }
}

void GameFlow::Pause()
{

}

void GameFlow::Stop()
{

}

void GameFlow::HandleInput(Client* client, char character, char state)
{
  
}

void GameFlow::RemoveGameObjectForUnit(unsigned short unitId)
{
  for (auto iter = gameObjects.begin(); iter != gameObjects.end(); ++iter)
  {
    if ((*iter)->unitDataId == unitId)
    {
      (*iter)->freed = true;
      gameObjects.erase(iter);
      break;
    }
  }
}

unsigned short GameFlow::InstantiateGameObject(NetworkInstantiatePacket* packet)
{
  BulletData& bdata = *(BulletData*)packet->data;
  printf("Unit owner: %u\n", bdata.owner);

  //printf("Position:  %f %f\n", packet->x, packet->y);
  printf("\n");

  unsigned short newObjectId = data->GetNextId();
  UnitData objectData;
  objectData.id = newObjectId;
  objectData.rotation = 0;
  objectData.type = packet->unitType;
  objectData.x = bdata.xpos;
  objectData.y = bdata.ypos;
  objectData.owner = bdata.owner;
  data->unitData[newObjectId] = objectData;

  GameObject* obj = nullptr;
  switch (packet->unitType)
  {
  case BULLET:
    obj = data->GetBulletResource();
    break;
  case ENEMY:
    obj = data->GetEnemyResource();
    break;
  case PLAYER:
    obj = data->GetPlayerResource();
    break;
  }
  obj->Init(newObjectId, data, this);
  gameObjects.push_back(obj);

  printf("Client ID: %hu\n", packet->header.id);
  printf("Sequence:  %hu\n", packet->header.sequenceCounter);
  printf("Command:   %u\n", packet->header.command);
  printf("Object ID: %hu\n", newObjectId);
  printf("Unit type: %hu\n", packet->unitType);
  return newObjectId;
}

void GameFlow::Destroy(unsigned short unitId)
{
  room->AddUnitToDestroyQueue(unitId);
}

bool ScoreBigger(Client* a, Client* b)
{
  return a->score >= b->score;
}

void GameFlow::AwardPoints(unsigned int points, unsigned char client)
{
  for (auto cli : room->clients)
  {
    if (cli->id == client)
    {
      cli->score += points;
      if (cli->score > 20000)
        state = GAME_OVER;
      break;
    }
  }

  // Sort here and determine if name list with scores needs to be updated.
  Client* prev = room->clients[0];
  bool updateList = false;

  unsigned int clientCount = room->clients.size();
  for (unsigned int i = 1; i < clientCount; ++i)
  {
    if (room->clients[i]->score > prev->score)
    {
      updateList = true;
    }
  }
  if (updateList)
  {
    std::sort(room->clients.begin(), room->clients.end(), ScoreBigger);
  }

  // Update scores
  room->BroadcastClientList();
}

void GameFlow::HealthUpdate(unsigned short unitId)
{
  room->AddUnitHealthUpdateQueue(unitId);
}

void GameFlow::ParseSpawnRequests()
{

  for (auto req : spawnRequests)
  {
    switch (req.first)
    {
    case BULLET:
    {
      UnitData objectData;
      objectData.type = BULLET;
      objectData.x = data->unitData[req.second].x;
      objectData.y = data->unitData[req.second].y;
      objectData.owner = data->unitData[req.second].owner;

      Bullet* b = data->GetBulletResource();

      if (data->unitData[req.second].type == PLAYER)
      {
        glm::vec2 mousePos = Input->GetMousePosition(data->unitData[req.second].owner);
        mousePos.x -= data->unitData[req.second].x;
        mousePos.y -= data->unitData[req.second].y;
        b->xdir = mousePos.x;
        b->ydir = mousePos.y;
      }
      else
      {
        Enemy* shootingEnemy = nullptr;
        for (auto iter = gameObjects.begin(); iter != gameObjects.end(); iter++)
        {
          if ((*iter)->unitDataId == req.second) // Enemy who shot the bullet
          {
            shootingEnemy = (Enemy*)*iter;
            break;
          }
        }
        if (shootingEnemy == nullptr || shootingEnemy->target == nullptr || !data->unitData[shootingEnemy->unitDataId].alive)
          break;

        b->xdir = data->unitData[shootingEnemy->target->unitDataId].x - data->unitData[req.second].x;
        b->ydir = data->unitData[shootingEnemy->target->unitDataId].y - data->unitData[req.second].y;
      }
      objectData.rotation = atan2(b->ydir, b->xdir);

      unsigned short newObjectId = data->GetNextId();
      objectData.id = newObjectId;

      data->unitData[newObjectId] = objectData;

      b->Init(newObjectId, data, this);
      gameObjects.push_back(b);
      room->AddUnitToSpawnQueue(newObjectId);
      break;
    }
    case ENEMY:
    {
      unsigned short newObjectId = data->GetNextId();
      UnitData objectData;
      objectData.id = newObjectId;
      objectData.type = ENEMY;
      if (rand() % 2 == 1)
      {
        if (rand() % 2 == 1) // spawn left
        {
          objectData.x = -50;
          objectData.y = (float)(rand() % 500) + 40.0f;
        }
        else //spawn right
        {
          objectData.x = 850;
          objectData.y = (float)(rand() % 500) + 40.0f;
        }
      }
      else
      {
        if (rand() % 2 == 1) // spawn top
        {
          objectData.x = (float)(rand() % 700) + 40.0f;
          objectData.y = -50;
        }
        else //spawn bottom
        {
          objectData.x = (float)(rand() % 700) + 40.0f;
          objectData.y = 650;
        }
      }
      objectData.owner = 0; // owned by server
      data->unitData[newObjectId] = objectData;
      objectData.rotation = 0;

      Enemy* e = data->GetEnemyResource();
      e->Init(newObjectId, data, this);
      gameObjects.push_back(e);
      room->AddUnitToSpawnQueue(newObjectId);
      break;
    }
    case PLAYER:
    {
      unsigned short newObjectId = data->GetNextId();
      UnitData objectData;
      objectData.id = newObjectId;
      objectData.type = PLAYER;
      objectData.x = 300.0f + (float)(rand() % 200);
      objectData.y = 250.0f + (float)(rand() % 100);
      objectData.owner = (unsigned char)req.second;
      data->unitData[newObjectId] = objectData;
      objectData.rotation = 0;
      Player* p = data->GetPlayerResource();
      p->Init(newObjectId, data, this);
      gameObjects.push_back(p);
      room->AddUnitToSpawnQueue(newObjectId);
    }
    }
  }
  spawnRequests.clear();
}

void GameFlow::CheckCollisions()
{
  //Collision checks
  static std::map<GameObject*, glm::vec2> accumulatedDisplacements;
  unsigned int vecSize = gameObjects.size();
  if (vecSize > 1)
  {
    for (unsigned int i = 0; i < vecSize - 1; ++i)
    {
      UnitData& unit1 = data->unitData[gameObjects[i]->unitDataId];

      if (!unit1.alive || gameObjects[i]->freed)
        continue;

      const float unit1x = unit1.x;
      const float unit1y = unit1.y;
      const float unit1radius = (unit1.type == PLAYER || unit1.type == ENEMY) ? 25.0f : 10.0f;

      for (unsigned int j = i + 1; j < vecSize; ++j)
      {
        UnitData& unit2 = data->unitData[gameObjects[j]->unitDataId];
        if (!unit2.alive || gameObjects[i]->freed)
          continue;

        //if (unit1.id == unit2.id)
        //  __debugbreak(); // Should never happen!

        const float unit2x = unit2.x;
        const float unit2y = unit2.y;
        const float unit2radius = (unit2.type == PLAYER || unit2.type == ENEMY) ? 25.0f : 10.0f;

        const float radiusSq = (unit1radius * unit1radius) + (unit2radius * unit2radius);
        const float xdiff = unit2x - unit1x, ydiff = unit2y - unit1y;
        const float distSq = (xdiff * xdiff) + (ydiff * ydiff);

        if (distSq < radiusSq) //circle collision check
        {
          gameObjects[i]->OnCollision(gameObjects[j]);
          gameObjects[j]->OnCollision(gameObjects[i]);

          if (unit1.type == ENEMY && unit2.type == ENEMY)
          {
            glm::vec2 diff = glm::vec2(unit1.x, unit1.y) - glm::vec2(unit2.x, unit2.y);
            diff *= 0.1f;
            accumulatedDisplacements[gameObjects[i]] += diff;
            accumulatedDisplacements[gameObjects[j]] -= diff;
            unit1.changed = true;
            unit2.changed = true;
          }
        }
      }
    }
  }
  for (auto obj : accumulatedDisplacements)
  {
    UnitData& unit = data->unitData[obj.first->unitDataId];
    unit.x += obj.second.x;
    unit.y += obj.second.y;
  }
  accumulatedDisplacements.clear();

}
