#pragma once

class Room;
class ClientManager;
struct fd_set;

namespace JobState
{
  enum JobState
  {
    PENDING,
    WAITING_FOR_DEPENDENCY,
    EXECUTING,
    FINISHED
  };
}

class Job
{
public:
  
  Job() {}
  ~Job(){}

  virtual void Execute() = 0;
};

class UpdateClientManagerJob : public Job
{
public:
  UpdateClientManagerJob(ClientManager* a_clm, fd_set* a_fds) : clm(a_clm), fds(a_fds){}
  virtual void Execute();

  ClientManager* clm;
  fd_set* fds;
};


class UpdateInputManagerJob : public Job
{
public:
  UpdateInputManagerJob() {}
  virtual void Execute();
};

class UpdateRoomGameFlowJob : public Job
{
public:
  UpdateRoomGameFlowJob(Room* a_room) : room(a_room) {}
  virtual void Execute();

  Room* room;
  float deltaTime;
};

class UpdateRoomJob : public Job
{
public:
  UpdateRoomJob(Room* a_room) : room(a_room) {}
  virtual void Execute();

  Room* room;
  float deltaTime;
};

class ParseRoomUdpMessages : public Job
{
public:
  ParseRoomUdpMessages(Room* a_room) : room(a_room) {}
  virtual void Execute();

  Room* room;
};

class SendRoomUdpMessages : public Job
{
public:
  SendRoomUdpMessages(Room* a_room) : room(a_room) {}
  virtual void Execute();

  Room* room;
};
