#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

enum ClientState
{
  NOT_CONNECTED,
  CONNECTED_TO_SERVER,
  IN_LOBBY,
  IN_ROOM
};

class Client
{
public:
  Client() :lastKnownSequenceId(0), roomId(0), isUdpSet(false), score(0),
    syncTimer(0), sequenceCount(0), state(NOT_CONNECTED) {}
  ~Client();

  void Disconnect();

  // TCP message
  void SendServerMessage(std::string);

  sockaddr_in tcpAddress;
  sockaddr_in udpAddress;

  char ipAddress[INET_ADDRSTRLEN];
  char roomId;
  bool stateSyncedInRoom, isUdpSet;
  unsigned int score;
  unsigned char id;
  std::string name;
  unsigned char lastKnownSequenceId; // serverside
  unsigned char sequenceCount;
  float syncTimer;
  SOCKET tcpSocket;
  ClientState state;
};
