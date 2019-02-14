#include "tutor.hh"
#include <cmath>
#include <QFile>
#include <time.h>
#include <QDebug>


/* ********************************************************************************************* *
 * Tutor interface
 * ********************************************************************************************* */
Tutor::Tutor(MorseEncoder *encoder, QObject *parent)
  : QObject(parent), _encoder(encoder), _running(false)
{
  // pass...
}

Tutor::~Tutor() {
  // pass...
}

QString
Tutor::summary() const {
  return "";
}

void
Tutor::handle(const QChar &ch) {
  Q_UNUSED(ch);
  // pass...
}

void
Tutor::start() {
  _running = true;
}

void
Tutor::stop() {
  _running = false;
}


/* ********************************************************************************************* *
 * KochTrainer
 * ********************************************************************************************* */
inline QVector<QChar> _initKochLessons() {
  QVector<QChar> chars;
  chars << 'k' << 'm' << 'r' << 's' << 'u' << 'a' << 'p' << 't' << 'l' << 'o' << 'w'
        << 'i' << '.' << 'n' << 'j' << 'e' << 'f' << '0' << 'y' << ',' << 'v' << 'g'
        << '5' << '/' << 'q' << '9' << 'z' << 'h' << '3' << '8' << 'b' << '?' << '4'
        << '2' << '7' << 'c' << '1' << 'd' << '6' << 'x' << '=' << '+' << QChar(0x2403) ;
  return chars;
}
// The vector of all chars ordered by lesson
QVector<QChar> KochTutor::_lessons = _initKochLessons();


KochTutor::KochTutor(MorseEncoder *encoder, int lesson, bool prefLastChars,  int lastCharFrequencyFactor,
bool repeatLastChar,
                     size_t minGroupSize, size_t maxGroupSize,
                     int lines, bool showSummary, int successThreshold, QObject *parent)
  : Tutor(encoder, parent), _lesson(lesson), _prefLastChars(prefLastChars), _lastCharFrequencyFactor(lastCharFrequencyFactor),
    _repeatLastChar(repeatLastChar), _minGroupSize(std::min(minGroupSize, maxGroupSize)),
    _maxGroupSize(std::max(minGroupSize, maxGroupSize)), _lines(lines), _linecount(0),
    _showSummary(showSummary), _threshold(successThreshold), _text(),
    _chars_send(0), _words_send(0), _lines_send(0)
{
  // Init random number generator
  srand(time(nullptr));

  connect(_encoder, SIGNAL(charSend(QChar)), this, SLOT(onCharSend(QChar)));
}

KochTutor::~KochTutor() {
  // pass...
}

QChar
KochTutor::next() {
  if (atEnd())
    reset();
  else if (0 == _text.size())
    _nextline();

  QChar ch = _text.first(); _text.pop_front();
  if ('\n' == ch)
    _lines_send++;
  else if (' ' == ch)
    _words_send++;
  else
    _chars_send++;
  return ch;
}

bool
KochTutor::atEnd() {
  return ((0 == _text.size()) && (_lines == _linecount));
}

int
KochTutor::lesson() const {
  return _lesson;
}
void
KochTutor::setLesson(int lesson) {
  _lesson = std::max(2, std::min(lesson, _lessons.size()));
}

bool
KochTutor::prefLastChars() const {
  return _prefLastChars;
}

void
KochTutor::setPrefLastChars(bool pref) {
  _prefLastChars = pref;
}

int
KochTutor::lastCharFrequencyFactor() const {
  return _lastCharFrequencyFactor;
}

void
KochTutor::setLastCharFrequencyFactor(int lastCharFrequencyFactor) {
  _lastCharFrequencyFactor = lastCharFrequencyFactor;
}

bool
KochTutor::repeatLastChar() const {
  return _repeatLastChar;
}

void
KochTutor::setRepeatLastChar(bool enable) {
  _repeatLastChar = enable;
}

int
KochTutor::lines() const {
  return _lines;
}

void
KochTutor::setLines(int lines) {
  _lines = lines;
}

