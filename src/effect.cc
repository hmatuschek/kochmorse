#include "effect.hh"
#include <time.h>
#include <cmath>
#include "globals.hh"
#include <QDebug>


/* ******************************************************************************************** *
 * Noise effect
 * ******************************************************************************************** */
NoiseEffect::NoiseEffect(QIODevice *source, bool enabled, float snr, QObject *parent)
  : QIODevice(parent), _source(source), _enabled(enabled), _snr(snr)
{
  // init RNG
  srand(time(0));
  if (_source) {
    connect(_source, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
    _source->open(QIODevice::ReadOnly);
  }

  setSNR(_snr);
}

NoiseEffect::~NoiseEffect() {
  // pass...
}

void
NoiseEffect::setSource(QIODevice *src) {
  if (_source)
    disconnect(_source, SIGNAL(readyRead()), this, SIGNAL(readyRead()));

  _source = src;
  if (! _source)
    return;

  connect(_source, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
  _source->open(QIODevice::ReadOnly);
}

qint64
NoiseEffect::bytesAvailable() const {
  if (_source)
    return _source->bytesAvailable() + QIODevice::bytesAvailable();
  return QIODevice::bytesAvailable();
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
NoiseEffect::readData(char *data, qint64 len) {
  if (! _source)
    return 0;

  // If disabled -> skip
  if (! _enabled)
    return _source->read(data, len);

  // Number of frames
  size_t n = (len/2);

  // Copy input buffer
  n = _source->read(data, 2*n)/2;
  int16_t *in = reinterpret_cast<int16_t *>(data);
  // For every pair of frames
  for (size_t i=0; i<n; i+=2) {
    float a, b; gaussRNG(a,b);
    in[i]   = _sfac*in[i] + _nfac*a;
    in[i+1] = _sfac*in[i+1] + _nfac*b;
  }
  // If N-frames is odd -> handle last frame
  if (n%2) {
    float a, b; gaussRNG(a,b);
    in[n-1] = _sfac*in[n-1] + _nfac*a;
  }
  return 2*n;
}

qint64
NoiseEffect::writeData(const char *data, qint64 maxlen) {
  return 0;
}

void
NoiseEffect::setEnabled(bool enabled) {
  _enabled = enabled;
}

void
NoiseEffect::setSNR(float snr) {
  _snr = snr;
  // Get noise and signal scale-factor.
  _nfac = 0; _sfac = 1;
  if (0 > _snr) {
    // If SNR < 0, keep noise at level "1" == 2**12, but reduce signal
    _nfac = ((1<<14)-1); _sfac = std::pow(10, _snr/20-6);
  } else {
    // If SNR > 0, keep signal at level "1" and scale noise down.
    _nfac = ((1<<14)-1)*std::pow(10, -_snr/20); _sfac = 0.5;
  }
}



/* ******************************************************************************************** *
 * Fading effect
 * ******************************************************************************************** */
FadingEffect::FadingEffect(QIODevice *source, bool enabled, float maxDamp, float rate, QObject *parent)
  : QIODevice(parent), _source(source), _enabled(enabled), _maxDamp(0), _rate(rate),
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

  if (_source) {
    connect(_source, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
    _source->open(QIODevice::ReadOnly);
  }
}

FadingEffect::~FadingEffect() {
  // pass...
}

qint64
FadingEffect::readData(char *data, qint64 len)
{
  if (! _source)
    return 0;

  len = _source->read(data, len);

  // Skip if disabled
  if (! _enabled)
    return len;

  // Copy input buffer
  int16_t *values = reinterpret_cast<int16_t *>(data);
  // Do!
  for (int i=0; i<(len/2); i++) {
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
  return len;
}

qint64
FadingEffect::writeData(const char *data, qint64 maxlen) {
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

void
FadingEffect::setSource(QIODevice *src) {
  if (_source)
    disconnect(_source, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
  _source = src;
  if (! _source)
    return;
  connect(_source, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
  _source->open(QIODevice::ReadOnly);
}

qint64
FadingEffect::bytesAvailable() const {
  if (_source)
    return _source->bytesAvailable() + QIODevice::bytesAvailable();
  return QIODevice::bytesAvailable();
}
