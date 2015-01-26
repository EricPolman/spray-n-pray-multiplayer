#include "Thread.h"
#include "Job.h"
#include <chrono>
#include "JobDelegator.h"

Thread::Thread()
: _stop(false), currentJob(nullptr), status(IDLE)
{
  _thread = std::thread(&Thread::Run, this);
}

Thread::~Thread()
{
  if (&_thread != nullptr)
    _thread.detach();
}

void Thread::Run()
{
  while (!_stop)
  {
    switch (status)
    {
    case Status::IDLE:
      if (_stop)
      {
        status = FINISHED;
        return;
      }
      std::this_thread::yield();
      break;
    case Status::EXECUTING:
      currentJob->Execute();
      currentJob = nullptr;
      status = IDLE;
      break;
    }
  }
}

void Thread::Terminate()
{
  _stop = true;
  status = FINISHED;
  _thread.join();
}

void Thread::Sync()
{
  while (currentJob != nullptr) 
  { 
    std::this_thread::sleep_for(std::chrono::microseconds(1)); 
  }
}
void Thread::Start()
{
  _stop = false;
  status = IDLE;
  currentJob = nullptr;
  _thread = std::thread(&Thread::Run, this);
}

void Thread::GiveJob(Job* a_job)
{
  currentJob = a_job;
  status = EXECUTING;
}