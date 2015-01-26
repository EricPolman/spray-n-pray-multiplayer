#include "Job.h"
#include "ClientManager.h"
#include <WinSock2.h>
#include "InputManager.h"
#include "Room.h"

void UpdateClientManagerJob::Execute()
{
  clm->Update(0, fds);
}

void UpdateInputManagerJob::Execute()
{
  Input->Update(0);
}

void UpdateRoomGameFlowJob::Execute()
{
  room->_gameFlow.Update(deltaTime);
}

void UpdateRoomJob::Execute()
{
  room->Update(deltaTime);
}

void ParseRoomUdpMessages::Execute()
{
  if (room->_msgManager.processBufferPosition > 0)
  {
    room->HandleUDPMessage(room->_msgManager._processBuffer, room->_msgManager.processBufferPosition);
    room->_msgManager.ClearProcessBuffer();
  }
}

void SendRoomUdpMessages::Execute()
{
  room->BroadcastWaitingMessages();
}
