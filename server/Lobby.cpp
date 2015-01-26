#include "Lobby.h"
#include "Client.h"
#include "Room.h"
#include "GameServer.h"

Lobby::Lobby()
{
}


Lobby::~Lobby()
{
}


void Lobby::Join(Client* client)
{
  // TODO: Send message here
  client->SendServerMessage("/jndlby"); // Joined Lobby
  client->state = IN_LOBBY;

  std::string msg = "/ssay ";
  msg.append(client->name);
  msg.append(" has joined the lobby.");
  BroadcastMessage(msg);
  clients.push_back(client);

  BroadcastClientList();
  BroadcastRoomList();
}

void Lobby::Leave(Client* client)
{
  for (auto iter = clients.begin(); iter != clients.end(); ++iter)
  {
    if (*iter == client)
    {
      clients.erase(iter);
      break;
    }
  }
  std::string msg = "/ssay ";
  msg.append(client->name);
  msg.append(" has left the lobby.");

  BroadcastMessage(msg);
  BroadcastClientList();
}

void Lobby::SetName(Client* client, std::string name)
{
  // TODO: Send message here

}

void Lobby::BroadcastRoomList()
{
  std::string msg = "/roomlist ";

  for (auto room : GameServer::instance->GetRooms())
  {
    if (room)
    {
      msg.append(room->name);
      msg.append(" (" + std::to_string(room->clients.size()) + ")");
      msg += (char)0xFF;
    }
  }

  for (auto cli : clients)
  {
    cli->SendServerMessage(msg);
  }
}

void Lobby::BroadcastClientList()
{
  std::string msg = "/clilist ";

  for (auto cli : clients)
  {
    msg.append(cli->name);
    msg += (char)0xFF;
  }

  for (auto cli : clients)
  {
    cli->SendServerMessage(msg);
  }
}

void Lobby::BroadcastMessage(std::string msg)
{
  for (auto cli : clients)
  {
    cli->SendServerMessage(msg);
  }
}

void Lobby::BroadcastChat(char* message, unsigned int size, Client* client)
{
  char broadcastMsg[256];
  sprintf_s(broadcastMsg, "%16s: %s\n", client->name.c_str(), message);
  printf(broadcastMsg, "%16s: %s\n", client->name.c_str(), message);
  for (auto i : clients)
  {
    i->SendServerMessage(broadcastMsg);
  }
}
