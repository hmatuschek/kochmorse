#include "morseencoder.hh"
#include "globals.hh"
#include <cmath>




MorseEncoder::MorseEncoder(AudioSink *sink, double ditFreq, double daFreq,
                           double speed, double effSpeed, Sound sound, bool parallel, QObject *parent)
  : QThread(parent), _ditFreq(ditFreq), _daFreq(daFreq), _speed(speed), _effSpeed(effSpeed),
    _sound(sound), _unitLength(0), _effUnitLength(0), _ditSample(), _daSample(), _icPause(),
    _iwPause(), _sink(sink), _parallel(parallel), _running(false), _queueLock(), _queueWait(),
    _queue(), _played(0)
{
  _createSamples();
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
  _ditSample.resize(2*_unitLength*sizeof(int16_t));
  int16_t *ditData = reinterpret_cast<int16_t *>(_ditSample.data());
  for (size_t i=0; i<_unitLength; i++) {
    // gen tone
    ditData[i] = (2<<12)*std::sin((2*M_PI*_ditFreq*i)/Globals::sampleRate);
    // apply window
    if (i <= epsilon) {
      ditData[i] *= double(i+1)/epsilon;
    } if (i >= (_unitLength-epsilon)) {
      ditData[i] *= double(_unitLength-i)/epsilon;
    }
  }
  // append 1 "dit" pause
  for (size_t i=_unitLength; i<(2*_unitLength); i++) { ditData[i] = 0; }

  // Create da sample
  _daSample.resize(4*_unitLength*sizeof(int16_t));
  int16_t *daData = reinterpret_cast<int16_t *>(_daSample.data());
  for (size_t i=0; i<(3*_unitLength); i++) {
    // gen tone
    daData[i] = (2<<12)*std::sin((2*M_PI*_daFreq*i)/Globals::sampleRate);
    // apply window
    if (i <= epsilon) {
      daData[i] *= double(i)/epsilon;
    } if (i >= (3*_unitLength-epsilon)) {
      daData[i] *= double(3*_unitLength-i)/epsilon;
    }
  }
  // append 1 "dit" pause
  for (size_t i=(3*_unitLength); i<(4*_unitLength); i++) { daData[i] = 0; }

  // Compute inter-char pause (3 x effUnit, not incl. pause after symbol (dit or da))
  size_t icPauseLength = 3*_effUnitLength-_unitLength;
  _icPause.resize(icPauseLength*sizeof(int16_t));
  int16_t *icPauseData = reinterpret_cast<int16_t *>(_icPause.data());
  for (size_t i=0; i<icPauseLength; i++) { icPauseData[i] = 0; }

  // Compute inter-word pause (7 x effUnit, not incl. pause after last char)
  size_t iwPauseLength = 7*_effUnitLength - icPauseLength;
  _iwPause.resize(iwPauseLength*sizeof(int16_t));
  int16_t *iwPauseData = reinterpret_cast<int16_t *>(_iwPause.data());
  for (size_t i=0; i<iwPauseLength; i++) { iwPauseData[i] = 0; }
}


void
MorseEncoder::send(const QString &text) {
  for (QString::const_iterator c = text.begin(); c != text.end(); c++) {
    this->send(*c);
  }
}


void
MorseEncoder::send(QChar ch) {
  if (_parallel) {
    // If parallel put char in queue
    _queueLock.lock();
    _queue.append(ch);
    _queueWait.wakeOne();
    _queueLock.unlock();
  } else {
    // otherwise encode & play directly
    this->_send(ch);
  }
}


void
MorseEncoder::start() {
  if (! _parallel) { return; }
  if (_running) { return; }
  _running = true;
  // Check if the thread is still running
  if (isRunning()) { return; }
  // Reset time lapsed
  resetTime();
  // Start thread
  QThread::start();
}


void
MorseEncoder::stop()
{
  // If not parallel -> skip.
  if (! _parallel) { return; }
  // Otherwise signal thread to exit...
  _running = false;
  // ... clear queue ...
  _queueLock.lock();
  _queue.clear();
  _queueLock.unlock();
  // ... signal all threads ...
  _queueWait.wakeAll();
  // ... and join threads
  this->wait();
}


double
MorseEncoder::time() const {
  return _played;
}

void
MorseEncoder::resetTime() {
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
MorseEncoder::run() {
  while (_running) {
    _queueLock.lock();
    // If queue is empty, wait for a char
    if (0 == _queue.size()) { _queueWait.wait(&_queueLock); }
    if (0 == _queue.size()) { _queueLock.unlock(); continue; }
    // Take a char from the queue
    QChar ch = _queue.first(); _queue.pop_front();
    _queueLock.unlock();
    // If queue is empty now, signal empty queue.
    if (0 == _queue.size()) { emit charsSend(); }
    // Encode & play char
    this->_send(ch);
    // Signal char send
    emit charSend(ch);
  }
}


void
MorseEncoder::_send(QChar ch)
{
  // ensure lower-case letter
  ch = ch.toLower();
  // If space -> send inter-word pause
  if ((' ' == ch) || ('\n') == ch){
    // "play" inter-word pause
    _sink->play(_iwPause);
    // Update time
    _played += double(1000*_iwPause.size())/(2*Globals::sampleRate);
    return;
  }
  // If not in code-table skip
  if (! Globals::charTable.contains(ch)) { return; }
  // Get code
  QString code = Globals::charTable[ch];
  // Send code
  for (QString::iterator sym=code.begin(); sym!=code.end(); sym++) {
    if ('.' == *sym) {
      // Play "dit"
      _sink->play(_ditSample);
      // Update time
      _played += double(1000*_ditSample.size())/(2*Globals::sampleRate);
    } else {
      // Play "da"
      _sink->play(_daSample);
      // update time
      _played += double(1000*_daSample.size())/(2*Globals::sampleRate);
    }
  }
  // Send inter char pause:
  _sink->play(_icPause);
  // Update time elapsed
  _played += double(1000*_icPause.size())/(2*Globals::sampleRate);
}
