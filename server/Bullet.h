#pragma once
#include "GameObject.h"

class Bullet 
  : public GameObject
{
public:
  Bullet();
  ~Bullet();

  virtual void Init(unsigned short a_unitDataId, GameData* a_data, GameFlow* a_flow);
  virtual void Update(float a_fDeltaTime);
  virtual void OnCollision(GameObject* other);

  bool dirIsSet;
  float distanceTravelled;
  float xdir, ydir;
};

