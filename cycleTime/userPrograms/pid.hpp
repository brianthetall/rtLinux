#ifndef __PID_HPP__
#define __PID_HPP__

#include "../armTimer.h"
#include <iostream>
#include <string>
#include <string.h> //memset

class Pid
{
private:
  double output; //PID output [-100,100]
  
  struct PidConfig
  {
    double setpoint;
    double Kp;
    double Ki;
    double Kd;
  };

public:
  PidConfig cfg;

  Pid()
  {
    cfg.setpoint = 0.0;
    cfg.Kp = 1.0;
    cfg.Ki = 0.0;
    cfg.Kd = 0.0;
    output = 0.0;
  }

  Pid(double Kp, double Ki, double Kd)
  {
    cfg.setpoint = 0.0;
    cfg.Kp = Kp;
    cfg.Ki = Ki;
    cfg.Kd = Kd;
    output = 0.0;
  }

  const PidConfig getCfg(void)
  {
    return this->cfg;
  }

  void setSetpoint(double setpoint)
  {
    this->cfg.setpoint = setpoint;
  }

  void setKp(double Kp)
  {
    this->cfg.Kp = Kp;    
  }

  void setKi(double Ki)
  {
    this->cfg.Ki = Ki;
  }

  void setKd(double Kd)
  {
    this->cfg.Kd = Kd;
  }
  
  /**
   * Return PID output [double]
   */
  double pid_control(double current_process_value) {
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
    double error = this->cfg.setpoint - current_process_value;
    std::cout<<"Error="<<error<<std::endl;;

    // Calculate control terms
    this->output = this->cfg.Kp * error 
      + this->cfg.Ki * integral * dt 
      + this->cfg.Kd * (error - previous_error) / dt;

    std::cout<<"Integral="<<this->cfg.Ki * integral * dt <<std::endl;
    std::cout<<"Output="<<this->output<<std::endl;
    
    // Apply output limits to prevent actuator saturation
    if (this->output > 100.0) {
      this->output = 100.0;
    } else if (this->output < -100.0) {
      this->output = -100.0;
    }

    // Update integral and previous_error for next iteration
    integral += error * dt;
    previous_error = error;

    // Store the current time for the next cycle
    last_update = current_time;
    std::cout<<"Output="<<this->output<<std::endl;
    return this->output;
  }

  std::string pid_toString(void)
  {
    char doubleBuf[32]; //use snprintf
    std::string retval;
    retval.reserve(128); //don't reallocate on every +=

    //read the doubles
    retval="Kp=";
    memset(doubleBuf, 0, sizeof(doubleBuf));
    snprintf(doubleBuf, sizeof(doubleBuf), "%.3f", this->cfg.Kp);
    retval+=doubleBuf; //Kp

    retval+=" Ki=";
    memset(doubleBuf, 0, sizeof(doubleBuf));
    snprintf(doubleBuf, sizeof(doubleBuf), "%.3f", this->cfg.Ki);
    retval+=doubleBuf; //Ki

    retval+=" Kd=";
    memset(doubleBuf, 0, sizeof(doubleBuf));
    snprintf(doubleBuf, sizeof(doubleBuf), "%.3f", this->cfg.Kd);
    retval+=doubleBuf; //Kd

    retval+=" Setpoint=";
    memset(doubleBuf, 0, sizeof(doubleBuf));
    snprintf(doubleBuf, sizeof(doubleBuf), "%.3lf", this->cfg.setpoint);
    retval+=doubleBuf; //Setpoint

    return retval;
  }
  
};
#endif
