#include "Client.h"
#include "InputManager.h"
#include "surface.h"
#include <iostream>
#include <fstream>
#include <WS2tcpip.h>
#include "Bullet.h"
#include "MainScene.h"
#include "Player.h"
#include "Enemy.h"
#include "Messenger.h"
#include "UDPMessageHandler.h"

#define TCP_PORT 3742
#define UDP_PORT 2473
int msgSize;

//Set up basic data
Client::Client() : m_sequence(0), m_showChat(true), m_gameFlowState(GAME_STARTING)
{
  udpmessagesSent = tcpmessagesSent = udpbytesSent = tcpBytesSent = udpmessagesReceived = tcpmessagesReceived = udpbytesReceived = tcpbytesReceived = 0;

  m_Messenger = new Messenger();
  m_UDPMessageHandler = new UDPMessageHandler();
  std::cout << "Client started.\n";
  
  //start of by setting the ip, after that we connect
  m_State = SETTING_IP;

  std::ifstream nameFile("assets/name.txt");
  if (nameFile.is_open())
  {
    std::string name;
    std::getline(nameFile, name);
    nameFile.close();
    m_Name = name;
  }
  else
    m_Name = "unnamed";
}

Client::~Client()
{
  Disconnect();
  delete(m_Messenger);
  delete(m_UDPMessageHandler);
}

bool Client::Connect()
{
  if (connect(TCPSocket, (sockaddr*)&tcpAddress, sizeof(tcpAddress)) != 0)
  {
    m_Messenger->PushMessage("Connection failed, please re-enter IP");
    m_State = SETTING_IP;
    return false;
  }
  std::ofstream ipFile("assets/ip.txt");
  if (ipFile.is_open())
  {
    ipFile << m_IP;
    ipFile.close();
  }

  std::cout << "Connected\n";
  m_Messenger->PushMessage("Connected to: " + m_IP);
  m_State = SETTING_UP;
  return true;
}

void Client::ReceiveMessage()
{
  fd_set readfds;
  timeval t;
  t.tv_sec = 0;
  t.tv_usec = 1;

  //get UDP messages
  FD_ZERO(&readfds);
  FD_SET(UDPSocket, &readfds);
  FD_SET(TCPSocket, &readfds);
  select(2, &readfds, 0, 0, (const timeval*)&t);

  //get UDP messages
  if (FD_ISSET(UDPSocket, &readfds))
  {
    static char b[BUFFER_SIZE];
    int size = sizeof(udpAddr);
    msgSize = recvfrom(UDPSocket, b, BUFFER_SIZE, 0, (sockaddr*)&udpAddr, &size);

    if (msgSize > 0)
    {
      udpbytesReceived += msgSize;
      udpmessagesReceived++;
    }

    m_UDPMessageHandler->ParseUDPMessage(b, msgSize, this);
  }

  //get TCP messages
  if (FD_ISSET(TCPSocket, &readfds))
  {
    static char b[BUFFER_SIZE];
    char* p = b;
    msgSize = recv(TCPSocket, b, BUFFER_SIZE, 0);
    if (msgSize > 0)
    {
      tcpmessagesReceived++;
      tcpbytesReceived += msgSize;
    }
    if (msgSize > 0)
    {
      while (p < b + msgSize)
      {
        if (strncmp(p, "/setid ", 7) == 0)
        {
          m_ID = *(unsigned char*)&p[7];
          std::cout << "ID set to " << m_ID << "\n";

          sprintf_s(m_buffer, "/name %s", m_Name.c_str());
          send(TCPSocket, m_buffer, strlen(m_buffer) + 1, 0);

          //set udp socket after you got id
          UDPSocket = socket(AF_INET, SOCK_DGRAM, 0);
          udpAddr.sin_family = AF_INET;
          inet_pton(AF_INET, m_IP.c_str(), &(udpAddr.sin_addr.s_addr));
          udpAddr.sin_port = htons(UDP_PORT);

          BlankPacket packet(m_ID, m_sequence);
          char* b = (char*)&packet;
          sendto(UDPSocket, b, sizeof(packet), 0, (sockaddr*)&udpAddr, sizeof(udpAddr));
          udpmessagesSent++;
          udpbytesSent += sizeof(packet);
          m_sequence++;
        }
        else if (strncmp(p, "/jndlby", 7) == 0)
        {
          std::cout << "Joined lobby\n";
          m_Messenger->PushMessage("Joined lobby\n");
          m_State = IN_LOBBY;
        }
        else if (strncmp(p, "/joinedroom ", 12) == 0)
        {
          char* roomName = &p[12];
          sprintf_s(m_buffer, "Joined room \'%s\'\n", roomName);
          m_roomName = roomName;
          std::cout << m_buffer;
          m_GameObjects.clear();
          m_State = IN_ROOM;
          m_Messenger->PushMessage(m_buffer);
        }
        else if (strncmp(p, "/ssay ", 6) == 0)
        {
          char* msg = &p[6];
          sprintf_s(m_buffer, "  server message: %s\n", msg);
          m_Messenger->PushMessage(m_buffer);
        }
        else if (strncmp(p, "/clilist ", 9) == 0)
        {
          m_Messenger->m_NameList.clear();
          memcpy(m_buffer, &p[9], strlen(p) - 9);
          unsigned int listSize = strlen(p) - 9;
          char* list = m_buffer;

          while (list != m_buffer + listSize)
          {
            unsigned int len = 0;
            while (*(list + len) != (char)0xFF)
              ++len;

            list[len] = '\0';
            m_Messenger->m_NameList.push_back(list);
            list += len + 1;
          }
        }
        else if (strncmp(p, "/roomlist ", 10) == 0)
        {
          m_Messenger->m_RoomNameList.clear();
          memcpy(m_buffer, &p[10], strlen(p) - 10);
          unsigned int listSize = strlen(p) - 10;
          char* list = m_buffer;

          while (list != m_buffer + listSize)
          {
            unsigned int len = 0;
            while (*(list + len) != (char)0xFF)
              ++len;

            list[len] = '\0';
            m_Messenger->m_RoomNameList.push_back(list);
            list += len + 1;
          }
        }
        else
        {
          std::cout << p;
          m_Messenger->PushMessage(p);
        }
        p += strlen(p) + 1;
      }
    }
    else if (m_State == IN_ROOM || m_State == IN_LOBBY) //disconnected
    {
      m_Messenger->PushMessage("Lost connection to server...returning to menu");
      Disconnect();
    }
  }
}

