#include <stdio.h>
#include <stdint.h>
#include <unistd.h> // for sleep()

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

int main() {
    uint64_t frequency_hz;
    uint64_t start_ticks, end_ticks;
    uint64_t elapsed_ticks;
    double elapsed_seconds;

    // Get the timer frequency
    frequency_hz = get_arm64_timer_frequency();
    if (frequency_hz == 0) {
        fprintf(stderr, "Could not get ARM Generic Timer frequency.\n");
        return 1;
    }
    printf("ARM Generic Timer Frequency: %llu Hz\n", frequency_hz);

    // Get the starting counter value
    start_ticks = get_arm64_virtual_timer();

    // Simulate some work, like a 1-second delay
    sleep(1);

    // Get the ending counter value
    end_ticks = get_arm64_virtual_timer();

    // Calculate the elapsed ticks
    elapsed_ticks = end_ticks - start_ticks;

    // Convert ticks to seconds
    elapsed_seconds = (double)elapsed_ticks / frequency_hz;

    printf("Start ticks: %llu\n", start_ticks);
    printf("End ticks:   %llu\n", end_ticks);
    printf("Elapsed ticks: %llu\n", elapsed_ticks);
    printf("Elapsed time: %f seconds\n", elapsed_seconds);

    return 0;
}
