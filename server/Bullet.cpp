#include "Bullet.h"
#include "InputManager.h"
#include "GameFlow.h"
#include "GameData.h"

Bullet::Bullet()
{
}


Bullet::~Bullet()
{
}

void Bullet::Update(float a_fDeltaTime)
{
  UnitData* unit = &data->unitData[unitDataId];

  unit->x += xdir * 350.0f * a_fDeltaTime;
  unit->y += ydir * 350.0f * a_fDeltaTime;

  distanceTravelled += 350.0f * a_fDeltaTime;
  if (distanceTravelled > 600)
  {
    unit->alive = false;
    flow->Destroy(unitDataId);
  }
}

void Bullet::OnCollision(GameObject* other)
{
  UnitData* unit = &data->unitData[unitDataId];
  UnitData* otherUnit = &data->unitData[other->unitDataId];

  if (otherUnit->owner == unit->owner) // 0 == 0 is enemy bullet
     return;

  if (otherUnit->type == PLAYER)
  {
    // You shot another player!
    unit->alive = false;
  }
  else
  {
    if (otherUnit->type == ENEMY)
    {
      // Give points for this yo
      flow->AwardPoints(100, unit->owner);
    }
    unit->alive = false;
  }
  flow->Destroy(unitDataId);
}

void Bullet::Init(unsigned short a_unitDataId, GameData* a_data, GameFlow* a_flow)
{
  GameObject::Init(a_unitDataId, a_data, a_flow);

  UnitData* unit = &data->unitData[unitDataId];
  distanceTravelled = 0;
  glm::vec2 dir = glm::normalize(glm::vec2(xdir, ydir));
  xdir = dir.x;
  ydir = dir.y;
}