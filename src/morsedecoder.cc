#include "morsedecoder.hh"
#include <cmath>
#include <iostream>
#include "globals.hh"


MorseDecoder::MorseDecoder(double speed, double freq, QObject *parent)
  : AudioSink(parent), _speed(speed), _freq(freq), _unitLength(0),
    _Nsamples(0), _buffer(0), _sinTable(0), _cosTable(0), _threshold(1e-5), _delaySize(0)
{
  _updateConfig();
}

MorseDecoder::~MorseDecoder() {
  delete _buffer;
  delete _sinTable;
  delete _cosTable;
}

void
MorseDecoder::setFreq(double freq) {
  _freq = freq; _updateConfig();
}

void
MorseDecoder::setSpeed(double wpm) {
  _speed = wpm; _updateConfig();
}

void
MorseDecoder::_updateConfig() {
  // Free "old" tables and buffers.
  if (0 != _buffer) { delete _buffer; }
  if (0 != _sinTable) { delete _sinTable; }
  if (0 != _cosTable) { delete _cosTable; }

  // Compute dit length in samples from speed and sample rate
  size_t ditLength = size_t((60.*Globals::sampleRate)/(50.*_speed));
  _unitLength = std::max(size_t(1), ditLength/2);
  _delaySize = 0;
  _Nsamples = 0;

  // Allocate buffers and tables
  _buffer   = new int16_t[_unitLength];
  _sinTable = new float[_unitLength];
  _cosTable = new float[_unitLength];

  // Init tables and buffers
  for (size_t i=0; i<_unitLength; i++) {
    _buffer[i] = 0;
    _sinTable[i] = std::sin((2*M_PI*_freq*i)/Globals::sampleRate);
    _cosTable[i] = std::cos((2*M_PI*_freq*i)/Globals::sampleRate);
  }
  for (size_t i=0; i<8; i++) {
    _delayLine[i] = 0;
  }
  _pauseCount = 0;
}


void
MorseDecoder::play(const QByteArray &data) {
  size_t Nframes = data.size()/2;
  const int16_t *data_ptr = reinterpret_cast<const int16_t *>(data.data());
  for (size_t i=0; i<Nframes; i++) {
    _buffer[_Nsamples] = data_ptr[i]; _Nsamples++;
    if (_unitLength == _Nsamples) {
      _Nsamples = 0;
      float sin_sum = 0, cos_sum = 0;
      for (size_t j=0; j<_unitLength; j++) {
        float x = float(_buffer[j])/(2<<15);
        sin_sum += _sinTable[j]*x;
        cos_sum += _cosTable[j]*x;
      }
      sin_sum /= _unitLength; cos_sum /= _unitLength;
      float pwr = sin_sum*sin_sum + cos_sum*cos_sum;
      _delayLine[_delaySize] = (pwr > _threshold);
      _delaySize++;
    }
    if (_isDot()) {
      _delaySize = 0; _processSymbol('.');
    } else if (_isDash()) {
      _delaySize = 0; _processSymbol('-');
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


bool
MorseDecoder::_isDot() const {
  return (4 == _delaySize) && (1 == _delayLine[0]) && (1 == _delayLine[1]) && (0 == _delayLine[2])
      && (0 == _delayLine[3]);
}

bool
MorseDecoder::_isDash() const {
  return (8 == _delaySize) && (1 == _delayLine[0]) && (1 == _delayLine[1]) && (1 == _delayLine[2])
      && (1 == _delayLine[3]) && (1 == _delayLine[4]) && (1 == _delayLine[5])
      && (0 == _delayLine[6]) && (0 == _delayLine[7]);
}

bool
MorseDecoder::_isPause() const {
  return (4 <= _delaySize) && (0 == _delayLine[0]) && (0 == _delayLine[1]) && (0 == _delayLine[2])
      && (0 == _delayLine[3]);
}

void
MorseDecoder::_processSymbol(char sym) {
  if (' ' == sym) {
    if (0 == _symbols.size()) {
      _pauseCount++;
      if (2 == _pauseCount) {
        _pauseCount = 0;
        std::cerr << "Received & decoded pause." << std::endl;
        emit charReceived(' ');
      }
    } else {
      // try to decode symbol...
      if (! Globals::codeTable.contains(_symbols)) {
        std::cerr << "Drop unknown symbol " << _symbols.toStdString() << std::endl;
      } else {
        std::cerr << "Received & decoded symbol " <<
                     QString(Globals::codeTable[_symbols]).toStdString() << std::endl;
        emit charReceived(Globals::codeTable[_symbols]);
      }
      _symbols.clear();
      _pauseCount = 0;
    }
  } else {
    _symbols.append(sym);
    _pauseCount = 0;
  }
}
