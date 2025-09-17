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
  
public:
  Sine() = default;
  Sine(double frequency, double amplitude, double phase_shift, double sampling_rate, double offset):FunctionGenerator(frequency,amplitude,phase_shift,sampling_rate,offset)
  {}

  double next(void)
  {

    // Calculate the angle increment per sample
    double angle_increment = (2 * M_PI * this->getFrequency()) / this->getSampleRate();
    //printf("[const]AngleIncrement = %lf\n",angle_increment);

    // Update the angle for the next sample
    this->angle += angle_increment + this->phase_shift;
    this->angle = fmod(this->angle, (double)2*M_PI);
    //printf("Calculated Angle = %lf\n",this->angle);
  
    // Generate the sine wave value
    //double calcuated_angle = fmod(this->angle, pi2);
    //printf("calculated_angle=%lf",calcuated_angle);

    //Factor in the Amplitude:
    double sine_value = this->amplitude * sin(this->angle) + this->offset;

    // Print the generated sine wave value
    //    printf("Sine Wave Value: %.6f\n", sine_value);
    
    return sine_value;      
  }

  void reset(void)
  {
    this->angle = 0.0; //reset the calculated angle
  }
  
  virtual double getSampleRate(void) const{ return this->sampling_rate; }
  virtual double getFrequency(void) const{ return this->frequency; }
  virtual double getAmplitude(void) const{ return this->amplitude; }
  virtual double getPhaseShift(void) const{ return this->phase_shift; }
  virtual double getOffset(void) const{ return this->offset; }
  
};

#endif
