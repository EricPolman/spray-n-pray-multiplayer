#pragma once
#include "GameObject.h"
class Player :
  public GameObject
{
public:
  Player();
  ~Player();

  virtual void Update(float a_fDeltaTime);
  virtual void OnCollision(GameObject* other);
  virtual void Init(unsigned short a_unitDataId, GameData* a_data, GameFlow* a_flow);

  float shootTimer;
  float speed;
};

