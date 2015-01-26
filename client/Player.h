#pragma once
#include "GameObject.h"

namespace Tmpl8
{
  class Surface;
}

class Player : public GameObject
{
public:
  Player(float a_xpos, float a_ypos, float a_speed, char* a_file, Client* a_Client);
  ~Player();

  virtual void Update(float a_dt);
  virtual void Render(Tmpl8::Surface* a_Surface);

  Tmpl8::Surface* m_healthEmpty, *m_healthFull;
};

