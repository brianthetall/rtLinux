#ifndef __PID_HPP__
#define __PID_HPP__

#include "../armTimer.h"
#include <iostream>
#include <string>
#include <string.h> //memset

class Pid
{
private:
  const double maxGainAdjustment = 0.0001;
  const double maxKp = 0.7;
  const double maxKi = 0.1;
  const double maxKd = 0.0000003;
  double error, previous_error;
  double output; //PID output [-100,100]
  double integral;
  bool adaptiveTuningEnabled;
  
  struct PidConfig
  {
    double setpoint;
    double Kp;
    double Ki;
    double Kd;
  };

  /*
    Adaptive PID Control
    Adjust gains as needed as underlying systems change
   */
  void adjustGains() {
    // Example: Increase Kp if the system is under-damped
    static int consecutiveOscillations = 0;
    static bool inOscillation = false;

    // Detect oscillations (simplified example)
    double currentErrorSign = this->error > 0 ? 1 : -1;
    double previousErrorSign = this->previous_error > 0 ? 1 : -1;

    if (currentErrorSign != previousErrorSign)
      {
	consecutiveOscillations++;
	inOscillation = true;
      }
    else
      {
	consecutiveOscillations = 0;
	inOscillation = false;
      }

    // Adjust Kp based on oscillation detection
    if (inOscillation && consecutiveOscillations > 10)
      {
	// Reduce Kp to dampen oscillations
	cfg.Kp -= maxGainAdjustment * 10;
	std::cout<<"Kp="<<cfg.Kp<<"inOsc and > 3"<<std::endl;
      }
    else
      {
	// Increase Kp for faster response
	cfg.Kp += maxGainAdjustment * 1;
	std::cout<<"Kp="<<cfg.Kp<<"else"<<std::endl;
      }

    // Detect persistent static errors to adjust Ki
    if (/*std::abs(this->error) > 5.0 &&*/ consecutiveOscillations < 2)
      {
	// Increase Ki to reduce static error
	cfg.Ki += maxGainAdjustment * 0.5;
	std::cout<<"Ki="<<cfg.Ki<<"consecutiveOsc < 2, "<<consecutiveOscillations<<std::endl;
      }
    else
      {
	// Decrease Ki to prevent integrator windup
	cfg.Ki -= maxGainAdjustment * 0.1;
	std::cout<<"Ki="<<cfg.Ki<<"else, COsc="<<consecutiveOscillations<<std::endl;	
      }

    // Ensure gains do not go out of reasonable bounds
    if (cfg.Kp > maxKp) cfg.Kp = maxKp;
    //    if (cfg.Ki < 0.0) cfg.Ki = 0.0;
    if (cfg.Ki > maxKi) cfg.Ki = maxKi;
    if (cfg.Kd > maxKd) cfg.Kd = maxKd;
  }
  
public:
  PidConfig cfg;

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

  Pid(double Kp, double Ki, double Kd, bool adaptiveTuningEnabled):Pid(Kp,Ki,Kd)
  {
    this->adaptiveTuningEnabled = adaptiveTuningEnabled;
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

    if(this->adaptiveTuningEnabled)
      {
	this->adjustGains(); //if it isn't broken, fix it till it is!
      }
    
    return this->output;
  }

  std::string toString(void)
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
