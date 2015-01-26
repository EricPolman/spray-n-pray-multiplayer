#include "Player.h"
#include "InputManager.h"
#include "MainScene.h"
#include "Client.h"
#include "surface.h"
#include "Messenger.h"

Player::Player(float a_Xpos, float a_Ypos, float a_Speed, char* a_file, Client* a_Client)
{
  m_x = m_prevX = m_drawX = a_Xpos;
  m_y = m_prevX = m_drawY = a_Ypos;
  m_xVel = m_yVel = 0;
  m_speed = a_Speed;
  m_Texture = new Tmpl8::Surface(a_file);
  m_width = m_Texture->GetWidth();
  m_height = m_Texture->GetHeight();
  m_Client = a_Client;
  m_unitType = UnitType::PLAYER;
  accumulatedX = accumulatedY = 0;
  m_Health = 10;
  endTime = clockTime = 0;
  m_healthFull = new Tmpl8::Surface("assets/healthFull.png");
  m_healthEmpty = new Tmpl8::Surface("assets/healthEmpty.png");
}

Player::~Player()
{
  delete m_Texture;
  delete m_healthEmpty;
  delete m_healthFull;
}

void Player::Update(float a_dt)
{
  if (m_Owner == m_Client->GetID() && !m_Client->m_Messenger->IsMessaging()) //only update if it's the clients player player
  {
    m_xVel = m_yVel = 0;
    //m_drawX = m_x, m_drawY = m_y;
    // Player movement 
    if (Input->IsKeyDown(SDLK_w))
      m_yVel -= 1;
    if (Input->IsKeyDown(SDLK_s))
      m_yVel += 1;
    if (Input->IsKeyDown(SDLK_a))
      m_xVel -= 1;
    if (Input->IsKeyDown(SDLK_d))
      m_xVel += 1;

    glm::vec2 vel(m_xVel, m_yVel);
    if (vel.x != 0 || vel.y != 0)
    {
      vel = glm::normalize(vel) * m_speed;
      m_xVel = vel.x;
      m_yVel = vel.y;

      m_drawX += m_xVel * a_dt;
      m_drawY += m_yVel * a_dt;
    }
    //point p = m_Mainscene->GetInputTime();
    //m_drawX += m_speed * p.x;
    //m_drawY += m_speed * p.y;
  }
  else if (currTime < updateTime)//unit not owned by player, calculate position velocity using entity interpolation
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

void Player::Render(Tmpl8::Surface* a_Surface)
{
  m_Texture->CopyTo(a_Surface, m_drawX - m_Texture->GetWidth() / 2, m_drawY - m_Texture->GetHeight() / 2);

  m_healthEmpty->CopyTo(a_Surface, m_drawX - (m_Texture->GetWidth() / 2) - 5, m_drawY - (m_Texture->GetHeight() / 2) - 10);
  for (int i = 0; i < m_Health; i++)
  {
    m_healthFull->CopyTo(a_Surface, m_drawX - (m_Texture->GetWidth() / 2) - 5 + (i * 5), m_drawY - (m_Texture->GetHeight() / 2) - 10);
  }
}