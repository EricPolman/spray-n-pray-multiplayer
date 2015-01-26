#include "Messenger.h"
#include "InputManager.h"
#include <iostream>
#include <fstream>
#include "Client.h"
#include "game.h"
#include "surface.h"

Messenger::Messenger() : m_Messaging(false)
{
}

Messenger::~Messenger()
{
}

void Messenger::Message(Client* a_cli)
{
  if (m_Messaging)
  {
    char c = Input->GetKeyPress();
    if (c != 0 && m_Message.size() < 64)
    {
      if (c == '\b' && !m_Message.empty())
        m_Message.pop_back();
      else
        m_Message += c;
    }

    if (Input->IsKeyPressed(SDLK_RETURN))
    {
      if (m_Message.size() > 0)
      {
        if (!_strcmpi(m_Message.c_str(), "/quit"))
        {
          m_Message.clear();
          m_MessageLog.clear();
          m_Messaging = false;
          a_cli->Disconnect();
          return;
        }
        if (m_Message[0] != '/')
        {
          m_Message = "/say " + m_Message;
          strcpy_s(buffer, m_Message.c_str());
          send(a_cli->TCPSocket, buffer, strlen(buffer) + 1, 0);
          a_cli->tcpmessagesSent++;
          a_cli->tcpBytesSent += sizeof(buffer);
          m_Message.clear();
        }
        else
        {
          if (strncmp(m_Message.c_str(), "/name ", 6) == 0)
          {
            // make sure name doesn't exceed 16 characters
            if (m_Message.length() > 22)
              m_Message.resize(22);

            m_MessageLog.push_front("Name set to: " + m_Message.substr(6, m_Message.size() - 6));

            strcpy_s(buffer, m_Message.c_str());
            send(a_cli->TCPSocket, buffer, strlen(buffer) + 1, 0);
            a_cli->tcpmessagesSent++;
            a_cli->tcpBytesSent += sizeof(buffer);

            std::ofstream nameFile("assets/name.txt");
            if (nameFile.is_open())
            {
              nameFile << m_Message.substr(6, m_Message.size() - 6);
            }

            m_Message.clear();
          }
          else
          {
            send(a_cli->TCPSocket, m_Message.c_str(), m_Message.size() + 1, 0);
            a_cli->tcpmessagesSent++;
            a_cli->tcpBytesSent += sizeof(m_Message);
          }
        }
      }
      m_Message.clear();
      m_Messaging = false;
    }
  }
  else
  {
    if (Input->IsKeyPressed(SDLK_RETURN) && !m_Messaging)
      m_Messaging = true;
  }
}

void Messenger::DrawChat(Tmpl8::Surface* a_Surface, Client* a_cli)
{
  ClientState m_State = a_cli->GetState();
  //a_Surface->Print((char*)m_StateMessage.c_str() , 20, 20, 0xffff00);
  a_Surface->Line(0, SCRHEIGHT - 26, SCRWIDTH / 3, SCRHEIGHT - 26, 0xffff00);
  if (m_State == SETTING_IP)
  {
    a_Surface->Print("Server IP: ", 20, SCRHEIGHT - 20, 0xffff00);
    a_Surface->Print((char*)m_Message.c_str(), 100, SCRHEIGHT - 20, 0xffff00);
  }
  else if (m_State == IN_LOBBY || m_State == IN_ROOM)
  {
    if (m_State == IN_LOBBY)
    {
      for (unsigned int i = 0; i < m_NameList.size(); ++i)
      {
        a_Surface->Print((char*)m_NameList[i].c_str(), SCRWIDTH - 156, SCRHEIGHT - 270 + 12 * i, 0xFFFFFF);
      }
      for (unsigned int i = 0; i < m_RoomNameList.size(); ++i)
      {
        a_Surface->Print((char*)m_RoomNameList[i].c_str(), SCRWIDTH - 360, SCRHEIGHT - 270 + 12 * i, 0xFFFFFF);
      }
    }
    else
    {
      if (a_cli->m_gameFlowState == GAME_OVER)
      {
        for (unsigned int i = 0; i < m_NameList.size(); ++i)
        {
          a_Surface->Print((char*)m_NameList[i].c_str(), SCRWIDTH / 2 - 60, 285 + 12 * i, 0xFFFF00);
        }
      }
      else
      {
        a_Surface->Print("Players in room", 20, 20, 0xFFFFFF);
        for (unsigned int i = 0; i < m_NameList.size(); ++i)
        {
          a_Surface->Print((char*)m_NameList[i].c_str(), 20, 40 + 12 * i, 0xFFFF00);
        }
      }
    }

    a_Surface->Print("Message: ", 20, SCRHEIGHT - 20, 0xffff00);

    if (m_Messaging)
      a_Surface->Print((char*)m_Message.c_str(), 100, SCRHEIGHT - 20, 0xffff00);
    else
      a_Surface->Print("Press Enter to start typing a message.. ", 100, SCRHEIGHT - 20, 0xffff00);
  }

  int y = SCRHEIGHT - 40;
  for (auto it = m_MessageLog.begin(); it != m_MessageLog.end(); it++)
  {
    if (y > SCRHEIGHT - (SCRHEIGHT / 4))
    {
      if (strncmp("  server message", it->c_str(), 16) == 0)
        a_Surface->Print((char*)it->c_str(), 20, y, 0xFF);
      else
      {
        const char* str = it->c_str();
        if (it->size() > 16 && strncmp(&str[16], ":", 1) == 0) // Is most probably chat 
          a_Surface->Print((char*)it->c_str(), 20, y, 0xffff00);
        else
          a_Surface->Print((char*)it->c_str(), 20, y, 0xffffff);
      }
    }
    else
    {
      m_MessageLog.pop_back();
      break;
    }
    y -= 10;
  }
}