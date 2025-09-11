#define _POSIX_C_SOURCE 200809L //idk what this does, if anything

#include <limits.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <time.h>
//#include <intrin.h> //TSC High Speed Timer
//#include <cstdint>

#define CYCLE_TIME 50000 //100us
#define MAX_ERROR 4000 //[ns]
#define TRUE 1
#define FALSE 0

// Reads the virtual counter via inline assembly
static inline uint64_t get_arm64_virtual_timer() {
    uint64_t timer_value;
    // mrs (move register from system register) instruction
    __asm__ volatile("mrs %0, cntvct_el0" : "=r"(timer_value));
    return timer_value;
}

// Reads the counter frequency via inline assembly
static inline uint64_t get_arm64_timer_frequency() {
    uint64_t frequency;
    // mrs instruction
    __asm__ volatile("mrs %0, cntfrq_el0" : "=r"(frequency));
    return frequency;
}

void *thread_func(void* data) {

  int passCount = 0;
  long calculatedCycleTimeNs=0, error=0, maxError=0;
  struct timespec sleep_ts;
  uint64_t frequency_hz=0;
  uint64_t start_ticks=0, prev_start_ticks=0, end_ticks=0;
  uint64_t elapsed_ticks=0;
  uint64_t elapsed_us=0;
  double elapsed_seconds=0.0;
  double timeBetweenCycles=0.0;
  while (1) {

    start_ticks = get_arm64_virtual_timer(); // Get the starting counter value

    // Get the timer frequency
    frequency_hz = get_arm64_timer_frequency();
    if (frequency_hz == 0) {
        fprintf(stderr, "Could not get ARM Generic Timer frequency.\n");
	//   return 1;
    }

    timeBetweenCycles = (double)(start_ticks - prev_start_ticks) / frequency_hz;
    
    //    printf("ARM Generic Timer [Hz]: %llu Hz Start_Ticks=%llu Prev_Start_Ticks=%llu CycleTime=%lf\n", frequency_hz, start_ticks,prev_start_ticks,timeBetweenCycles);
    printf("ARM Generic Timer [Hz]: %llu Hz  CycleTime=%lf\n", frequency_hz, timeBetweenCycles);
    printf("Elapsed Seconds=%lf Elapsed [us]=%llu SleepTime [us]=%llu\n",elapsed_seconds, elapsed_us, CYCLE_TIME - (elapsed_us*1000));
        printf("Elapsed Seconds=%lf Elapsed [us]=%llu SleepTime [us]=%llu\n",elapsed_seconds, elapsed_us, CYCLE_TIME - (elapsed_us*1000));    printf("Elapsed Seconds=%lf Elapsed [us]=%llu SleepTime [us]=%llu\n",elapsed_seconds, elapsed_us, CYCLE_TIME - (elapsed_us*1000));    printf("Elapsed Seconds=%lf Elapsed [us]=%llu SleepTime [us]=%llu\n",elapsed_seconds, elapsed_us, CYCLE_TIME - (elapsed_us*1000));    printf("Elapsed Seconds=%lf Elapsed [us]=%llu SleepTime [us]=%llu\n",elapsed_seconds, elapsed_us, CYCLE_TIME - (elapsed_us*1000));    printf("Elapsed Seconds=%lf Elapsed [us]=%llu SleepTime [us]=%llu\n",elapsed_seconds, elapsed_us, CYCLE_TIME - (elapsed_us*1000));    printf("Elapsed Seconds=%lf Elapsed [us]=%llu SleepTime [us]=%llu\n",elapsed_seconds, elapsed_us, CYCLE_TIME - (elapsed_us*1000));

    prev_start_ticks=start_ticks;
    end_ticks = get_arm64_virtual_timer(); // Get the ending counter value


    
    // Calculate the elapsed ticks
    elapsed_ticks = end_ticks - start_ticks;
    
    // Convert ticks to seconds
    elapsed_seconds = (double)elapsed_ticks / frequency_hz;
    elapsed_us = elapsed_seconds * 1000000;


    
    sleep_ts.tv_sec=0;
    sleep_ts.tv_nsec = CYCLE_TIME - (elapsed_us*1000) - 2000; //2000 is an observed fudge factor
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
