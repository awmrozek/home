/*!**************************************************************************
*! FILE NAME  : Job.cc
*!                                                            
*! DESCRIPTION: The implementation of the Job module
*!                          
*!************************************************************************* */

#include "job.ihh"
#include "iostream.hh"

//#define D_JOB
#ifdef D_JOB
#define trace cout
#else
#define trace if(false) cout
#endif

//*#**************************************************************************
//*# 

Job::Job(Job::Priority thePriority)
        : myPriority(thePriority)
{
}

//*#**************************************************************************
//*# 

void
Job::execute()
{
  Job* aJob = JobQueue::instance()->first();
  aJob->doit();
  delete aJob;
}

//*#**************************************************************************
//*# 

void
Job::createServers(unsigned int theNumberOfServers,
                   int          theStackSize,
                   unsigned int theSoftLimit)
{
  JobQueue::initialise(theSoftLimit);
  for (unsigned i = 0; i < theNumberOfServers; i++)
  {
    (new JobThread(theStackSize, i))->start();
  }
}

//-----------------------------------------------------------------------------
// 

void
Job::schedule(Job* theJob)
{
  JobQueue::instance()->add(theJob);
}


//-----------------------------------------------------------------------------
// 

Job::Priority
Job::priority() const
{
  return myPriority;
}

//*#**************************************************************************
//*# 

Job::~Job()
{}

//-----------------------------------------------------------------------------
// 

JobQueue*
JobQueue::myInstance = 0;

//*#**************************************************************************
//*# 

JobQueue*
JobQueue::instance()
{
  if (!myInstance)
  {
    myInstance = new JobQueue;
  }
  return myInstance;
}

//-----------------------------------------------------------------------------
// 

void
JobQueue::initialise(unsigned int theSoftLimit)
{
  JobQueue::instance()->softLimit(theSoftLimit);
}

//-----------------------------------------------------------------------------
// 

void
JobQueue::softLimit(unsigned int theSoftLimit)
{
  mySoftLimit = theSoftLimit;
}

//-----------------------------------------------------------------------------
// 

bool
JobQueue::softLimitExceeded() const
{
  return static_cast<unsigned int>(myQueue.Length()) > mySoftLimit;
}

//*#**************************************************************************
//*# 

void
JobQueue::add(Job* theJob)
{
  myQueue.Append(theJob);
  myJobsToExecute->signal();
}

//*#**************************************************************************
//*# 

Job*
JobQueue::first()
{
  myJobsToExecute->wait();
  Job* aJob = myQueue.Front();
  myQueue.Pop();
  return aJob;
}

//*#**************************************************************************
//*# 

JobQueue::JobQueue()
        : myQueue(),
          myJobsToExecute(Semaphore::createQueueSemaphore("Job", 0)),
          mySoftLimit(JobQueue::defaultSoftLimit)
{}

//*#**************************************************************************
//*# 

JobQueue::~JobQueue()
{
  delete myJobsToExecute;
}

//*#**************************************************************************
//*# 

JobThread::JobThread(int theStackSize, int theNumber)
    : Thread("Job", theStackSize),
      myNumber(theNumber)
{}

//*#**************************************************************************
//*# 

void
JobThread::proc()
{
  while(true)
  {
    Job::execute();
    trace << "JobThread #" << myNumber << " finished Job, stack usage: "
          << this->stackUsage() << endl;
  }
}




