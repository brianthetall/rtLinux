#ifndef __SINE_HPP__
#define __SINE_HPP__

#include "functionGenerator.hpp"

#ifndef M_PI
#define M_PI 3.141592653589793 //Not in the math.h?
#endif

class Sine : public FunctionGenerator
{
private:
  const double pi2 = (double)2*M_PI;
  double angle_increment;
  
public:
  Sine() = default;
  Sine(double frequency, double amplitude, double phase_shift, double sampling_rate, double offset):FunctionGenerator(frequency,amplitude,phase_shift,sampling_rate,offset)
  {
    // Calculate the angle increment per sample
    this->angle_increment = (2 * M_PI * this->getFrequency()) / this->getSampleRate();
    this->output = 0.0;
  }

  virtual double next(void)
  {

    // Update the output for the next sample
    this->output += angle_increment + this->phase_shift;
    this->output = fmod(this->output, (double)2*M_PI); //keep the output [0,2pi)
  
    // Generate the sine wave value
    // Factor in the Amplitude & DC offset:
    return this->amplitude * sin(this->output) + this->offset;;      
  }

  virtual void reset(void)
  {
    this->output = 0.0; //reset the calculated output
  }
  
  virtual double getSampleRate(void) const{ return this->sampling_rate; }
  virtual double getFrequency(void) const{ return this->frequency; }
  virtual double getAmplitude(void) const{ return this->amplitude; }
  virtual double getPhaseShift(void) const{ return this->phase_shift; }
  virtual double getOffset(void) const{ return this->offset; }
  
};

#endif
