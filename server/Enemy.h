#pragma once
#include "GameObject.h"
class Enemy :
  public GameObject
{
public:
  Enemy();
  ~Enemy();

  virtual void Update(float a_fDeltaTime);
  virtual void OnCollision(GameObject* other);
  virtual void Init(unsigned short a_unitDataId, GameData* a_data, GameFlow* a_flow);

  GameObject* target;
  float m_firetime;
};

