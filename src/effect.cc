#include "effect.hh"
#include <time.h>
#include <cmath>
#include "globals.hh"
#include <QDebug>
#include <iostream>
#include <random>

#define sinc(x) (x==0 ? 1: (std::sin(M_PI*x)/(M_PI*x)))


/* ******************************************************************************************** *
 * Noise effect
 * ******************************************************************************************** */
NoiseEffect::NoiseEffect(QIODevice *source, bool enabled, float snr, bool filter, float Fc, float Bw, QObject *parent)
  : QIODevice(parent), _source(source), _enabled(enabled), _snr(snr), _filter(filter), _Fc(Fc), _Bw(Bw), _bidx(0)
{
  // init RNG
  srand(time(nullptr));
  if (_source) {
    connect(_source, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
    _source->open(QIODevice::ReadOnly);
  }

  setSNR(_snr);
  updateFIR();
}

NoiseEffect::~NoiseEffect() {
  // pass...
}

void
NoiseEffect::updateFIR() {
  float Bl = (_Fc-_Bw/2)/Globals::sampleRate;
  float Bu = (_Fc+_Bw/2)/Globals::sampleRate;
  for (int i=0; i<FIR_ORDER; i++) {
    _buffer[i] = 0;
    _fir[i] = (2*Bu*sinc(2*Bu*(i-FIR_ORDER/2)) - 2*Bl*sinc(2*Bl*(i-FIR_ORDER/2)));
  }
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
  float x = 2*float(rand())/float(RAND_MAX) - 1;
  float y = 2*float(rand())/float(RAND_MAX) - 1;
  float s = x*x + y*y;
  while (s >= 1) {
    x = 2*float(rand())/float(RAND_MAX) - 1;
    y = 2*float(rand())/float(RAND_MAX) - 1;
    s = x*x + y*y;
  }
  a = x*std::sqrt(-2*std::log(s)/s);
  b = y*std::sqrt(-2*std::log(s)/s);
}

float
NoiseEffect::filter(float value) {
  if (! _filter)
    return value;

  _buffer[_bidx++] = value;
  if (_bidx>=FIR_ORDER)
    _bidx=0;
  double res = 0;
  for (int i=0; i<FIR_ORDER; i++)
    res += (_buffer[(_bidx+i)%FIR_ORDER] * _fir[i]);
  return res;
}

qint64
NoiseEffect::readData(char *data, qint64 len) {
  if (! _source)
    return 0;

  // If disabled -> skip
  if (! _enabled)
    return _source->read(data, len);

  // Number of frames
  size_t n = size_t(len/2);

  // Copy input buffer
  n = size_t(_source->read(data, 2*n)/2);
  int16_t *in = reinterpret_cast<int16_t *>(data);
  // For every pair of frames
  for (size_t i=0; i<n; i+=2) {
    float a, b; gaussRNG(a,b);
    in[i] = filter(_sfac*in[i] + _nfac*a);
    in[i+1] = filter(_sfac*in[i+2] + _nfac*a);
  }
  if (n%2) {
    float a, b; gaussRNG(a,b);
    in[n-1] = filter(_sfac*in[n-1] + _nfac*a);
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

bool
NoiseEffect::filterEnabled() const {
  return _filter;
}

void
NoiseEffect::setFiterEnabled(bool enabled) {
  _filter = enabled;
}

float
NoiseEffect::Fc() const {
  return _Fc;
}

void
NoiseEffect::setFc(float Fc) {
  _Fc = Fc;
  updateFIR();
}

float
NoiseEffect::Bw() const {
  return _Bw;
}

void
NoiseEffect::setBw(float Bw) {
  _Bw = Bw;
  updateFIR();
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
