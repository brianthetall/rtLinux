#define _POSIX_C_SOURCE 200809L

#include "armTimer.h"
#include "userPrograms/pid.h" //control PID loop
#include "userPrograms/print.h" //user program
#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

void *thread_func(void* data) {

  int cycleTimeNs = *(int*)data * 1000; //convert from [us] to [ns]
  int passCount = 0, nanosleep_ret = 0;
  long calculatedCycleTimeNs=0, error=0, maxError=0;
  struct timespec sleep_ts, remaining_ts;
  uint64_t frequency_hz=0;
  uint64_t start_ticks=0, prev_start_ticks=0, end_ticks=0;
  uint64_t elapsed_ticks=0;
  uint64_t elapsed_ns=0;
  double elapsed_seconds=0.0;
  double timeBetweenCycles=0.0;
  int (*userAdd)(int,int) = &addition; //function pointer to user-function
  sleep_ts.tv_sec=0;

  PIDConfig pidCfg;
  pidCfg.setpoint = 0;
  pidCfg.Kp = 10.0;
  pidCfg.Ki = 1.0;
  pidCfg.Kd = 0.0; //disable the D term
  double pid_output = 0.0;
  char buffer[128];
  
  while (1) {

    start_ticks = get_arm64_virtual_timer(); // Get the starting counter value

    // Get the timer frequency
    frequency_hz = get_arm64_timer_frequency();
    if (frequency_hz == 0) {
        fprintf(stderr, "Could not get ARM Generic Timer frequency.\n");
	//   return 1;
    }

    //Calculate the cycle time
    timeBetweenCycles = (double)(start_ticks - prev_start_ticks) / frequency_hz;

    printf("ARM Generic Timer [Hz]: %llu Hz  CycleTime=%lf[s]\n", frequency_hz, timeBetweenCycles);
    printf("Elapsed Seconds=%lf Elapsed [ns]=%llu SleepTime [ns]=%llu\n",elapsed_seconds, elapsed_ns, cycleTimeNs - (elapsed_ns));

    prev_start_ticks=start_ticks;
    remaining_ts.tv_sec=0;
    remaining_ts.tv_nsec=0;


    /**************EXECUTE-USER-FUNCTIONS**********************************/
    printf("UserAdd 60+9=%d\n",userAdd(60,9));
    pid_control(&pidCfg,/*getProcessValue()*/ 10.0, &pid_output);
    printf("PID: %s, Output:%lf\n", pid_toString(&pidCfg, &buffer[0]), pid_output);
    /**************End-EXECUTE-USER-FUNCTIONS******************************/
    

    // Get the ending counter value
    end_ticks = get_arm64_virtual_timer();
    
    // Calculate the elapsed ticks
    elapsed_ticks = end_ticks - start_ticks;
    
    // Convert ticks to [ns]
    elapsed_ns = ((double)elapsed_ticks / frequency_hz) * 1000000000;
      
    sleep_ts.tv_nsec = cycleTimeNs - elapsed_ns - ((double)(get_arm64_virtual_timer() - end_ticks)/frequency_hz)*1000000000 - 2000; //3000; is an observed fudge factor
    do
      {
	nanosleep_ret = nanosleep(&sleep_ts, &remaining_ts);
      }while( nanosleep_ret != 0 );
    
  }

  return NULL;
}

int main(int argc, char* argv[]) {
  struct sched_param param;
  pthread_attr_t attr;
  pthread_t thread;
  int ret, cycleTime;
  if(argc != 2)
    {
      printf("Must pass cycleTime in [us]\n");
      exit(-3);
    }
  else
    {
      cycleTime = atoi( argv[1] );
      printf("CycleTime[us]=%ld\n", cycleTime);
    }

  /* Lock memory */
  if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
    printf("mlockall failed: %m\n");
    exit(-2);
  }

  /* Initialize pthread attributes (default values) */
  ret = pthread_attr_init(&attr);
  if (ret != 0) {
    printf("init pthread attributes failed\n");
    goto out;
  }

  /* Set a specific stack size */
  ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);
  if (ret != 0) {
    printf("pthread setstacksize failed\n");
    goto out;
  }

  /* Set scheduler policy and priority of pthread */
  ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO); //RR);
  if (ret != 0) {
    printf("pthread setschedpolicy failed\n");
    goto out;
  }

  param.sched_priority = 99; // Highest RT priority
  ret = pthread_attr_setschedparam(&attr, &param);
  if (ret != 0) {
    printf("pthread setschedparam failed\n");
    goto out;
  }

  /* Make the thread detached */
  ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
  if (ret != 0) {
    printf("pthread setdetachstate failed\n");
    goto out;
  }

  /* Print the current priority to verify */
  struct sched_param get_param;
  pthread_attr_getschedparam(&attr, &get_param);
  printf("Thread priority: %d\n", get_param.sched_priority);

  /* Create a pthread with specified attributes */
  ret = pthread_create(&thread, &attr, thread_func, &cycleTime);
  if (ret != 0) {
    printf("create pthread failed\n");
    goto out;
  }

  /* Join the thread and wait until it is done */
  ret = pthread_join(thread, NULL);
  if (ret != 0) {
    printf("join pthread failed: %m\n");
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
