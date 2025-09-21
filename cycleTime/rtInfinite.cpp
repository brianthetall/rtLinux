#define _POSIX_C_SOURCE 200809L

#include "armTimer.h"
#include "userPrograms/sine.hpp" //Waveform Generator
#include "userPrograms/ramp.hpp" //Waveform Generator
#include "userPrograms/square.hpp" //Waveform Generator
#include "userPrograms/pid.hpp" //PID loop
#include "userPrograms/pidAdaptive.hpp" //PID Adaptive loop
#include "userPrograms/print.h" //user program
#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <time.h>
#include <memory>

#define TRUE 1
#define FALSE 0

class RtInfinite
{
  
public:
  RtInfinite() = default; //default constructor

  static void *thread_func(void* data)
  {
    bool virgin = true;
    uint count=0;
    
    int cycleTimeNs = *(int*)data * 1000; //convert from [us] to [ns]
    double cycleTimeSec = (double)cycleTimeNs/1000000000;
    int nanosleep_ret = 0;
    long calculatedCycleTimeNs=0;
    struct timespec sleep_ts, remaining_ts; //time to sleep, and remaining time to sleep if interrupted.
    uint64_t frequency_hz=0;
    uint64_t start_ticks=0, prev_start_ticks=0, end_ticks=0;
    uint64_t elapsed_ticks=0;
    uint64_t elapsed_ns=0;
    double timeBetweenCycles=0.0;
    uint64_t timeBetweenCyclesNs=0;
    uint64_t sleepTime=0;
    
    int (*userAdd)(int,int) = &addition; //function pointer to user-function
    sleep_ts.tv_sec=0;

    double fudge=0.0;
    int64_t fudgeInt=0;
    std::shared_ptr<PidAbstract>cycleTimeFudgePid = std::make_shared<PidAdaptive>(0.7, static_cast<double>(25.14), /*0.00000014*/ 0);
    cycleTimeFudgePid->setSetpoint(cycleTimeSec);
    
    Pid pid(1.0, 0.0, 0.0); //Kp,Ki,Kd
    std::shared_ptr<FunctionGenerator>generator = std::make_shared<Sine>(0.10, 140, 0.0, 1.0/cycleTimeSec, 0); //Waveform Configuration:
  
    while (1) {

      start_ticks = get_arm64_virtual_timer(); // Get the starting counter value

      // Get the timer frequency
      frequency_hz = get_arm64_timer_frequency();
      if (frequency_hz == 0) {
        fprintf(stderr, "Could not get ARM Generic Timer frequency.\n");
	exit(1); //Show stopper!
      }

      //Calculate the cycle time
      timeBetweenCycles = (double)(start_ticks - prev_start_ticks) / frequency_hz;
      timeBetweenCyclesNs = timeBetweenCycles * 1000000000;
      
      std::cout << std::fixed << std::setprecision(9) << std::showpoint;
      std::cout << "CycleTime:"<<cycleTimeSec<<"[s]"<<std::endl;
      std::cout << "CycleTime=" << timeBetweenCycles << "[s]" << std::endl;
      std::cout << "Elapsed [ns]=" << elapsed_ns << std::endl << "SleepTime [ns]=" << sleepTime << std::endl;

      prev_start_ticks=start_ticks;
      remaining_ts.tv_sec=0;
      remaining_ts.tv_nsec=0;


      /**************EXECUTE-USER-FUNCTIONS**********************************/
      //printf("UserAdd 60+9=%d\n",userAdd(60,9));
      //      double signal = generator->next();
      //      std::cout << "PID: " << pid.toString() << " Signal=" << signal <<
      //	" Output:" << pid.pid_control(signal) << std::endl << std::endl;
      if(!virgin && count>3)
	{
	  fudge = cycleTimeFudgePid->pid_control( timeBetweenCycles );
	  fudgeInt = virgin ? 0:static_cast<int64_t>(fudge*1000000000); //convert_to_ns
	  //	  exit(100);
	}
      virgin = false; count++;
      std::cout<<cycleTimeFudgePid->toString()<<std::endl;
      std::cout<<"PID_Fudge="<<fudgeInt<<std::endl<<std::endl;
      /**************End-EXECUTE-USER-FUNCTIONS******************************/
      
      // Get the ending counter value
      end_ticks = get_arm64_virtual_timer();
    
      // Calculate the elapsed ticks
      elapsed_ticks = end_ticks - start_ticks;
    
      // Convert ticks to [ns]
      elapsed_ns = ((double)elapsed_ticks / frequency_hz) * 1000000000;
      sleepTime = cycleTimeNs - elapsed_ns; //estimated sleep time

      sleep_ts.tv_nsec = sleepTime - fudgeInt > 0 ? sleepTime - fudgeInt : sleepTime; //is an observed fudge factor
      do
	{
	  nanosleep_ret = nanosleep(&sleep_ts, &remaining_ts);
	}while( nanosleep_ret != 0 );
    
    }

    return NULL;
  }

};

int main(int argc, char* argv[]) {
  struct sched_param param;
  pthread_attr_t attr;
  pthread_t thread;
  int ret, cycleTime;
  if(argc != 2)
    {
      std::cout << "Must pass cycleTime in [us]" << std::endl;
      exit(-3);
    }
  else
    {
      cycleTime = atoi( argv[1] );
      std::cout << "CycleTime[us]=" << cycleTime << std::endl;
    }

  /* Lock memory */
  if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
    std::cout << "mlockall failed" <<std::endl;
    exit(-2);
  }

  /* Initialize pthread attributes (default values) */
  ret = pthread_attr_init(&attr);
  if (ret != 0) {
    std::cout << "init pthread attributes failed" << std::endl;
    goto out;
  }

  /* Set a specific stack size */
  ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
  if (ret != 0) {
    std::cout << "pthread setstacksize failed" << std::endl;
    goto out;
  }

  /* Set scheduler policy and priority of pthread */
  ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO); //RR);
  if (ret != 0) {
    std::cout << "pthread setschedpolicy failed" << std::endl;
    goto out;
  }

  param.sched_priority = 99; // Highest RT priority
  ret = pthread_attr_setschedparam(&attr, &param);
  if (ret != 0) {
    std::cout << "pthread setschedparam failed" << std::endl;
    goto out;
  }

  /* Make the thread detached */
  ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if (ret != 0) {
    std::cout << "pthread setdetachstate failed" << std::endl;
    goto out;
  }

  /* Print the current priority to verify */
  struct sched_param get_param;
  pthread_attr_getschedparam(&attr, &get_param);
  std::cout << "Thread priority: " << get_param.sched_priority << std::endl;

  /* Create a pthread with specified attributes */
  ret = pthread_create(&thread, &attr, RtInfinite::thread_func, &cycleTime);
  if (ret != 0) {
    std::cout << "create pthread failed" << std::endl;
    goto out;
  }

  /* Join the thread and wait until it is done */
  ret = pthread_join(thread, NULL);
  if (ret != 0) {
    std::cout << "join pthread failed!" << std::endl;
  }

  while(1)
    {
      struct timespec current_time;
      struct timespec sleep_ts = {1, 0};
      clock_gettime(CLOCK_REALTIME, &current_time);
      nanosleep(&sleep_ts, NULL);
    }

 out:
  return ret;
}
