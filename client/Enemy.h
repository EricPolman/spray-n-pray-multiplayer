#pragma once
#include "Player.h"

namespace Tmpl8
{
  class Surface;
}

class Enemy : public GameObject
{
public:
  Enemy(float a_xpos, float a_ypos, float a_speed, char* a_file, Client* a_Client);
  ~Enemy();

  virtual void Update(float a_dt);

  Tmpl8::Surface* m_Texture1, *m_Texture2;
};

