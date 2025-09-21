#ifndef __PID_HPP__
#define __PID_HPP__

#include "pidAbstract.hpp"

class Pid : public PidAbstract
{
public:


  Pid()
  {
    cfg.setpoint = 0.0;
    cfg.Kp = 1.0;
    cfg.Ki = 0.0;
    cfg.Kd = 0.0;
    output = 0.0;
    adaptiveTuningEnabled = false;
    integral = error = previous_error = 0.0;
  }

  Pid(double Kp, double Ki, double Kd)
  {
    cfg.setpoint = 0.0;
    cfg.Kp = Kp;
    cfg.Ki = Ki;
    cfg.Kd = Kd;
    output = 0.0;
    adaptiveTuningEnabled = false;
    integral = error = previous_error = 0.0;
  }

  Pid(double Kp, double Ki, double Kd):Pid(Kp,Ki,Kd)
  {}
  
  virtual const PidConfig getCfg(void)
  {
    return this->cfg;
  }

  virtual void setSetpoint(double setpoint)
  {
    this->cfg.setpoint = setpoint;
  }

  virtual void setKp(double Kp)
  {
    this->cfg.Kp = Kp;    
  }

  virtual void setKi(double Ki)
  {
    this->cfg.Ki = Ki;
  }

  virtual void setKd(double Kd)
  {
    this->cfg.Kd = Kd;
  }
  
  /**
   * Return PID output [double]
   */
  virtual double pid_control(double current_process_value)
  {
    static double last_update = 0.0;

    // Obtain current time with high-resolution timing
    double current_time = get_current_time();

    // Calculate the time difference
    double dt = current_time - last_update;

    // Prevent division by zero or negative time
    if (dt <= 0 || dt > 1) {
      dt = 1e-6;
    }

    // Compute the error
    this->error = current_process_value - this->cfg.setpoint;
    std::cout<<"Error="<<this->error<<std::endl;;

    // Calculate control terms
    this->output = this->cfg.Kp * this->error 
      + this->cfg.Ki * this->integral * dt
      + this->cfg.Kd * (this->error - this->previous_error) / dt;

    //    std::cout<<"Integral="<<this->cfg.Ki * this->integral * dt <<std::endl;
    std::cout<<"ki="<<this->cfg.Ki<<" integral="<<this->integral<<" dt="<<dt<<std::endl;
    
    // Apply output limits to prevent actuator saturation
    if (this->output > 100.0) {
      this->output = 100.0;
    } else if (this->output < -100.0) {
      this->output = -100.0;
    }

    // Update integral and previous_error for next iteration
    this->integral += this->error * dt;
    this->previous_error = this->error;

    // Store the current time for the next cycle
    last_update = current_time;
    
    return this->output;
  }

  virtual std::string toString(void)
  {
    char doubleBuf[32]; //use snprintf
    std::string retval;
    retval.reserve(128); //don't reallocate on every +=

    //read the doubles
    retval="Kp=";
    memset(doubleBuf, 0, sizeof(doubleBuf));
    snprintf(doubleBuf, sizeof(doubleBuf), "%.9lf", this->cfg.Kp);
    retval+=doubleBuf; //Kp

    retval+=" Ki=";
    memset(doubleBuf, 0, sizeof(doubleBuf));
    snprintf(doubleBuf, sizeof(doubleBuf), "%.9lf", this->cfg.Ki);
    retval+=doubleBuf; //Ki

    retval+=" Kd=";
    memset(doubleBuf, 0, sizeof(doubleBuf));
    snprintf(doubleBuf, sizeof(doubleBuf), "%.9lf", this->cfg.Kd);
    retval+=doubleBuf; //Kd

    retval+=" Setpoint=";
    memset(doubleBuf, 0, sizeof(doubleBuf));
    snprintf(doubleBuf, sizeof(doubleBuf), "%.9lf", this->cfg.setpoint);
    retval+=doubleBuf; //Setpoint

    retval+=" Output=";
    memset(doubleBuf, 0, sizeof(doubleBuf));
    snprintf(doubleBuf, sizeof(doubleBuf), "%.9lf", this->output);
    retval+=doubleBuf; //Setpoint    

    return retval;
  }
  
};
#endif
