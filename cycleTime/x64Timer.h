#ifndef __X64_TMR__
#define __X64_TMR__

/* x86_64 timer header – analogous to armTimer.h [1] */

#include <stdint.h>

/* ------------------------------------------------------------- */
/* 1. Read the current TSC value                                 */
/* ------------------------------------------------------------- */
static inline uint64_t get_virtual_timer(void)
{
#if defined(__GNUC__) || defined(__clang__)
    unsigned int hi, lo;
    /* rdtsc places the low 32 bits in EAX, high 32 bits in EDX */
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)lo) | (((uint64_t)hi) << 32);
#else
    /* Unsupported compiler – fallback to a dummy value */
    return 0;
#endif
}

/* ------------------------------------------------------------- */
/* 2. Return the TSC frequency (Hz) – placeholder                */
/* ------------------------------------------------------------- */
static inline uint64_t get_timer_frequency(void)
{
    /* Most practical implementations obtain the frequency from
       the operating system or from the CPUID instruction.  For the
       purposes of this header we use a common default value.
       Replace this with the actual frequency for your platform. */
    return 1000000000ULL;          /* 1 GHz – placeholder value */
}

/* ------------------------------------------------------------- */
/* 3. Convert the counter to a time value in seconds            */
/* ------------------------------------------------------------- */
static inline double get_current_time(void)
{
    return (double)get_virtual_timer() /
           (double)get_timer_frequency();
}

#endif /* __X64_TMR__ */
