#include "morseencoder.hh"
#include "globals.hh"
#include <cmath>
#include <QDebug>
#include <QByteArray>


MorseEncoder::MorseEncoder(double ditFreq, double daFreq, double speed, double icpFac, double iwpFac,
                           Sound sound, Jitter jitter, QObject *parent)
  : QIODevice(parent), _ditFreq(ditFreq), _daFreq(daFreq), _speed(speed), _icpfac(icpFac), _iwpfac(iwpFac),
    _sound(sound), _jitter(jitter), _unitLength(0), _ditSamples(4), _daSamples(4),
    _icPause(), _iwPause(), _current(0), _queue(), _tsend(0), _played(0)
{
  _createSamples();
}


void
MorseEncoder::_createSamples()
{
  // ensure effective pause factors are >= 1
  _icpfac = std::max(1.0, _icpfac);
  _iwpfac = std::max(1.0, _iwpfac);

  // Compute unit (dit) length (PARIS std. = 50 units per word) in samples
  // including 10 dits inter-character pauses (excl. 1 dit pause after last symbol) and
  // and 5 dits inter-word pause (excl. 1+2 dit pause after last char).
  _unitLength = size_t((60.*Globals::sampleRate)/(50.*_speed));

  // The first and last epsilon samples are windowed
  size_t epsilon = 0;
  switch (_sound) {
  case SOUND_SOFT:
    epsilon = (10*Globals::sampleRate)/_ditFreq;
    break;
  case SOUND_SHARP:
    epsilon = (5*Globals::sampleRate)/_ditFreq;
    break;
  case SOUND_CRACKING:
    epsilon = (1*Globals::sampleRate)/_ditFreq;;
    break;
  }

  // Create dit sample
  for (int jitter=0; jitter<4; jitter++) {
    size_t slen = _unitLength, plen = _unitLength;
    if ((1 == jitter) || (3==jitter))
      slen += _unitLength/4;
    if ((2 == jitter) || (3==jitter))
      plen += _unitLength/4;

    _ditSamples[jitter].resize((slen+plen)*sizeof(int16_t));
    int16_t *ditData = reinterpret_cast<int16_t *>(_ditSamples[jitter].data());

    for (size_t i=0; i<slen; i++) {
      // gen tone
      ditData[i] = ((1<<15)-1)*std::sin((2*M_PI*_ditFreq*i)/Globals::sampleRate);
      // apply window
      if (i <= epsilon)
        ditData[i] *= double(i+1)/epsilon;
      if (i >= (slen-epsilon))
        ditData[i] *= double(slen-i)/epsilon;
    }

    // append 1 "dit" pause
    for (size_t i=slen; i<(slen+plen); i++)
      ditData[i] = 0;
  }

  // Create da sample
  for (int jitter=0; jitter<4; jitter++) {
    size_t slen = 3*_unitLength, plen = _unitLength;
    if ((1 == jitter) || (3==jitter))
      slen += _unitLength;
    if ((2 == jitter) || (3==jitter))
      plen += _unitLength/4;

    _daSamples[jitter].resize((slen+plen)*sizeof(int16_t));
    int16_t *daData = reinterpret_cast<int16_t *>(_daSamples[jitter].data());

    for (size_t i=0; i<slen; i++) {
      // gen tone
      daData[i] = ((1<<15)-1)*std::sin((2*M_PI*_daFreq*i)/Globals::sampleRate);
      // apply window
      if (i <= epsilon) {
        daData[i] *= double(i)/epsilon;
      } if (i >= (slen-epsilon)) {
        daData[i] *= double(slen-i)/epsilon;
      }
    }

    // append 1 "dit" pause
    for (size_t i=slen; i<(slen+plen); i++)
      daData[i] = 0;
  }

  // Compute inter-char pause (3 x dit, not incl. pause after symbol (dit or da))
  size_t icPauseLength = (3*_icpfac*_unitLength)-_unitLength;
  _icPause.resize(icPauseLength*sizeof(int16_t));
  int16_t *icPauseData = reinterpret_cast<int16_t *>(_icPause.data());
  for (size_t i=0; i<icPauseLength; i++)
    icPauseData[i] = 0;

  // Compute inter-word pause (7 x effUnit, not incl. pause after last char)
  size_t iwPauseLength = (7*_iwpfac*_unitLength) - 2*_unitLength;
  _iwPause.resize(iwPauseLength*sizeof(int16_t));
  int16_t *iwPauseData = reinterpret_cast<int16_t *>(_iwPause.data());
  for (size_t i=0; i<iwPauseLength; i++)
    iwPauseData[i] = 0;
}


