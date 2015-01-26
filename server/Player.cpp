#include "Player.h"
#include "InputManager.h"
#include "GameFlow.h"
#include "GameData.h"

Player::Player()
{
  speed = 120.0f;
}


Player::~Player()
{
}

void Player::Update(float a_fDeltaTime)
{
  //Client* owner = GameServer::instance->GetClient(unit->owner);
  UnitData* unit = &data->unitData[unitDataId];
  if (unit->alive)
  {
    glm::vec2 accumVel;
    if (Input->IsKeyDown(unit->owner, SDLK_a))
    {
      accumVel.x -= 1;
    }
    if (Input->IsKeyDown(unit->owner, SDLK_d))
    {
      accumVel.x += 1;
    }
    if (Input->IsKeyDown(unit->owner, SDLK_w))
    {
      accumVel.y -= 1;
    }
    if (Input->IsKeyDown(unit->owner, SDLK_s))
    {
      accumVel.y += 1;
    }
    if (accumVel.x != 0 || accumVel.y != 0)
    {
      accumVel = glm::normalize(accumVel);

      unit->x += accumVel.x * a_fDeltaTime * speed;
      unit->y += accumVel.y * a_fDeltaTime * speed;
      unit->changed = true;
    }

    if (unit->x < 25) unit->x = 25;
    if (unit->x > 775) unit->x = 775;
    if (unit->y < 25) unit->y = 25;
    if (unit->y > 575) unit->y = 575;

    shootTimer += a_fDeltaTime;
    if (Input->IsMouseButtonDown(unit->owner, 0) && shootTimer > 0.1666f)
    {
      shootTimer = 0;
      flow->spawnRequests.push_back(std::pair<UnitType, unsigned short>(BULLET, unit->id));
    }
  }
}

void Player::OnCollision(GameObject* other)
{
  UnitData* otherUnit = &data->unitData[other->unitDataId];
  if ((otherUnit->type == ENEMY || otherUnit->type == BULLET) && (otherUnit->owner != data->unitData[unitDataId].owner))
  {
    data->unitData[unitDataId].health--;
    if (data->unitData[unitDataId].health > 0)
    {
      flow->HealthUpdate(unitDataId);
    }
    else
    {
      flow->Destroy(unitDataId);
    }
  }
}

void Player::Init(unsigned short a_unitDataId, GameData* a_data, GameFlow* a_flow)
{
  GameObject::Init(a_unitDataId, a_data, a_flow);
  a_data->unitData[a_unitDataId].health = 10;
  shootTimer = 0; 
}