void Client::Disconnect()
{
  m_GameObjects.clear();
  std::cout << "Disconnected\n";
  m_Messenger->PushMessage("Disconnected...");
  m_State = DISCONNECTED;

  closesocket(UDPSocket);
  closesocket(TCPSocket);
  WSACleanup();
}

void Client::SetIP(Tmpl8::Surface* a_Surface)
{
  char c = Input->GetKeyPress();
  if (c != 0 && m_Messenger->m_Message.size() < 64)
  {
    if (c == '\b' && !m_Messenger->m_Message.empty())
      m_Messenger->m_Message.pop_back();
    else
      m_Messenger->m_Message += c;
  }

  if (Input->IsKeyPressed(SDLK_RETURN))
  {
    m_IP = m_Messenger->m_Message;
    m_Messenger->m_Message.clear();
    
    if (m_IP.size() == 0)
    {
      std::ifstream ipFile("assets/ip.txt");
      if (ipFile.is_open())
      {
        std::string ipline;
        std::getline(ipFile, ipline);
        ipFile.close();
        m_IP = ipline;
      }
    }

    //m_IP = "86.80.131.94";//polman home
    m_Messenger->PushMessage("IP set to: " + m_IP);
    m_State = CONNECTING;
    m_Messenger->PushMessage("Connecting..");
  }
}

void Client::SetName(Tmpl8::Surface* a_Surface)
{
  /*std::string* message = m_Messenger->GetCurrentMessage();
  a_Surface->Print("Client name: ", 20, SCRHEIGHT - 20, 0xffff00);
  char c = Input->GetKeyPress();
  if (c != '0' && message->size() < 64)
  {
    if (c == '\b' && !message->empty())
      message->pop_back();
    else
      message += c;
  }
  a_Surface->Print((char*)message->c_str(), 100, 580, 0xffff00);

  if (Input->IsKeyPressed(SDLK_RETURN))
  {
    message = ("Eric Troost");
  }*/
}

void Client::Update(Tmpl8::Surface* a_Surface, float a_DT)
{
  ReceiveMessage();
  switch (m_State)
  {
  case CONNECTING:
    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    //set tcp socket
    m_buffer[0] = '\0';
    msgSize = 0;

    TCPSocket = socket(AF_INET, SOCK_STREAM, 0);
    tcpAddress.sin_family = AF_INET;
    inet_pton(AF_INET, m_IP.c_str(), &tcpAddress.sin_addr.s_addr);
    tcpAddress.sin_port = htons(TCP_PORT);

    m_StateMessage = "Connecting...";
    if (Connect())
    {
    }
    break;

  case CONNECTED:
  {
    break;
  }
  case IN_ROOM:
  {
    m_StateMessage = "In room: " + m_roomName;
    if (Input->IsKeyPressed(SDLK_t) && !m_Messenger->IsMessaging())
    m_showChat = !m_showChat;
    m_Messenger->Message(this);
    break;
  }
  case IN_LOBBY:
    //static bool joinedRoom = false;
    //if (!joinedRoom)
    //{
    //  joinedRoom = true;
    //  // Join room
    //  send(TCPSocket, "joinroom 3", 11, 0); // any char will do
    //}
    m_StateMessage = "In Lobby..";
    if (Input->IsKeyPressed(SDLK_t) && !m_Messenger->IsMessaging())
      m_showChat = !m_showChat;

    m_Messenger->Message(this);
    break;
  case DISCONNECTED:
    m_State = SETTING_IP;
    break;
  case SETTING_UP:

    break;

  case SETTING_IP:
    m_StateMessage = "Setting Ip...";
    SetIP(a_Surface);
    break;

  case SETTING_NAME:
    m_StateMessage = "Setting name...";
    SetName(a_Surface);
    break;
  }

}

void Client::OnGUI(Tmpl8::Surface* a_Surface)
{
  if (m_showChat)
    m_Messenger->DrawChat(a_Surface, this);
}

void Client::AddGameobject(GameObject* a_gameobject, unsigned short a_Id)
{
  a_gameobject->SetID(a_Id);
  m_GameObjects[a_Id] = a_gameobject;
}

