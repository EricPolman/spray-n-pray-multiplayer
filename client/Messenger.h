#pragma once
#include <string>
#include <list>
#include <vector>

namespace Tmpl8
{
  class Surface;
}
class Client;
class Messenger
{
public:
  Messenger();
  ~Messenger();

  void Message(Client* a_cli);
  void DrawChat(Tmpl8::Surface*, Client* a_cli);
  void PushMessage(std::string a_Message){ m_MessageLog.push_front(a_Message); }
  std::vector<std::string> m_NameList;
  std::vector<std::string> m_RoomNameList;
  bool IsMessaging(){ return m_Messaging; }

  std::string m_Message;

private:
  bool m_Messaging;
  std::list<std::string> m_MessageLog;
  char buffer[4096];
};

