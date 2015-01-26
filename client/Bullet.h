#pragma once
#include "GameObject.h"

class Bullet : public GameObject
{
public:
  Bullet(float a_xpos, float a_ypos, float a_xDir, float a_yDir, float a_speed, char* a_file, Client* a_Client);
  ~Bullet();

  virtual void Update(float a_dt);
};

