#pragma once
#include <string>
#include <winsock2.h>
#include <list>
#include <vector>
#include <Map>
#include "UDPCommands.h"

#define BUFFER_SIZE 4096

enum ClientState
{
  NOT_CONNECTED,
  CONNECTING,
  CONNECTED,
  SETTING_UP,
  IN_LOBBY,
  IN_ROOM,
  DISCONNECTED,
  SETTING_NAME,
  SETTING_IP
};

class GameObject;
class Messenger;
class UDPMessageHandler;

namespace Tmpl8
{
  class Surface;
}

class Client
{
public:
  Client();
  ~Client();

  void Update(Tmpl8::Surface* a_Surface, float a_DT);
  bool Connect();
  void Disconnect();
  void SetIP(Tmpl8::Surface* a_Surface);
  void SetName(Tmpl8::Surface* a_Surface);
  void ReceiveMessage();
  void OnGUI(Tmpl8::Surface* a_Surface);

  void AddGameobject(GameObject* a_gameobject, unsigned short a_Id);
  std::map<unsigned short, GameObject*> & GetGameObjectList(){ return m_GameObjects; }
  ClientState GetState(){ return m_State; }
  unsigned short GetSequence(){ return m_sequence; }
  unsigned char GetID(){ return m_ID; }
  void IntergrateSequence(){ m_sequence++; }

  Messenger* m_Messenger;
  UDPMessageHandler* m_UDPMessageHandler;
  SOCKET UDPSocket;
  sockaddr_in udpAddr;
  SOCKET TCPSocket;
  sockaddr_in tcpAddress;
  GameFlowState m_gameFlowState;

  unsigned int udpmessagesSent, tcpmessagesSent;
  unsigned int udpbytesSent, tcpBytesSent;
  unsigned int udpmessagesReceived, tcpmessagesReceived;
  unsigned int udpbytesReceived, tcpbytesReceived;

private:
	std::string m_IP;
  std::string m_StateMessage;
  std::map<unsigned short, GameObject*> m_GameObjects;
  ClientState m_State;
  unsigned char m_ID;
  bool m_showChat;
  std::string m_Name;
  std::string m_roomName;

  unsigned short m_sequence;
  char m_buffer[BUFFER_SIZE];
};

