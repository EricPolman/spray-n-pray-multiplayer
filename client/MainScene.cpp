#include "MainScene.h"
#include "Client.h"
#include "surface.h"
#include "Bullet.h"
#include "Player.h"
#include "Enemy.h"
#include "InputManager.h"
#include "Messenger.h"
#include "UDPMessageHandler.h"

Tmpl8::Surface MainScene::backdrop("assets/gamebg.png");
Tmpl8::Surface MainScene::gameover("assets/gameoverbg.png");
Tmpl8::Surface MainScene::Starting("assets/countdown_go.png");
Tmpl8::Surface MainScene::s1("assets/countdown_1.png");
Tmpl8::Surface MainScene::s2("assets/countdown_2.png");
Tmpl8::Surface MainScene::s3("assets/countdown_3.png");

MainScene::MainScene(Client* a_client)
: m_startTimer(0), m_respawnTimer(0)
{
  m_Client = a_client;
}

void MainScene::Update(float a_DT)
{
  switch (m_Client->m_gameFlowState)
  {
  case GAME_STARTING:
    m_startTimer += a_DT;
    break;
  case GAME_RUNNING:
  {
    m_respawnTimer -= a_DT;
    HandleInput();
    m_startTimer = 0;
    auto& gameObjects = m_Client->GetGameObjectList();

    static std::map<GameObject*, glm::vec2> accumulatedDisplacements;
    unsigned int vecSize = gameObjects.size();
    if (vecSize > 1)
    {
      for (auto g1 : gameObjects)
      {
        if (g1.second->m_unitType != ENEMY)
          continue;

        const float unit1x = g1.second->m_x;
        const float unit1y = g1.second->m_y;
        const float unit1radius = 25;

        for (auto g2 : gameObjects)
        {
          if (g1.second == g2.second)
            continue;
          if (g2.second->m_unitType != ENEMY)
            continue;

          const float unit2x = g2.second->m_x;
          const float unit2y = g2.second->m_y;
          const float unit2radius = 25;

          const float radiusSq = (unit1radius * unit1radius) + (unit2radius * unit2radius);
          const float xdiff = unit2x - unit1x, ydiff = unit2y - unit1y;
          const float distSq = (xdiff * xdiff) + (ydiff * ydiff);

          if (distSq < radiusSq) //circle collision check
          {
            glm::vec2 diff = glm::vec2(unit1x, unit1y) - glm::vec2(unit2x, unit2y);
            diff *= a_DT;
            accumulatedDisplacements[g1.second] += diff;
            accumulatedDisplacements[g2.second] -= diff;
          }
        }
      }
    }
    for (auto obj : accumulatedDisplacements)
    {
      //obj.first->m_drawX -= obj.second.x;
      //obj.first->m_drawY -= obj.second.y;
      obj.first->m_x += obj.second.x;
      obj.first->m_y += obj.second.y;
      obj.first->m_prevX = obj.first->m_x;
      obj.first->m_prevY = obj.first->m_y;
    }
    accumulatedDisplacements.clear();

    myPlayer = nullptr;
    for (auto it = m_Client->GetGameObjectList().begin(); it != m_Client->GetGameObjectList().end(); it++)
    {
      if (!it->second)
        __debugbreak();

      if (it->second->m_unitType == PLAYER && it->second->m_Owner == m_Client->GetID())
        myPlayer = it->second;

      it->second->m_Mainscene = this;
      (it->second)->Update(a_DT);
    }
  }
    break;
  case GAME_OVER:
    break;
  }
}