QString
KochTutor::summary() const {
  if (! _showSummary)
    return "";
  int chars_send = int(_chars_send); chars_send -= 3; // - "vvv\n"
  int words_send = int(_words_send);
  int lines_send = int(_lines_send); lines_send -= 1; // - "vvv\n"
  if (_repeatLastChar) {
    chars_send -= 5;
    words_send -= 5;
    lines_send -= 1;
  }
  chars_send -= lines_send; // - " =\n" at end of each line
  int threshold = int(chars_send*(100-_threshold))/100;
  if (_lesson < _lessons.size())
    return tr("\n\nSent %1 chars in %2 words and %3 lines. "
              "If you have less than %4 mistakes, you can proceed to lesson %5.")
        .arg(chars_send).arg(words_send).arg(lines_send).arg(threshold).arg(_lesson+1);
  else
    return tr("\n\nSent %1 chars in %2 words and %3 lines. "
              "If you have less than %4 mistakes, you completed the course!")
        .arg(chars_send).arg(words_send).arg(lines_send).arg(threshold);
}

void
KochTutor::setShowSummary(bool show) {
  _showSummary = show;
}

void
KochTutor::start() {
  this->reset();
  Tutor::start();
  _encoder->start();
  _encoder->send(this->next());
}

void
KochTutor::stop() {
  Tutor::stop();
  _encoder->stop();
}

void
KochTutor::reset()
{
  // Empty current session
  _text.clear();
  // Reset line count
  _linecount = 0;
  // Insert "vvv\n"
  _text.push_back('v'); _text.push_back('v'); _text.push_back('v'); _text.push_back('\n');
  // Insert newest char if "repeat last char" is enabled
  if (_repeatLastChar) {
    for (int i=0; i<5; i++) {
      _text.push_back(_lessons[_lesson-1]); _text.push_back(' ');
    }
    _text.push_back('\n');
  }
  // sample a line of text.
  _nextline();
  _chars_send = 0;
  _words_send = 0;
  _lines_send = 0;
}

bool
KochTutor::needsDecoder() const {
  return false;
}

void KochTutor::onCharSend(QChar c) {
  Q_UNUSED(c);
  if ((! this->atEnd()) && _running)
    _encoder->send(next());
  else if (atEnd())
    emit sessionComplete();
}

void
KochTutor::_nextline() {
  // For each line:
  //  -> every line consists of 25 symbols irrespecive of the group size.
  for (size_t i=0; i<25;) {
    // Sample group size
    size_t n = _minGroupSize + ( rand() % (1+_maxGroupSize-_minGroupSize) );
    for (size_t j=0; j<n; j++) {
      // Sample char from lesson
      size_t idx = 0;
      if (_prefLastChars) {
        double v = -1;
        while ((v < 0) || (v >= _lesson)) {
             v = (_lesson + _lesson*std::log(double(rand())/RAND_MAX)/_lastCharFrequencyFactor);
        }
        idx = size_t(v);
      } else {
        idx = _lesson*double(rand())/RAND_MAX;
      }
      _text.push_back(_lessons[idx]);
    }
    i += n;
    _text.push_back(' ');
  }
  _text.push_back('=');
  _text.push_back('\n');
  _linecount++;
}


/* ********************************************************************************************* *
 * RandomTutor
 * ********************************************************************************************* */
RandomTutor::RandomTutor(MorseEncoder *encoder, size_t minGroupSize, size_t maxGroupSize, int lines, bool showSummary, QObject *parent)
  : Tutor(encoder, parent), _minGroupSize(minGroupSize), _maxGroupSize(maxGroupSize),
    _lines(lines), _linecount(0), _showSummary(showSummary), _text(), _chars()
{
  // Init random number generator
  srand(time(nullptr));
  _chars << 'a' << 'b' << 'c' << 'd' << 'e' << 'f' << 'g' << 'h' << 'i' << 'j' << 'k' << 'l' << 'm'
         << 'n' << 'o' << 'p' << 'q' << 'r' << 's' << 't' << 'u' << 'v' << 'w' << 'x' << 'y' << 'z'
         << '0' << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8' << '9' << '.' << ',' << '?'
         << '/' << '&' << ':' << ';' << '=' << '+' << '-' << '@' << '(' << ')'
         << QChar(0x017a) /*ź*/ << QChar(0x00e4) /*ä*/ << QChar(0x0105) /*ą*/ << QChar(0x00f6) /*ö*/
         << QChar(0x00f8) /*ø*/ << QChar(0x00f3) /*ó*/ << QChar(0x00fc) /*ü*/ << QChar(0x016d) /*ŭ*/
         << QChar(0x03c7) /*χ*/ << QChar(0x0125) /*ĥ*/ << QChar(0x00e0) /*à*/ << QChar(0x00e5) /*å*/
         << QChar(0x00e8) /*è*/ << QChar(0x00e9) /*é*/ << QChar(0x0109) /*ę*/ << QChar(0x00f0) /*ð*/
         << QChar(0x00de) /*þ*/ << QChar(0x0109) /*ĉ*/ << QChar(0x0107) /*ć*/ << QChar(0x011d) /*ĝ*/
         << QChar(0x0125) /*ĵ*/ << QChar(0x015d) /*ŝ*/ << QChar(0x0142) /*ł*/ << QChar(0x0144) /*ń*/
         << QChar(0x00f1) /*ñ*/ << QChar(0x0107) /*ż*/ << QChar(0x00bf) /*¿*/ << QChar(0x00a1) /*¡*/
         << QChar(0x00df) /*ß*/ << QChar(0x0144) /*ś*/
         << QChar(0x2417) /* BK */ << QChar(0x2404) /* CL */ << QChar(0x2403) /* SK */
         << QChar(0x2406) /* SN */;
  connect(_encoder, SIGNAL(charSend(QChar)), this, SLOT(onCharSend(QChar)));
}

