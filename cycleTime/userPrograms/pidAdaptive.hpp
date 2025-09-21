#ifndef __PID_ADAPTIVE_HPP__
#define __PID_ADAPTIVE_HPP__

#include "pid.hpp"

class PidAdaptive : public Pid
{
private:
  const double maxGainAdjustment = 0.001;
  const double maxKp = 1.0;
  const double maxKi = 50.0;
  const double maxKd = 0.0000003;

  /*
    Adaptive PID Control
    Adjust gains as needed as underlying systems change
  */
  void adjustGains(void) {
    // Example: Increase Kp if the system is under-damped
    static int consecutiveOscillations = 0;
    static bool inOscillation = false;

    // Detect oscillations (simplified example)
    int currentErrorSign = this->error > 0 ? 1 : -1;
    int previousErrorSign = this->previous_error > 0 ? 1 : -1;

    if (currentErrorSign != previousErrorSign)
      {
	std::cout<<"OSC: currentError="<<this->error<<", PrevError="<<this->previous_error<<std::endl;
	consecutiveOscillations++;
	inOscillation = true;
      }
    else
      {
	std::cout<<"CLEAR of OSC"<<std::endl;
	consecutiveOscillations = 0;
	inOscillation = false;
      }

    // Adjust Kp based on oscillation detection
    if (inOscillation && consecutiveOscillations > 30 and std::fabs(this->error) > 0.000001000)
      {
	// Reduce Kp to dampen oscillations
	//cfg.Kp -= maxGainAdjustment * 10;
	cfg.Kp *= 0.6;
	std::cout<<"Kp="<<cfg.Kp<<" Decreased"<<std::endl;

	
	// Decrease Ki to prevent integrator windup
	//	cfg.Ki -= maxGainAdjustment * 10;
	//	cfg.Ki *= 0.8;
	this->integral *= 0.8;
	std::cout<<"Ki="<<cfg.Ki<<" else, COsc="<<consecutiveOscillations<<std::endl;
	
	
      }
    else if( std::fabs(this->error) > 0.000002500 && !inOscillation)
      {
	// Increase Kp for faster response
	cfg.Kp += maxGainAdjustment * 0.01;
	std::cout<<"Kp="<<cfg.Kp<<" Increased"<<std::endl;

	// Increase Ki to reduce static error
	cfg.Ki += maxGainAdjustment * 0.001;
	std::cout<<"Ki="<<cfg.Ki<<" consecutiveOsc < 2, "<<consecutiveOscillations<<std::endl;
      }

    /*
    // Detect persistent static errors to adjust Ki
    if (std::abs(this->error) > 5.0 && consecutiveOscillations < 10)
    {
    // Increase Ki to reduce static error
    cfg.Ki += maxGainAdjustment * 10.0;
    std::cout<<"Ki="<<cfg.Ki<<" consecutiveOsc < 2, "<<consecutiveOscillations<<std::endl;
    }
    else
    {
    // Decrease Ki to prevent integrator windup
    cfg.Ki -= maxGainAdjustment * 0.1;
    std::cout<<"Ki="<<cfg.Ki<<" else, COsc="<<consecutiveOscillations<<std::endl;
    }
    */
      
    // Ensure gains do not go out of reasonable bounds
    if (cfg.Kp > maxKp) cfg.Kp = maxKp;
    //    if (cfg.Ki < 0.0) cfg.Ki = 0.0;
    if (cfg.Ki > maxKi) cfg.Ki = maxKi;
    if (cfg.Kd > maxKd) cfg.Kd = maxKd;

    //what about minimums?
    
  }

  
public:
  PidAdaptive() = default;
  PidAdaptive(double Kp, double Ki, double Kd):Pid(Kp,Ki,Kd){}
  virtual ~PidAdaptive() = default;

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

    this->adjustGains(); //if it isn't broken, fix it till it is!
        
    // Update integral and previous_error for next iteration
    this->integral += this->error * dt;
    this->previous_error = this->error;

    // Store the current time for the next cycle
    last_update = current_time;
    
    return this->output;
  }
  
};

#endif
