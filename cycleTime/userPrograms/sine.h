#ifndef __SINE_H__
#define __SINE_H__

#include <stdio.h>
#include <math.h>
#include <unistd.h>

#define PI 3.141592653589793 //Not in the math.h?

typedef struct
{
  double frequency;
  double amplitude;
  double phase_shift;
  double sampling_rate;
  double angle; //phase angle
}SineConfig;

double sine(SineConfig* const cfg)
{
  const double pi2 = (double)2*PI;
  // Calculate the angle increment per sample
  double angle_increment = (2 * PI * cfg->frequency) / cfg->sampling_rate;
  printf("[const]AngleIncrement = %lf\n",angle_increment);

  // Update the angle for the next sample
  cfg->angle += angle_increment + cfg->phase_shift;
  //cfg->angle = fmod(cfg->angle, (double)2*PI);
  printf("Calculated Angle = %lf\n",cfg->angle);
  
  // Generate the sine wave value
  double calcuated_angle = fmod(cfg->angle, pi2);
  printf("calculated_angle=%lf",calcuated_angle);

  //Factor in the Amplitude:
  double sine_value = cfg->amplitude * sin(calcuated_angle);

  // Print the generated sine wave value
  printf("Sine Wave Value: %.6f\n", sine_value);
  return sine_value;  
}
#endif