RandomTutor::RandomTutor(MorseEncoder *encoder, const QSet<QChar> &chars, size_t minGroupSize, size_t maxGroupSize, int lines, bool showSummary, QObject *parent)
  : Tutor(encoder, parent), _minGroupSize(minGroupSize), _maxGroupSize(maxGroupSize), _lines(lines),
    _showSummary(showSummary), _text(), _chars(), _chars_send(0), _words_send(0), _lines_send(0)
{
  // Init random number generator
  srand(time(nullptr));
  _chars.reserve(chars.size());
  QSet<QChar>::const_iterator c = chars.begin();
  for (; c != chars.end(); c++)
    _chars.push_back(*c);

  connect(_encoder, SIGNAL(charSend(QChar)), this, SLOT(onCharSend(QChar)));
}

RandomTutor::~RandomTutor() {
  // pass...
}

QString
RandomTutor::summary() const {
  if (! _showSummary)
    return "";
  int chars_send = _chars_send; chars_send -= 3; // - "vvv\n"
  int words_send = _words_send;
  int lines_send = _lines_send; lines_send -= 1; // - "vvv\n"
  chars_send -= lines_send; // - " =\n" at end of each line
  return tr("\n\nSent %1 chars in %2 words and %3 lines.")
      .arg(chars_send).arg(words_send).arg(lines_send);
}

QChar
RandomTutor::next() {
  if (0 == _chars.size()) { return '\0'; }

  if ((0 == _text.size()) && (_linecount == _lines))
    reset();
  else if (0 == _text.size())
    _nextline();

  QChar ch = _text.first(); _text.pop_front();
  if ('\n' == ch)
    _lines_send++;
  else if (' ' == ch)
    _words_send++;
  else
    _chars_send++;
  return ch;
}

bool
RandomTutor::atEnd() {
  return ((0 == _text.size()) && (_lines == _linecount)) || (0 == _chars.size());
}

void
RandomTutor::start() {
  this->reset();
  Tutor::start();
  _encoder->start();
  _encoder->send(this->next());
}

void
RandomTutor::stop() {
  Tutor::stop();
  _encoder->stop();
}

void
RandomTutor::reset()
{
  // Empty current session
  _text.clear();
  // If empty char set -> done.
  if (0 == _chars.size()) { return; }
  // Reset linecount
  _linecount = 0;
  //
  _chars_send = _words_send = _lines_send = 0;
  // Insert "vvv\n"
  _text.push_back('v'); _text.push_back('v'); _text.push_back('v'); _text.push_back('\n');
  // sample a line
  _nextline();
}

void RandomTutor::onCharSend(QChar c) {
  if ((! this->atEnd()) && _running)
    _encoder->send(next());
  else if (atEnd())
    emit sessionComplete();
}

bool
RandomTutor::needsDecoder() const {
  return false;
}

void
RandomTutor::_nextline() {
  for (size_t i=0; i<25;) {
    // Sample group size
    size_t n = _minGroupSize + ( rand() % (1+_maxGroupSize-_minGroupSize) );
    for (size_t j=0; j<n; j++) {
      // Sample char from chars
      size_t idx = _chars.size()*double(rand())/RAND_MAX;
      _text.push_back(_chars[idx]);
    }
    i += n;
    _text.push_back(' ');
  }
  _text.push_back('=');
  _text.push_back('\n');
  _linecount++;
}

