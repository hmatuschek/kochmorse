#include "morseencoder.hh"
#include <cmath>
#include <iostream>


inline QHash<QChar, QString> _initCodeTable() {
  QHash<QChar, QString> table;
  table['a'] = ".-";     table['b'] = "-...";   table['c'] = "-.-.";   table['d'] = "-..";
  table['e'] = ".";      table['f'] = "..-.";   table['g'] = "--.";    table['h'] = "....";
  table['i'] = "..";     table['j'] = ".---";   table['k'] = "-.-";    table['l'] = ".-..";
  table['m'] = "--";     table['n'] = "-.";     table['o'] = "---";    table['p'] = ".--.";
  table['q'] = "--.-";   table['r'] = ".-.";    table['s'] = "...";    table['t'] = "-";
  table['u'] = "..-";    table['v'] = "...-";   table['w'] = ".--";    table['x'] = "-..-";
  table['y'] = "-.--";   table['z'] = "--..";   table['0'] = "-----";  table['1'] = ".----";
  table['2'] = "..---";  table['3'] = "...--";  table['4'] = "....-";  table['5'] = ".....";
  table['6'] = "-....";  table['7'] = "--...";  table['8'] = "---..";  table['5'] = "----.";
  table['.'] = ".-.-.-"; table[','] = "--..--"; table['?'] = "..--.."; table['/'] = "-..-.";
  table['&'] = ".-...";  table[':'] = "---..."; table[';'] = "-.-.-."; table['='] = "-...-";
  table['+'] = ".-.-.";  table['-'] = "-....-"; table['@'] = ".--.-.";
  return table;
}

QHash<QChar, QString> MorseEncoder::_codeTable = _initCodeTable();


MorseEncoder::MorseEncoder(AudioSink *sink, double ditFreq, double daFreq,
                           double speed, double effSpeed, QObject *parent)
  : QObject(parent), _ditFreq(ditFreq), _daFreq(daFreq), _speed(speed), _effSpeed(effSpeed),
    _unitLength(0), _effUnitLength(0), _sink(sink)
{
  // ensure effective speed is <= speed
  _effSpeed = std::min(_speed, _effSpeed);
  // Get sample rate from audio device
  double rate = _sink->rate();

  // Compute unit (dit) length (PARIS std. eq 50 dits per word)
  _unitLength = size_t((rate*50*_speed)/60);
  _effUnitLength = size_t((rate*50*_effSpeed)/60);

  // Create dit sample
  _ditSample.resize(2*_unitLength*sizeof(int16_t));
  int16_t *ditData = reinterpret_cast<int16_t *>(_ditSample.data());
  for (size_t i=0; i<_unitLength; i++) {
    // gen tone
    ditData[i] = (2<<14)*std::sin(2*M_PI*_ditFreq*double(i)/rate);
    // apply Welch window
    double mid = (double(_unitLength-1)/2);
    ditData[i] *= (1.0 - std::pow((i-mid)/mid, 2.0));
  }
  // append 1 "dit" pause
  for (size_t i=_unitLength; i<2*_unitLength; i++) { ditData[i] = 0; }

  // Create da sample
  _daSample.resize(4*_unitLength*sizeof(int16_t));
  int16_t *daData = reinterpret_cast<int16_t *>(_daSample.data());
  for (size_t i=0; i<3*_unitLength; i++) {
    // gen tone
    daData[i] = std::sin(2*M_PI*_daFreq*double(i)/rate);
    // apply Welch window
    double mid = (double(3*_unitLength-1)/2);
    daData[i] *= (1.0 - std::pow((i-mid)/mid, 2.0));
  }
  // append 1 "dit" pause
  for (size_t i=3*_unitLength; i<4*_unitLength; i++) { daData[i] = 0; }

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
  std::cerr << "Send '" << ch.toLatin1() << "'..." << std::endl;

  // ensure lower-case letter
  ch = ch.toLower();
  // If space -> send inter-word pause
  if (' ' == ch) { _sink->play(_iwPause); return; }
  // If not in code-table skip
  if (! _codeTable.contains(ch)) { return; }
  // Get code
  QString code = _codeTable[ch];
  // Send code
  for (QString::iterator sym=code.begin(); sym!=code.end(); sym++) {
    if ('.' == *sym) { _sink->play(_ditSample); }
    else if ('_' == *sym) { _sink->play(_daSample); }
  }
  // Send inter char pause:
  _sink->play(_icPause);
}
