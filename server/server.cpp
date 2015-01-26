// ha2-test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GameServer.h"
#include "UDPCommands.h"

int _tmain(int argc, _TCHAR* argv[])
{
  GameServer server;

  server.Start(8);
  server.Run(); // Fix deltatime
  server.Shutdown();

  return 0;
}