QSet<QChar>
RandomTutor::chars() const {
  QSet<QChar> cs;
  for (int i=0; i<_chars.size(); i++) { cs.insert(_chars[i]); }
  return cs;
}

void
RandomTutor::setChars(const QSet<QChar> &chars) {
  QSet<QChar>::const_iterator c = chars.begin();
  _chars.clear(); _chars.reserve(chars.size());
  for (; c != chars.end(); c++) {
    _chars.push_back(*c);
  }
}

int
RandomTutor::lines() const {
  return _lines;
}

void
RandomTutor::setLines(int lines) {
  _lines = lines;
}


/* ********************************************************************************************* *
 * GenQSOTutor
 * ********************************************************************************************* */
GenTextTutor::GenTextTutor(MorseEncoder *encoder, const QString &filename, QObject *parent)
  : Tutor(encoder, parent), _generator(filename)
{
  connect(_encoder, SIGNAL(charSend(QChar)), this, SLOT(onCharSend(QChar)));
}

GenTextTutor::~GenTextTutor() {
  // pass...
}

void
GenTextTutor::start() {
  this->reset();
  Tutor::start();
  _encoder->start();
  _encoder->send(this->next());
}

void
GenTextTutor::stop() {
  Tutor::stop();
  _encoder->stop();
}

QChar
GenTextTutor::next() {
  // If no text is selected yet
  if (0 == _currentText.size()) {
    // Sample next text
    QTextStream buffer(&_currentText);
    QHash<QString, QString> ctx;
    _generator.generate(buffer, ctx);
  }
  // Get char from QSO
  QChar c = _currentText[0];
  // Update remaining text
  _currentText = _currentText.remove(0, 1);
  return c;
}

bool
GenTextTutor::atEnd() {
  return 0 == _currentText.size();
}

void
GenTextTutor::reset() {
  _currentText.clear();
}

bool
GenTextTutor::needsDecoder() const {
  return false;
}

void
GenTextTutor::onCharSend(QChar ch) {
  if ((! atEnd()) && _running)
    _encoder->send(next());
  else if (atEnd())
    emit sessionComplete();
}

/* ********************************************************************************************* *
 * TxTutor placeholder tutor
 * ********************************************************************************************* */
TXTutor::TXTutor(QObject *parent)
  : Tutor(0, parent)
{
  // pass...
}

TXTutor::~TXTutor() {
  // pass...
}

QChar
TXTutor::next() {
  return 0;
}

bool
TXTutor::atEnd() {
  return false;
}

void
TXTutor::reset() {
  // pass...
}

bool
TXTutor::needsDecoder() const {
  return true;
}


/* ********************************************************************************************* *
 * ChatTutor
 * ********************************************************************************************* */
ChatTutor::ChatTutor(MorseEncoder *encoder, QObject *parent)
  : Tutor(encoder, parent), _chat(), _inputbuffer(), _outputbuffer()
{
    connect(_encoder, SIGNAL(charSend(QChar)), this, SLOT(onCharSend(QChar)));
}

ChatTutor::~ChatTutor() {
  // pass...
}

QChar
ChatTutor::next() {
  QChar c = 0;
  if (_outputbuffer.size()) {
    c = _outputbuffer.at(0);
    _outputbuffer.remove(0,1);
  }
  return c;
}

bool
ChatTutor::atEnd() {
  return false;
}

void
ChatTutor::start() {
  this->reset();
  Tutor::start();
  _encoder->start();
}

void
ChatTutor::stop() {
  Tutor::stop();
  _encoder->stop();
}

void
ChatTutor::reset() {
  _outputbuffer.clear();
  _inputbuffer.clear();
}

void
ChatTutor::onCharSend(QChar ch) {
    if (0 ==_outputbuffer.size())
        return;
    ch = _outputbuffer.at(0);
    _outputbuffer.remove(0, 1);
    _encoder->send(ch);
}

bool
ChatTutor::needsDecoder() const {
  return true;
}

void
ChatTutor::handle(const QChar &ch) {
  _inputbuffer.push_back(ch);
  if (_inputbuffer.endsWith(" k ") || _inputbuffer.endsWith("qrz? ")) {
    QTextStream outstr(&_outputbuffer);
    _chat.handle(_inputbuffer.simplified(), outstr);
    _inputbuffer.clear();
    _encoder->send(next());
  }
}