void
MorseEncoder::send(const QString &text) {
  bool empty = _queue.isEmpty();
  for (QString::const_iterator c = text.begin(); c != text.end(); c++) {
    if ( (! Globals::charTable.contains(*c)) && (' ' != *c) && ('\n' != *c) )
      continue;
    _queue.append(*c);
  }
  if (empty)
    _send();
}


void
MorseEncoder::send(QChar ch) {
  if ((! Globals::charTable.contains(ch)) && (' ' != ch) && ('\n' != ch) && ('\t'!=ch))
    return;
  bool empty = _queue.isEmpty();
  _queue.append(ch);
  if (empty)
    _send();
}


void
MorseEncoder::start() {
  _queue.clear();
  resetTime();
}


void
MorseEncoder::stop() {
  _queue.clear();
}


double
MorseEncoder::time() const {
  return _played;
}

void
MorseEncoder::resetTime() {
  _tsend = 0;
  _played = 0;
}


void
MorseEncoder::setSpeed(int speed) {
  _speed = speed; _createSamples();
}

void
MorseEncoder::setICPFactor(double factor) {
  _icpfac = factor; _createSamples();
}
void
MorseEncoder::setIWPFactor(double factor) {
  _iwpfac = factor; _createSamples();
}


void
MorseEncoder::setDotTone(double freq) {
  _ditFreq = freq; _daFreq=freq;
  _createSamples();
}

void
MorseEncoder::setDashTone(double freq) {
  _daFreq = freq; _createSamples();
}

void
MorseEncoder::setSound(Sound sound) {
  _sound = sound; _createSamples();
}

void
MorseEncoder::setJitter(Jitter jitter) {
  _jitter = jitter;
}

qint64
MorseEncoder::bytesAvailable() const {
  return _buffer.size() + QIODevice::bytesAvailable();
}

void
MorseEncoder::_send()
{
  if (_queue.isEmpty()) {
    emit charsSend();
    return;
  }

  // ensure lower-case letter
  _current = _queue.front().toLower(); _queue.pop_front();

  // If space -> send inter-word pause
  if ((' ' == _current) || ('\n') == _current){
    // "play" inter-word pause
    _buffer.append(_iwPause);
    // Update time
    _tsend += double(1000*_iwPause.size())/(2*Globals::sampleRate);
    return;
  } else if ('\t' == _current) {
    // "play" inter-word pause 3x
    _buffer.append(_iwPause);
    _buffer.append(_iwPause);
    _buffer.append(_iwPause);
    // Update time
    _tsend += double(1000*3*_iwPause.size())/(2*Globals::sampleRate);
    return;
  }

  // Get code
  QString code = Globals::charTable[_current];

  int ditIdx = 0, daIdx=0;
  switch (_jitter) {
    case JITTER_BUG:
      daIdx = rand() % 4;
      break;
    case JITTER_STRAIGHT:
      ditIdx = rand() % 4;
      daIdx = rand() % 4;
      break;
    default:
      break;
  }

  // Send code
  for (QString::iterator sym=code.begin(); sym!=code.end(); sym++) {
    if ('.' == *sym) {
      // Play "dit"
      _buffer.append(_ditSamples[ditIdx]);
      // Update time
      _tsend += double(1000*_ditSamples[ditIdx].size())/(2*Globals::sampleRate);
    } else {
      // Play "da"
      _buffer.append(_daSamples[daIdx]);
      // update time
      _tsend += double(1000*_daSamples[daIdx].size())/(2*Globals::sampleRate);
    }
  }
  // Send inter char pause:
  _buffer.append(_icPause);
  emit readyRead();
  // Update time elapsed
  _tsend += double(1000*_icPause.size())/(2*Globals::sampleRate);
}

qint64
MorseEncoder::readData(char *data, qint64 maxlen) {  
  maxlen = std::min(maxlen, qint64(_buffer.size()));
  if (0 == maxlen)
    return 0;
  memcpy(data, _buffer.constData(), maxlen);
  _buffer.remove(0, maxlen);
  if (0 == _buffer.size())
    emit charSend(_current);
  return maxlen;
}

qint64
MorseEncoder::writeData(const char *data, qint64 len) {
  return 0;
}
