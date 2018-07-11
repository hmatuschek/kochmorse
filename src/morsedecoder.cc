#include "morsedecoder.hh"
#include <cmath>
#include <iostream>
#include "globals.hh"


MorseDecoder::MorseDecoder(double speed, float threshold, QObject *parent)
  : QObject(parent), AudioSink(), _speed(speed), _ditLength(0), _unitLength(0),
    _Nsamples(0), _buffer(0), _threshold(threshold), _highCount(0), _lowCount(0),
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
  _ditLength = size_t((60.*Globals::sampleRate)/(50.*_speed));
  _unitLength = std::max(size_t(1), _ditLength/4);
  _Nsamples = 0;
  _pauseCount = 0;
  _lastChar = '\0';
  _highCount = 0;
  _lowCount = 0;

  // Allocate buffers and tables
  _buffer   = new int16_t[_unitLength];
  // Init tables and buffers
  for (size_t i=0; i<_unitLength; i++)
    _buffer[i] = 0;
}


void
MorseDecoder::process(const QByteArray &data) {
  // Extract frames from raw data
  size_t Nframes = data.size()/2;
  const int16_t *data_ptr = reinterpret_cast<const int16_t *>(data.data());
  // Process each frame
  for (size_t i=0; i<Nframes; i++) {
    // Append frame to buffer
    _buffer[_Nsamples] = data_ptr[i]; _Nsamples++;
    // if a unit length (1/4 dit) has been read
    if (_unitLength == _Nsamples) {
      _Nsamples = 0;
      // Compute 2-norm
      float nrm2 = 0;
      for (size_t j=0; j<_unitLength; j++) {
        float x = float(_buffer[j])/(1<<15);
        nrm2 += x*x;
      }
      nrm2 /= _unitLength;

      // Handle signal detection
      if (nrm2 > _threshold) {
        _highCount++;
        // If transition 0 -> 1
        if (_lowCount)
          _lowCount = 0;
      } else {
        _lowCount++;
        // If transition 1 -> 0
        if (_highCount) {
          // Check of a symbol was received
          if (_isDash()) {
            _highCount = 0; _processSymbol('-');
          } else if (_isDot()) {
            _highCount = 0; _processSymbol('.');
          } else {
            _highCount = 0;
          }
        } else {
          if (_isPause()) {
            _processSymbol(' ');
            _lowCount = 0;
          }
        }
      }
    }
  }
}


bool
MorseDecoder::_isDot() const {
  return (1 <= _highCount) && (8 > _highCount);
}

bool
MorseDecoder::_isDash() const {
  return (8 <= _highCount) && (24 >= _highCount);
}

bool MorseDecoder::_isInvalid() const {
  return 24 < _highCount;
}

bool
MorseDecoder::_isPause() const {
  return _lowCount >= 12;
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
