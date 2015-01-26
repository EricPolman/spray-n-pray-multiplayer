#pragma once
#include "UDPCommands.h"
#include <vector>
#include "Player.h"
#include "Bullet.h"
#include "Enemy.h"

#define PLAYER_RESOURCE_SIZE 32
#define BULLET_RESOURCE_SIZE 256
#define ENEMY_RESOURCE_SIZE 128

class GameData
{
public:
  GameData();
  ~GameData();

  unsigned short GetNextId();
  unsigned short AddUnit(UnitData& data);
  UnitData GetUnit(unsigned short id);
  unsigned int GetUnitCount() { return unitData.size(); }
  std::vector<UnitData> unitData;
  std::vector<unsigned short> freedUnits;

  Player* GetPlayerResource()
  {
    for (int i = 0; i < PLAYER_RESOURCE_SIZE; ++i)
    {
      if (playerResource[i].freed)
      {
        playerResource[i].freed = false;
        return &playerResource[i];
      }
    }
    return nullptr;
  }

  unsigned char _nextBulletId = 0;
  Bullet* GetBulletResource()
  {
    return &bulletResource[_nextBulletId++];
  }
  Enemy* GetEnemyResource()
  {
    for (int i = 0; i < ENEMY_RESOURCE_SIZE; ++i)
    {
      if (enemyResource[i].freed)
      {
        enemyResource[i].freed = false;
        return &enemyResource[i];
      }
    }
    return nullptr;
  }

  Player playerResource[PLAYER_RESOURCE_SIZE];
  Bullet bulletResource[BULLET_RESOURCE_SIZE];
  Enemy enemyResource[ENEMY_RESOURCE_SIZE];
};

