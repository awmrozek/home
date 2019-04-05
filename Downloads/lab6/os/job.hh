/*!**************************************************************************
*! FILE NAME  : Job.hh
*!                                                            
*! DESCRIPTION: The interface to the Job module
*!              
*!************************************************************************* */

#ifndef Job_hh
#define Job_hh

/*#**************************************************************************
CLASS NAME       : Job
BASE CLASSES     : none
                   
DESCRIPTION      : A task to be performed when there is a free server
                   to take care of it. When created, a Job is 
                   automatically inserted into a set of Jobs to execute,
                   and when there is a free server the Job is ececuted.
                   Subclass and implement doit() to define a Job type.
                   Jobs should be created with new as they are delete
                   after they have been executed.
                   A low priority job will only be inserted into the
                   set of Jobs to execute if the number of Jobs in the set
                   is less than the soft limit. High priority Jobs will be
                   inserted unless the hard limit is reached (end of memory).
RESPONSIBILITIES : Perform a task when there is a free server.
*#**************************************************************************/

class Job
{
 public:
  // Public interface:
  static void createServers(unsigned int theNumberOfServers,
                            int          theStackSize,
                            unsigned int theSoftLimit);
  // create theNumberOfServers servers for handling Jobs (done once only)
  // theSoftLimit is the maximum number of waiting Jobs there may be for a
  // low priority Job to be created sucessfully.

  static void schedule(Job*);
  // Schedule job for execution when there is a free server.
  
  // Internal interface:
  
  static void execute();
  // execute a scheduled Job in the set of Jobs to execute and delete the
  // job when done.

  enum Priority { low, high };

  Priority priority() const;

  virtual ~Job();

 protected:
  Job(Priority = low);
  // raises: Error::outOfMemory


 private:
  virtual void doit() = 0;
  // the task

  const Priority myPriority;
};

#endif



