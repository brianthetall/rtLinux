#include <iostream>
#include <cstdint>
#include <ratio>
#include <unistd.h> // for sleep()
#include <chrono>

// Use a custom clock that directly reads the ARM Generic Timer registers
class ArmGenericTimerClock {
public:
    using rep = uint64_t;
    using period = std::nano;
    using duration = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<ArmGenericTimerClock>;
    static const bool is_steady = true;

    static time_point now() noexcept {
        uint64_t timer_value;
        // Read the virtual counter via inline assembly
        asm volatile("mrs %0, cntvct_el0" : "=r"(timer_value));
        return time_point(duration(timer_value));
    }

    static uint64_t get_frequency_hz() noexcept {
        uint64_t frequency;
        // Read the counter frequency via inline assembly
        asm volatile("mrs %0, cntfrq_el0" : "=r"(frequency));
        return frequency;
    }
};

int main() {
    uint64_t frequency = ArmGenericTimerClock::get_frequency_hz();
    if (frequency == 0) {
        std::cerr << "Could not get ARM Generic Timer frequency." << std::endl;
        return 1;
    }
    std::cout << "ARM Generic Timer Frequency: " << frequency << " Hz" << std::endl;

    auto start_point_raw = ArmGenericTimerClock::now().time_since_epoch().count();

    // Simulate some work
    sleep(1);

    auto end_point_raw = ArmGenericTimerClock::now().time_since_epoch().count();

    // Calculate the difference in raw counter ticks
    uint64_t raw_ticks = end_point_raw - start_point_raw;

    // Convert raw ticks to microseconds using the frequency
    double elapsed_microseconds = static_cast<double>(raw_ticks) / frequency * 1000000.0;
    
    std::cout << "Raw elapsed ticks: " << raw_ticks << std::endl;
    std::cout << "Elapsed time using raw ticks: " << elapsed_microseconds << " microseconds" << std::endl;

    return 0;
}
