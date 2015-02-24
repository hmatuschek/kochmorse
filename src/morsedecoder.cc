#include "morsedecoder.hh"
#include <cmath>
#include <iostream>
#include "globals.hh"


MorseDecoder::MorseDecoder(double speed, QObject *parent)
  : AudioSink(parent), _speed(speed), _unitLength(0),
    _Nsamples(0), _buffer(0), _threshold(1e-5), _delaySize(0),
    _lastChar('\0')
{
  _updateConfig();
}

MorseDecoder::~MorseDecoder() {
  delete _buffer;
}

void
MorseDecoder::setSpeed(double wpm) {
  _speed = wpm; _updateConfig();
}

void
MorseDecoder::setThreshold(double threshold) {
  _threshold = threshold;
}

void
MorseDecoder::_updateConfig() {
  // Free "old" tables and buffers.
  if (0 != _buffer) { delete _buffer; }

  // Compute dit length in samples from speed and sample rate
  size_t ditLength = size_t((60.*Globals::sampleRate)/(50.*_speed));
  _unitLength = std::max(size_t(1), ditLength/2);
  _delaySize = 0;
  _Nsamples = 0;
  _pauseCount = 0;
  _lastChar = '\0';

  // Allocate buffers and tables
  _buffer   = new int16_t[_unitLength];
  // Init tables and buffers
  for (size_t i=0; i<_unitLength; i++) { _buffer[i] = 0; }
  for (size_t i=0; i<8; i++) { _delayLine[i] = 0; }
}


void
MorseDecoder::process(const QByteArray &data) {
  // Extract frames from raw data
  size_t Nframes = data.size()/2;
  const int16_t *data_ptr = reinterpret_cast<const int16_t *>(data.data());
  // Process each frame
  for (size_t i=0; i<Nframes; i++) {
    // Appen frame to buffer
    _buffer[_Nsamples] = data_ptr[i]; _Nsamples++;
    // if a unit length (1/2 dot) has been read
    if (_unitLength == _Nsamples) {
      _Nsamples = 0;
      // Compute 2-norm
      float nrm2 = 0;
      for (size_t j=0; j<_unitLength; j++) {
        float x = float(_buffer[j])/(2<<15);
        nrm2 += x*x;
      }
      nrm2 /= _unitLength;
      // Push symbol to delay line
      _delayLine[_delaySize] = (nrm2 > _threshold);
      _delaySize++;

      if (_isDash()) {
        _delaySize = 0; _processSymbol('-');
      } else if (_isDot()) {
        _delaySize = 0; _processSymbol('.');
      } else if (_isPause()) {
        _delaySize = 0; _processSymbol(' ');
      }

      // Implement ring-buffer behavior
      if (8 == _delaySize) {
        memmove(_delayLine, _delayLine+1, 7*sizeof(int16_t));
        _delaySize--;
      }
    }
  }
}


bool
MorseDecoder::_isDot() const {
  if (3 <= _delaySize) { return false; }
  size_t n = _delaySize-1;
  // Check if [..., 1, 1, 0] at [..., n-2, n-1, n]
  return (1 == _delayLine[n-2]) && (1 == _delayLine[n-1]) && (0 == _delayLine[n]);
}

bool
MorseDecoder::_isDash() const {
  if (7 <= _delaySize) { return false; }
  size_t n = _delaySize-1;
  // Check if [..., 1, 1, 1, 1, 1, 1, 0] at [..., n-6, n-5, ..., n-1, n]
  return (1 == _delayLine[n-6]) && (1 == _delayLine[n-5]) && (1 == _delayLine[n-4])
      && (1 == _delayLine[n-3]) && (1 == _delayLine[n-2]) && (1 == _delayLine[n-1])
      && (0 == _delayLine[n]);
}

bool
MorseDecoder::_isPause() const {
  if (4 <= _delaySize) { return false; }
  size_t n = _delaySize-1;
  return (0 == _delayLine[n-3]) && (0 == _delayLine[n-2]) && (0 == _delayLine[n-1])
      && (0 == _delayLine[n]);
}

void
MorseDecoder::_processSymbol(char sym) {
  if (' ' == sym) {
    if (0 == _symbols.size()) {
      _pauseCount++;
      if (2 == _pauseCount) {
        _pauseCount = 0;
        if (' ' != _lastChar) {
          emit charReceived(' ');
          _lastChar = ' ';
        }
      }
    } else {
      // try to decode symbol...
      if (! Globals::codeTable.contains(_symbols)) {
        emit unknownCharReceived(_symbols);
        _lastChar = '\0';
      } else {
        emit charReceived(Globals::codeTable[_symbols]);
        _lastChar = Globals::codeTable[_symbols];
      }
      _symbols.clear();
      _pauseCount = 0;
    }
  } else {
    _symbols.append(sym);
    _pauseCount = 0;
  }
}
