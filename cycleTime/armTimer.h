#ifndef __ARM_TMR__
#define __ARM_TMR__

#include <limits.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <time.h>

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

static inline double get_current_time()
{
  return (double)get_arm64_virtual_timer() / get_arm64_timer_frequency();
}

#endif
