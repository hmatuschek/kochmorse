#include "effect.hh"
#include <cmath>


NoiseEffect::NoiseEffect(AudioSink *sink, bool enabled, float snr, QObject *parent)
  : AudioSink(parent), _sink(sink), _enabled(enabled), _snr(snr)
{

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

void
NoiseEffect::play(const QByteArray &data) {
  // If disabled -> skip
  if (! _enabled) { _sink->play(data); return; }
  // Number of frames
  size_t n = (data.size()/2);

  // Copy input buffer
  QByteArray buffer(data);
  int16_t *in = reinterpret_cast<int16_t *>(buffer.data());
  // Get noise and signal scale-factor.
  float n_factor=0, s_factor=1;
  if (0 > _snr) {
    // If SNR < 0, keep noise at level "1" == 2**12, but reduce signal
    n_factor = (2<<12); s_factor = std::pow(10, _snr/10);
  } else {
    // If SNR > 0, keep signal at level "1" and scale noise down.
    n_factor = (2<<12)*std::pow(10, -_snr/10); s_factor = 1;
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
  _sink->play(buffer);
}

double
NoiseEffect::rate() const {
  return _sink->rate();
}

void
NoiseEffect::setEnabled(bool enabled) {
  _enabled = enabled;
}

void
NoiseEffect::setSNR(float snr) {
  _snr = snr;
}
