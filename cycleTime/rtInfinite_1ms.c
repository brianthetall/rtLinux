#define _POSIX_C_SOURCE 200809L //idk what this does, if anything

#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>

#define CYCLE_TIME 1000000; //1ms; 
#define MAX_ERROR 4000 //[ns]
#define TRUE 1
#define FALSE 0

long calculateDelta( struct timespec start, struct timespec previousStart )
{
  long cycleTime = start.tv_nsec - previousStart.tv_nsec;
  long oneSecond = 1000000000;
  if( cycleTime < 0 )
    {
      //we have jumped a second
      cycleTime = oneSecond - previousStart.tv_nsec + start.tv_nsec;
    }  
  return cycleTime;
}

void *thread_func(void* data) {

  int passCount = 0;
  long calculatedCycleTimeNs=0, error=0, maxError=0;
  struct timespec start_time, end_time, last_time, sleep_ts;
  
  while (1) {
    clock_gettime(CLOCK_REALTIME, &start_time); //get starting time

    calculatedCycleTimeNs = calculateDelta( start_time, last_time );
    error = calculatedCycleTimeNs-CYCLE_TIME;

    printf("Start Time: %ld seconds and %09ld nanoseconds cycleTime=%09ld[ns], error=%05ld[ns], maxError=%05ld[ns]\n",
           start_time.tv_sec, start_time.tv_nsec, calculatedCycleTimeNs, error, maxError );

    if( abs(error) >= MAX_ERROR && passCount++>1)
      {
	maxError = error > maxError ? error : maxError;
	//	return NULL; //Kill the program, we did not meet RT constraint.
      }
        
    last_time = start_time; //store for next loop's printf()

    clock_gettime(CLOCK_REALTIME, &end_time); //get ending time
    
    sleep_ts.tv_sec=0;
    sleep_ts.tv_nsec = CYCLE_TIME-calculateDelta(end_time, start_time); //write a better difference function that uses the second place?
    nanosleep(&sleep_ts, NULL);
  }

  return NULL;
}

int main(int argc, char* argv[]) {
  struct sched_param param;
  pthread_attr_t attr;
  pthread_t thread;
  int ret;

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
  ret = pthread_create(&thread, &attr, thread_func, NULL);
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
