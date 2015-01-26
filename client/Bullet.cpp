#include "Bullet.h"
#include "Client.h"
#include "surface.h"


Bullet::Bullet(float a_Xpos, float a_Ypos, float a_xDir, float a_yDir, float a_Speed, char* a_file, Client* a_Client)
{
  m_x = m_drawX = a_Xpos;
  m_y = m_drawY = a_Ypos;
  m_xVel = a_xDir;
  m_yVel = a_yDir;
  m_speed = a_Speed;
  m_Texture = new Tmpl8::Surface(a_file);
  m_width = m_Texture->GetWidth();
  m_height = m_Texture->GetHeight();
  m_Client = a_Client;
  m_unitType = UnitType::BULLET;
  accumulatedX = accumulatedY = 0;
  m_Owner;
}

Bullet::~Bullet()
{
  delete m_Texture;
}

void Bullet::Update(float a_dt)
{
  m_drawX += m_xVel * a_dt;
  m_drawY += m_yVel * a_dt;
}