#include "Enemy.h"
#include "Client.h"
#include <map>
#include "surface.h"


Enemy::Enemy(float a_xpos, float a_ypos, float a_speed, char* a_file, Client* a_Client)
{
  m_x = m_prevX = m_drawX = a_xpos;
  m_y = m_prevX = m_drawY = a_ypos;
  m_xVel = m_yVel = 0;
  m_speed = a_speed;
  m_Texture = new Tmpl8::Surface(a_file);
  m_width = m_Texture->GetWidth();
  m_height = m_Texture->GetHeight();
  m_Client = a_Client;
  m_unitType = UnitType::ENEMY;
  accumulatedX = accumulatedY = 0;
  endTime = clockTime = 0;
  m_Texture1 = new Tmpl8::Surface("assets/enemy1.png");
  m_Texture2 = new Tmpl8::Surface("assets/enemy2.png");
}

Enemy::~Enemy()
{
  if (m_Texture)
    delete m_Texture;
  if (m_Texture1)
    delete m_Texture1;
  if (m_Texture2)
    delete m_Texture2;
}

void Enemy::Update(float a_dt)
{ 
  if (m_Health == 1)
    m_Texture = m_Texture2;
  else if (m_Health == 2)
    m_Texture = m_Texture1;

  if (currTime < updateTime)//unit not owned by player, calculate position velocity using entity interpolation
  {
    m_drawX += m_xVel * a_dt;
    m_drawY += m_yVel * a_dt;
    currTime += a_dt;
  }
  else
  {
    m_drawX = m_x;
    m_drawY = m_y;
  }

}