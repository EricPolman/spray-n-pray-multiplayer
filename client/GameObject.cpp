#include "GameObject.h"
#include "InputManager.h"
#include "Client.h"
#include "MainScene.h"
#include "surface.h"

void GameObject::Move(float a_x, float a_y)
{
  m_prevX = m_x, m_prevY = m_y;
  m_x += a_x;
  m_y += a_y;
  m_drawX = m_x;
  m_drawY = m_y;
}

void GameObject::SetPosition(float a_x, float a_y)
{
  endTime = clockTime;
  clockTime = clock();
  updateTime = (float)(clockTime - endTime) / CLOCKS_PER_SEC;
  currTime = 0;

  m_prevX = m_x, m_prevY = m_y;
  m_x = a_x;
  m_y = a_y;

  m_drawX = (m_Owner == m_Client->GetID()) ? m_x : m_prevX;
  m_drawY = (m_Owner == m_Client->GetID()) ? m_y : m_prevY;
  
  m_xVel = (m_x - m_prevX) / updateTime;
  m_yVel = (m_y - m_prevY) / updateTime;
}

void GameObject::Render(Tmpl8::Surface* a_Surface)
{
  m_Texture->CopyTo(a_Surface, (int)m_drawX - m_Texture->GetWidth() / 2, (int)m_drawY - m_Texture->GetHeight() / 2);
}

void GameObject::CheckCollision(std::map<unsigned short, GameObject*> a_Gameobjects)
{
  int length = a_Gameobjects.size();
  int curr = 0;
  for (auto it = a_Gameobjects.begin(); it != a_Gameobjects.end(); it++)
  {
    if (curr >= length / 2)
    {
      //collision check here
      if (it->second->m_Owner != m_Owner)
      {
        GameObject* go = it->second;
        if (m_x < go->m_x + go->m_width && m_x + m_width > go->m_x &&
          m_y < go->m_y + go->m_height && m_y + m_height > go->m_y)
        {
          //collisiding
          //printf("object %hu colliding with object %hu\n", m_ID, go->m_ID);
          if (m_unitType == UnitType::PLAYER && go->m_unitType == UnitType::BULLET)
          {
          }
          else if (m_unitType == UnitType::BULLET && go->m_unitType == UnitType::PLAYER)
          {
          }
        }
      }
    }
    curr++;
  }
}