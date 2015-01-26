#include "JobDelegator.h"

int JobDelegator::g_numThreads = std::thread::hardware_concurrency() / 2;
//int JobDelegator::g_numThreads = 1; // 1 for main, 1 for input
JobDelegator* JobDelegator::singleton;

JobDelegator::JobDelegator()
{
  InitializeCriticalSection(&queueCritSec);
}


JobDelegator::~JobDelegator()
{
  Terminate();
  delete[] threads;
  DeleteCriticalSection(&queueCritSec);
}


void JobDelegator::Queue(Job* a_job)
{
  // No critical section needed here, because the main thread is the only 
  // thread accessing the queue
  
  //EnterCriticalSection(&queueCritSec); 
  _jobs.push(a_job);
  //LeaveCriticalSection(&queueCritSec);
}

Job* JobDelegator::Fetch()
{
  // No critical section needed here, because the main thread is the only 
  // thread accessing the queue

  //EnterCriticalSection(&queueCritSec);
  /*if (_jobs.empty())
  {
    LeaveCriticalSection(&queueCritSec);
    return nullptr;
  }*/
  Job* front = _jobs.front();
  _jobs.pop();
  //LeaveCriticalSection(&queueCritSec);
  return front;
}

void JobDelegator::Terminate()
{
  for (int i = 0; i < g_numThreads; ++i)
  {
    threads[i].Terminate();
  }
}

void JobDelegator::Delegate()
{
  while (!_jobs.empty())
  {
    Job* j = Fetch();
    bool gaveJob = false;
    while (!gaveJob)
    {
      int id = NextThreadId();
      if (threads[id].status == Thread::IDLE)
      {
        threads[id].GiveJob(j);
        gaveJob = true;
      }
    }
  }
}

void JobDelegator::Finish()
{
  Delegate();
  for (int i = 0; i < g_numThreads; ++i)
  {
    threads[i].Sync();
  }
}

void JobDelegator::CreateThreads()
{
  threads = new Thread[g_numThreads];

  // Code below was to align threads to 64 bytes. 
  // Did not give any performance improvements

  //threads = (Thread*)_aligned_malloc(sizeof(Thread)* g_numThreads, 64);
  //memset(threads, 0, sizeof(Thread)* g_numThreads);
  //for (int i = 0; i < g_numThreads; ++i)
  //{
  //  threads[i].Start();
  //}
}