#ifndef __FUNCTION_GENERATOR_HPP__
#define __FUNCTION_GENERATOR_HPP__

#include <cmath>
#include <cstddef>   // for std::size_t

class FunctionGenerator
{
protected:
  double frequency;
  double amplitude;
  double phase_shift;
  double sampling_rate;
  double offset; //DC offset

  double output; //calculated output phase; pre-amplitude & DC-offset

public:

  FunctionGenerator() = default;
  FunctionGenerator(double frequency, double amplitude, double phase_shift, double sampling_rate, double offset):frequency(frequency),amplitude(amplitude),phase_shift(phase_shift),sampling_rate(sampling_rate),offset(offset){}
  
  /// Destructor must be virtual for safe polymorphic deletion
  virtual ~FunctionGenerator() = default;

  /// Return the next sample of the generated waveform.
  /// \return Next sample value (double).
  virtual double next() = 0;

  /// Reset the generator to its initial state.
  virtual void reset() = 0;

  /// Return the sampling rate (samples per second).
  /// (Optional – not used in the demo, but handy for derived classes.)
  virtual double getSampleRate() const = 0;
  virtual double getFrequency(void) const = 0;
  virtual double getAmplitude(void) const = 0;
  virtual double getPhaseShift(void) const = 0;
  virtual double getOffset(void) const = 0;
};

#endif
