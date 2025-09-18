#ifndef __RAMP_HPP__
#define __RAMP_HPP__

#include "functionGenerator.hpp"

class Ramp : public FunctionGenerator
{
private:
  double phaseIncrement;

public:
  Ramp() = default;
  Ramp(double frequency, double amplitude, double phase_shift, double sampling_rate, double offset):FunctionGenerator(frequency,amplitude,phase_shift,sampling_rate,offset)
  {
    this->phaseIncrement = this->frequency / this->sampling_rate; // phase in [0,1) per sample
    this->output = 0.0;
  }

  virtual double next(void)
  {
    // Produce the ramp value (0.0 → 1.0)
    // Increment & wrap the phase
    this->output += this->phaseIncrement;
    this->output = std::fmod(this->output, 1.0);
    return this->output * this->amplitude + this->offset; //multiply the output by amplitude & add DC-offset
  }

  virtual void reset(void)
  {
    this->output = 0.0;
  }
    
  virtual double getSampleRate(void) const{ return this->sampling_rate; }
  virtual double getFrequency(void) const{ return this->frequency; }
  virtual double getAmplitude(void) const{ return this->amplitude; }
  virtual double getPhaseShift(void) const{ return this->phase_shift; }
  virtual double getOffset(void) const{ return this->offset; }

};

#endif
