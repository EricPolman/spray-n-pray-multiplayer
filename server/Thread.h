#pragma once
#include <thread>
#include <queue>
#include <mutex>
#include <Windows.h>

class Job;

class Thread
{
public:
  enum Status { IDLE, EXECUTING, PAUSED, FINISHED };

  Thread();
  ~Thread();

  Status status;

  void Start();
  void Run();
  void Terminate();
  void Sync();
  void GiveJob(Job* a_job);
private:

  std::thread _thread;
  Job* currentJob;
  bool _stop;
  int _padding[11]; // Force 64 bytes for a thread, for cache alignment
};

