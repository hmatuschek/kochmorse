#include "morseencoder.hh"
#include "globals.hh"
#include <cmath>




MorseEncoder::MorseEncoder(QIODevice *sink, double ditFreq, double daFreq,
                           double speed, double effSpeed, Sound sound, Jitter jitter, QObject *parent)
  : QObject(parent), _ditFreq(ditFreq), _daFreq(daFreq), _speed(speed), _effSpeed(effSpeed),
    _sound(sound), _jitter(jitter), _unitLength(0), _effUnitLength(0), _ditSamples(4), _daSamples(4),
    _icPause(), _iwPause(), _sink(sink), _queue(), _tsend(0), _played(0)
{
  _createSamples();
  connect(_sink, SIGNAL(bytesWritten(qint64)), this, SLOT(onBytesWritten(qint64)));
}


void
MorseEncoder::_createSamples()
{
  // ensure effective speed is <= speed
  _effSpeed = std::min(_speed, _effSpeed);

  // Compute unit (dit) length (PARIS std. = 50 units per word) in samples
  _unitLength = size_t((60.*Globals::sampleRate)/(50.*_speed));
  _effUnitLength = size_t((60.*Globals::sampleRate)/(50.*_effSpeed));

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
      slen += _unitLength/2;
    if ((2 == jitter) || (3==jitter))
      plen += _unitLength/2;

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
      slen += 3*_unitLength/2;
    if ((2 == jitter) || (3==jitter))
      plen += _unitLength/2;

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

  // Compute inter-char pause (3 x effUnit, not incl. pause after symbol (dit or da))
  size_t icPauseLength = 3*_effUnitLength-_unitLength;
  _icPause.resize(icPauseLength*sizeof(int16_t));
  int16_t *icPauseData = reinterpret_cast<int16_t *>(_icPause.data());
  for (size_t i=0; i<icPauseLength; i++)
    icPauseData[i] = 0;

  // Compute inter-word pause (7 x effUnit, not incl. pause after last char)
  size_t iwPauseLength = 7*_effUnitLength - icPauseLength;
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
  if (empty && (! _queue.isEmpty())) {
    _send(_queue.front());
    _queue.pop_front();
  }
}


void
MorseEncoder::send(QChar ch) {
  if ((! Globals::charTable.contains(ch)) && (' ' != ch) && ('\n' != ch))
    return;
  if (_queue.isEmpty())
    _send(ch);
  else
    _queue.push_back(ch);
}


void
MorseEncoder::start() {
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
MorseEncoder::setEffSpeed(int speed) {
  _effSpeed = speed; _createSamples();
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

void
MorseEncoder::_send(QChar ch)
{
  // ensure lower-case letter
  ch = ch.toLower();

  // If space -> send inter-word pause
  if ((' ' == ch) || ('\n') == ch){
    // "play" inter-word pause
    _sink->write(_iwPause);
    // Update time
    _tsend += double(1000*_iwPause.size())/(2*Globals::sampleRate);
    emit charSend(ch);
    return;
  }

  // Get code
  QString code = Globals::charTable[ch];

  int ditIdx = 0, daIdx=0;
  switch (_jitter) {
    case JITTER_BUG:
      daIdx = rand() % 4;
      break;
    case JITTER_STRAIGT:
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
      _sink->write(_ditSamples[ditIdx]);
      // Update time
      _tsend += double(1000*_ditSamples[ditIdx].size())/(2*Globals::sampleRate);
    } else {
      // Play "da"
      _sink->write(_daSamples[daIdx]);
      // update time
      _tsend += double(1000*_daSamples[daIdx].size())/(2*Globals::sampleRate);
    }
  }
  // Send inter char pause:
  _sink->write(_icPause);
  // Update time elapsed
  _tsend += double(1000*_icPause.size())/(2*Globals::sampleRate);
  emit charSend(ch);
}

void
MorseEncoder::onBytesWritten(qint64 n) {
  double dt = double(1000*n)/(2*Globals::sampleRate);
  _played += dt;
  if ((_tsend-_played) < 100) {
    if (_queue.isEmpty())
      emit charsSend();
    else {
      _send(_queue.front()); _queue.pop_front();
    }
  }
}
