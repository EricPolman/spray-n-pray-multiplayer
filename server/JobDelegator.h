#pragma once
// Borrowed from HA3 final assignment 

#include <queue>
#include "Thread.h"
#include "Job.h"
#include <Windows.h>

class JobDelegator
{
public:
  JobDelegator();
  ~JobDelegator();

  void CreateThreads();
  void Finish();
  void Terminate();
  void Queue(Job* a_job);
  Job* Fetch();
  void Delegate();
  unsigned int Count() { return _jobs.size(); }
  static int g_numThreads;
  static JobDelegator* singleton;
private:
  Thread* threads;
  std::queue<Job*> _jobs;
  CRITICAL_SECTION queueCritSec;

  int NextThreadId()
  {
    static int currentThread = 0;
    return ++currentThread > (g_numThreads - 1) ? currentThread = 0 : currentThread;
  }
};

#define JobSys JobDelegator::singleton
