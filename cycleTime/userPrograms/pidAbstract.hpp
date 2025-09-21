#ifndef __PID_ABSTRACT_HPP__
#define __PID_ABSTRACT_HPP__

#include "../armTimer.h"
#include <iostream>
#include <string>
#include <string.h> //memset

class PidAbstract
{
private:

  double error, previous_error;
  double output; //PID output [-100,100]
  double integral;

  struct PidConfig
  {
    double setpoint;
    double Kp;
    double Ki;
    double Kd;
  };

public:
  PidConfig cfg;

  PidAbstract() = default;
  PidAbstract(double Kp, double Ki, double Kd):cfg.Kp(Kp), cfg.Ki(Ki), cfg.Kd(Kd){}
  virtual ~PidAbstract() = default;
  virtual PidConfig getCfg(void) = 0;
  virtual void setSetpoint(double setpoint) = 0;
  virtual void setKp(double Kp) = 0;
  virtual void setKi(double Ki) = 0;
  virtual void setKd(double Kd) = 0;
  virtual double pid_control(double current_process_value) = 0;
  virtual std::string toString(void) = 0;
};

#endif
