#include "effect.hh"
#include <cmath>


NoiseEffect::NoiseEffect(AudioSink *sink, bool enabled, double snr, QObject *parent)
  : AudioSink(parent), _sink(sink), _enabled(enabled), _snr(snr)
{

}

NoiseEffect::~NoiseEffect() {
  // pass...
}

void
NoiseEffect::gaussRNG(double &a, double &b) const {
  double x = double(rand())/RAND_MAX;
  double y = double(rand())/RAND_MAX;
  a = std::sqrt(-2*std::log(x))*std::cos(2*M_PI*y);
  b = std::sqrt(-2*std::log(x))*std::sin(2*M_PI*y);
}

void
NoiseEffect::play(const QByteArray &data) {
  // If disabled -> skip
  if (! _enabled) { _sink->play(data); return; }
  size_t n = data.size()/2;
  QByteArray buffer(data);
  int16_t *in = reinterpret_cast<int16_t *>(buffer.data());
  for (size_t i=0; i<n/2; i++) {
    double a, b; gaussRNG(a,b);
    in[i]   += (2<<12)*std::pow(10, -_snr/10)*a;
    in[i+1] += (2<<12)*std::pow(10, -_snr/10)*b;
  }
  if (n%2) {
    double a, b; gaussRNG(a,b);
    in[n-1]   += (2<<12)*std::pow(10, -_snr/10)*a;
  }
  _sink->play(buffer);
}

double
NoiseEffect::rate() const {
  return _sink->rate();
}
