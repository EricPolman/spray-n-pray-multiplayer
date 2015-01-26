#include "Enemy.h"
#include "InputManager.h"
#include "GameFlow.h"
#include "GameData.h"

Enemy::Enemy()
{
  m_firetime = .5f;
}


Enemy::~Enemy()
{
}

void Enemy::Update(float a_fDeltaTime)
{
  UnitData* unit = &data->unitData[unitDataId];

  //set target
  glm::vec2 targetPos(unit->x, unit->y);
  
  if (target && !target->freed) // if available target (not freed mem)
  {
    UnitData* otherUnit = &data->unitData[target->unitDataId];

    if (((otherUnit->x - unit->x) * (otherUnit->x - unit->x)) + ((otherUnit->y - unit->y) * (otherUnit->y - unit->y)) > 100 * 100) //check if in firing range
    {
      //apply velocity in target direction
      glm::vec2 targetDir(unit->x, unit->y);
      targetDir = glm::vec2(otherUnit->x, otherUnit->y) - targetDir;
      targetDir = glm::normalize(targetDir);

      unit->x += (targetDir.x) * 80.0f * a_fDeltaTime;
      unit->y += (targetDir.y) * 80.0f * a_fDeltaTime;
      unit->changed = true;
    }
    else //start firing at player
    {
      m_firetime += a_fDeltaTime;
      if (m_firetime >= 0.75f)
      {
        m_firetime = 0;
        flow->spawnRequests.push_back(std::pair<UnitType, unsigned short>(BULLET, unitDataId));
      }
    }
  }
  else
  {
    float currDistance = 10000000;
    for (auto unit2 = flow->gameObjects.begin(); unit2 != flow->gameObjects.end(); ++unit2)
    {
      UnitData* unit2data = &data->unitData[(*unit2)->unitDataId];

      if (unit2data->type == PLAYER)
      {
        float dist = ((unit2data->x - unit->x) * (unit2data->x - unit->x)) + ((unit2data->y - unit->y) * (unit2data->y - unit->y));
        if (dist < currDistance)
        {
          currDistance = dist;
          target = *unit2;
        }
      }
    }
  }
}

void Enemy::OnCollision(GameObject* other)
{
  UnitData* unit = &data->unitData[unitDataId];
  UnitData* otherUnit = &data->unitData[other->unitDataId];
  if (otherUnit->type == PLAYER)
  {
    unit->health -= 1;
    flow->HealthUpdate(unitDataId);
  }
  else if (otherUnit->type == BULLET && otherUnit->owner != 0)
  {
    unit->health -= 1;
    if (unit->health > 0)
    {
      flow->HealthUpdate(unitDataId);
    }
    else
    {
      --flow->enemiesAlive;
      flow->Destroy(unitDataId);
    }
  }
}

void Enemy::Init(unsigned short a_unitDataId, GameData* a_data, GameFlow* a_flow)
{
  GameObject::Init(a_unitDataId, a_data, a_flow);
  a_data->unitData[a_unitDataId].health = 3;
  a_data->unitData[a_unitDataId].type = ENEMY;
  target = nullptr;
  ++flow->enemiesAlive;
}
