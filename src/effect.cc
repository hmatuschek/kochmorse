#include "effect.hh"
#include <time.h>
#include <cmath>
#include "globals.hh"


/* ******************************************************************************************** *
 * Noise effect
 * ******************************************************************************************** */
NoiseEffect::NoiseEffect(QIODevice *sink, bool enabled, float snr, QObject *parent)
  : QIODevice(parent), _sink(sink), _enabled(enabled), _snr(snr)
{
  // init RNG
  srand(time(0));
  connect(_sink, SIGNAL(bytesWritten(qint64)), this, SIGNAL(bytesWritten(qint64)));
  open(QIODevice::WriteOnly);
}

NoiseEffect::~NoiseEffect() {
  // pass...
}

void
NoiseEffect::gaussRNG(float &a, float &b) {
  float x = 2*float(rand())/RAND_MAX - 1;
  float y = 2*float(rand())/RAND_MAX - 1;
  float s = x*x + y*y;
  while (s >= 1) {
    x = 2*float(rand())/RAND_MAX - 1;
    y = 2*float(rand())/RAND_MAX - 1;
    s = x*x + y*y;
  }
  a = x*std::sqrt(-2*std::log(s)/s);
  b = y*std::sqrt(-2*std::log(s)/s);
}

qint64
NoiseEffect::writeData(const char *data, qint64 len) {
  // If disabled -> skip
  if (! _enabled)
    return _sink->write(data, len);

  // Number of frames
  size_t n = (len/2);

  // Copy input buffer
  QByteArray buffer(data, 2*n);
  int16_t *in = reinterpret_cast<int16_t *>(buffer.data());
  // Get noise and signal scale-factor.
  float n_factor=0, s_factor=1;
  if (0 > _snr) {
    // If SNR < 0, keep noise at level "1" == 2**12, but reduce signal
    n_factor = ((1<<14)-1); s_factor = std::pow(10, _snr/20-6);
  } else {
    // If SNR > 0, keep signal at level "1" and scale noise down.
    n_factor = ((1<<14)-1)*std::pow(10, -_snr/20); s_factor = 0.5;
  }
  // For every pair of frames
  for (size_t i=0; i<n; i+=2) {
    float a, b; gaussRNG(a,b);
    in[i]   = s_factor*in[i] + n_factor*a;
    in[i+1] = s_factor*in[i+1] + n_factor*b;
  }
  // If N-frames is odd -> handle last frame
  if (n%2) {
    float a, b; gaussRNG(a,b);
    in[n-1] = s_factor*in[n-1] + n_factor*a;
  }
  return _sink->write(buffer);
}

qint64
NoiseEffect::readData(char *data, qint64 maxlen) {
  return 0;
}

void
NoiseEffect::setEnabled(bool enabled) {
  _enabled = enabled;
}

void
NoiseEffect::setSNR(float snr) {
  _snr = snr;
}



/* ******************************************************************************************** *
 * Fading effect
 * ******************************************************************************************** */
FadingEffect::FadingEffect(QIODevice *sink, bool enabled, float maxDamp, float rate, QObject *parent)
  : QIODevice(parent), _sink(sink), _enabled(enabled), _maxDamp(0), _rate(rate),
    _dS(0), _dF(0), _factor(1)
{
  // init RNG
  srand(time(0));

  // Compute max damping factor
  _maxDamp = std::min(1., std::pow(10, maxDamp/10));
  // Sample first change-point
  _dS = -Globals::sampleRate*60*std::log(double(rand())/RAND_MAX)/_rate;
  // compute slope
  _dF = -(1-_maxDamp)/_dS;

  connect(_sink, SIGNAL(bytesWritten(qint64)), this, SIGNAL(bytesWritten(qint64)));
  open(QIODevice::WriteOnly);
}

FadingEffect::~FadingEffect() {
  // pass...
}

qint64
FadingEffect::writeData(const char *data, qint64 len)
{
  // Skip if disabled
  if (! _enabled)
    return _sink->write(data, len);

  // Copy input buffer
  QByteArray buffer(data, len);
  int16_t *values = reinterpret_cast<int16_t *>(buffer.data());
  // Do!
  for (int i=0; i<(buffer.size()/2); i++) {
    if (0 >= _dS) {
      // Sample next change-point
      _dS = -Globals::sampleRate*60*std::log(double(rand())/RAND_MAX)/_rate;
      // compute slope:
      if (_dF < 0) { _dF = (1-_maxDamp)/_dS; }
      else { _dF = -(1-_maxDamp)/_dS; }
    }
    // Scale value
    values[i] *= _factor; _factor += _dF; _dS--;
  }
  return _sink->write(buffer);
}

qint64
FadingEffect::readData(char *data, qint64 maxlen) {
  return 0;
}

void
FadingEffect::setEnabled(bool enabled) {
  _enabled = enabled;
}

void
FadingEffect::setMaxDamp(float damp) {
  _maxDamp = std::min(1., std::pow(10, damp/10));
}

void
FadingEffect::setFadingRate(float rate) {
  _rate = rate;
}