void MainScene::HandleInput()
{
  if (!m_Client->m_Messenger->IsMessaging())
  {
    if (Input->IsKeyPressed(SDLK_SPACE) && m_respawnTimer <= 0.0f && !myPlayer)
    {
      BulletData bData;
      bData.xpos = 100;
      bData.ypos = 100;
      bData.xdir = 0;
      bData.ydir = 0;
      bData.owner = m_Client->GetID();
      NetworkInstantiatePacket iPacket(0, true, bData);
      iPacket.unitType = UnitType::PLAYER;
      m_Client->m_UDPMessageHandler->SendUDPMessage((char*)&iPacket, m_Client);
      m_respawnTimer = 1.0f;
    }

    if (Input->IsKeyPressed(SDLK_w))
    {
      InputPacket inPacket(0, 0, SDLK_w, 0);
      m_sequenceMap[m_Client->GetSequence()] = KeyPressInfo(SDLK_w, 0);
      m_Client->m_UDPMessageHandler->SendUDPMessage((char*)&inPacket, m_Client);
    }
    if (Input->IsKeyPressed(SDLK_s))
    {
      InputPacket inPacket(0, 0, SDLK_s, 0);
      m_sequenceMap[m_Client->GetSequence()] = KeyPressInfo(SDLK_s, 0);
      m_Client->m_UDPMessageHandler->SendUDPMessage((char*)&inPacket, m_Client);
    }
    if (Input->IsKeyPressed(SDLK_a))
    {
      InputPacket inPacket(0, 0, SDLK_a, 0);
      m_sequenceMap[m_Client->GetSequence()] = KeyPressInfo(SDLK_a, 0);
      m_Client->m_UDPMessageHandler->SendUDPMessage((char*)&inPacket, m_Client);
    }
    if (Input->IsKeyPressed(SDLK_d))
    {
      InputPacket inPacket(0, 0, SDLK_d, 0);
      m_sequenceMap[m_Client->GetSequence()] = KeyPressInfo(SDLK_d, 0);
      m_Client->m_UDPMessageHandler->SendUDPMessage((char*)&inPacket, m_Client);
    }

    MouseInputPacket miPacket(0, 0, (unsigned short)Input->GetMousePosition().x, (unsigned short)Input->GetMousePosition().y, 2);
    m_Client->m_UDPMessageHandler->SendUDPMessage((char*)&miPacket, m_Client);

    if (Input->IsMouseButtonPressed(SDL_BUTTON_LEFT))
    {
      MouseInputPacket miPacket(0, 0, (unsigned short)Input->GetMousePosition().x, (unsigned short)Input->GetMousePosition().y, 0);
      m_Client->m_UDPMessageHandler->SendUDPMessage((char*)&miPacket, m_Client);
    }

    //key up
    if (Input->IsKeyUp(SDLK_w))
    {
      InputPacket inPacket(0, 0, SDLK_w, 1);
      m_sequenceMap[m_Client->GetSequence()] = KeyPressInfo(SDLK_w, 1);
      m_Client->m_UDPMessageHandler->SendUDPMessage((char*)&inPacket, m_Client);
    }
    if (Input->IsKeyUp(SDLK_s))
    {
      InputPacket inPacket(0, 0, SDLK_s, 1);
      m_sequenceMap[m_Client->GetSequence()] = KeyPressInfo(SDLK_s, 1);
      m_Client->m_UDPMessageHandler->SendUDPMessage((char*)&inPacket, m_Client);
    }
    if (Input->IsKeyUp(SDLK_a))
    {
      InputPacket inPacket(0, 0, SDLK_a, 1);
      m_sequenceMap[m_Client->GetSequence()] = KeyPressInfo(SDLK_a, 1);
      m_Client->m_UDPMessageHandler->SendUDPMessage((char*)&inPacket, m_Client);
    }
    if (Input->IsKeyUp(SDLK_d))
    {
      InputPacket inPacket(0, 0, SDLK_d, 1);
      m_sequenceMap[m_Client->GetSequence()] = KeyPressInfo(SDLK_d, 1);
      m_Client->m_UDPMessageHandler->SendUDPMessage((char*)&inPacket, m_Client);
    }

    if (Input->IsMouseButtonUp(SDL_BUTTON_LEFT))
    {
      MouseInputPacket miPacket(0, 0, (unsigned short)Input->GetMousePosition().x, (unsigned short)Input->GetMousePosition().y, 1);
      m_Client->m_UDPMessageHandler->SendUDPMessage((char*)&miPacket, m_Client);
    }
  }
}

void MainScene::Render(Tmpl8::Surface* a_Screen)
{
  if (m_Client->GetState() == IN_ROOM)
  {
    switch (m_Client->m_gameFlowState)
    {
    case GAME_STARTING:
      backdrop.CopyTo(a_Screen, 0, 0);
      if (m_startTimer < 1)
      {
        s3.CopyTo(a_Screen, 400 - s3.GetWidth() / 2, 300 - s3.GetHeight() / 2);
      }
      else if (m_startTimer < 2)
      {
        s2.CopyTo(a_Screen, 400 - s2.GetWidth() / 2, 300 - s2.GetHeight() / 2);
      }
      else if (m_startTimer < 3)
      {
        s1.CopyTo(a_Screen, 400 - s1.GetWidth() / 2, 300 - s1.GetHeight() / 2);
      }
      else
      {
        Starting.CopyTo(a_Screen, 400 - s1.GetWidth() / 2, 300 - s1.GetHeight() / 2);
      }
      break;
    case GAME_RUNNING:
    {
      backdrop.CopyTo(a_Screen, 0, 0);
      for (auto it = m_Client->GetGameObjectList().begin(); it != m_Client->GetGameObjectList().end(); it++)
        it->second->Render(a_Screen);
    }
      break;
    case GAME_OVER:
      gameover.CopyTo(a_Screen, 0, 0);
      break;
    }
  }
}
