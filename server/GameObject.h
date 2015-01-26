#pragma once
class GameData;
class GameFlow;

class GameObject
{
public:
  GameObject();
  virtual ~GameObject();

  virtual void Init(unsigned short a_unitDataId, GameData* a_data, GameFlow* a_flow)
  {
    freed = false;
    available = false;
    unitDataId = a_unitDataId;
    data = a_data;
    flow = a_flow;
  }
  virtual void Update(float a_fDeltaTime) = 0;
  virtual void OnCollision(GameObject* other) = 0;
  
  unsigned short unitDataId;
  GameData* data;
  GameFlow* flow;
  bool freed, available;
};

