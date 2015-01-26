#pragma once
#include "UDPCommands.h"
#include <map>
#include <time.h>

namespace Tmpl8
{
  class Surface;
}
class Client;
class MainScene;

class GameObject
{
public:

  virtual void Render(Tmpl8::Surface* a_Surface);
  virtual void Update(float a_DT) = 0;
  void Move(float a_x, float a_y);
  void CheckCollision(std::map<unsigned short, GameObject*> a_Gameobjects);
  void SetPosition(float a_x, float a_y);
  void SetID(unsigned short a_ID) { m_ID = a_ID; }
  unsigned short GetID(){ return m_ID; }

  float m_x, m_y;
  float m_prevX, m_prevY;
  float m_drawX, m_drawY;
  float m_xVel, m_yVel; //direction
  float m_speed;
  int m_width, m_height;
  Tmpl8::Surface* m_Texture;
  unsigned short m_ID;
  unsigned char m_Owner;
  unsigned char m_Health;
  float accumulatedX, accumulatedY;
  Client* m_Client;
  MainScene* m_Mainscene;
  UnitType m_unitType;

  //time stuff
  clock_t clockTime;
  clock_t endTime;
  float updateTime;
  float currTime;
};

