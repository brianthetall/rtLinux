#ifndef __PID_H__
#define __PID_H__

#include "../armTimer.h"
#include <stdio.h>

typedef struct {
    double setpoint;
    double Kp;
    double Ki;
    double Kd;
} PIDConfig;

void pid_control(const PIDConfig *config, double current_value, double *output) {
    static double integral = 0.0;
    static double previous_error = 0.0;
    static double last_update = 0.0;

    // Obtain current time with high-resolution timing
    double current_time = get_current_time();

    // Calculate the time difference
    double dt = current_time - last_update;

    // Prevent division by zero or negative time
    if (dt <= 0) {
        dt = 1e-6;
    }

    // Compute the error
    double error = config->setpoint - current_value;

    // Calculate control terms
    *output = config->Kp * error 
              + config->Ki * integral * dt 
              + config->Kd * (error - previous_error) / dt;

    // Apply output limits to prevent actuator saturation
    if (*output > 100.0) {
        *output = 100.0;
    } else if (*output < -100.0) {
        *output = -100.0;
    }

    // Update integral and previous_error for next iteration
    integral += error * dt;
    previous_error = error;

    // Store the current time for the next cycle
    last_update = current_time;
}

char* pid_toString( PIDConfig *cfg, char *buffer )
{
  sprintf( buffer,
	   "Kp=%f, Ki=%f, Kd=%f, Setpoint=%lf",
	   cfg->Kp, cfg->Ki, cfg->Kd, cfg->setpoint);
  return buffer;
}

#endif
