#include "tutor.hh"
#include <cmath>
#include <QFile>
#include <time.h>
#include <QDebug>


/* ********************************************************************************************* *
 * Tutor interface
 * ********************************************************************************************* */
Tutor::Tutor(QObject *parent)
  : QObject(parent)
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
  // pass...
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


KochTutor::KochTutor(int lesson, bool prefLastChars, bool repeatLastChar,
                     size_t minGroupSize, size_t maxGroupSize,
                     int lines, bool showSummary, QObject *parent)
  : Tutor(parent), _lesson(lesson), _prefLastChars(prefLastChars), _repeatLastChar(repeatLastChar),
    _minGroupSize(std::min(minGroupSize, maxGroupSize)),
    _maxGroupSize(std::max(minGroupSize, maxGroupSize)),
    _lines(lines), _linecount(0), _showSummary(showSummary), _text(), _chars_send(0),
    _words_send(0), _lines_send(0)
{
  // Init random number generator
  srand(time(0));
}

KochTutor::~KochTutor() {
  // pass...
}

QChar
KochTutor::next() {
  if ((0 == _text.size()) && (_lines == _linecount))
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
  return tr("\n\nSent %1 chars in %2 words and %3 lines.")
      .arg(_chars_send).arg(_words_send).arg(_lines_send);
}

void
KochTutor::setShowSummary(bool show) {
  _showSummary = show;
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

void
KochTutor::_nextline() {
  // for each of the 5 lines
  for (size_t i=0; i<25;) {
    // Sample group size
    size_t n = _minGroupSize + ( rand() % (1+_maxGroupSize-_minGroupSize) );
    for (size_t j=0; j<n; j++) {
      // Sample char from lesson
      size_t idx = 0;
      if (_prefLastChars) {
        double v = -1;
        while ((v < 0) || (v >= _lesson)) {
          v = (_lesson + _lesson*std::log(double(rand())/RAND_MAX)/4);
        }
        idx = v;
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
RandomTutor::RandomTutor(size_t minGroupSize, size_t maxGroupSize, int lines, bool showSummary, QObject *parent)
  : Tutor(parent), _minGroupSize(minGroupSize), _maxGroupSize(maxGroupSize),
    _lines(lines), _linecount(0), _showSummary(showSummary), _text(), _chars()
{
  // Init random number generator
  srand(time(0));
  _chars << 'a' << 'b' << 'c' << 'd' << 'e' << 'f' << 'g' << 'h' << 'i' << 'j' << 'k' << 'l' << 'm'
         << 'n' << 'o' << 'p' << 'q' << 'r' << 's' << 't' << 'u' << 'v' << 'w' << 'x' << 'y' << 'z'
         << '0' << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8' << '9' << '.' << ',' << '?'
         << '/' << '&' << ':' << ';' << '=' << '+' << '-' << '@' << '(' << ')'
         << QChar(0x2417) /* BK */ << QChar(0x2404) /* CL */ << QChar(0x2403) /* SK */
         << QChar(0x2406) /* SN */;
}

RandomTutor::RandomTutor(const QSet<QChar> &chars, size_t minGroupSize, size_t maxGroupSize, int lines, bool showSummary, QObject *parent)
  : Tutor(parent), _minGroupSize(minGroupSize), _maxGroupSize(maxGroupSize), _lines(lines),
    _showSummary(showSummary), _text(), _chars(), _chars_send(0), _words_send(0), _lines_send(0)
{
  // Init random number generator
  srand(time(0));
  _chars.reserve(chars.size());
  QSet<QChar>::const_iterator c = chars.begin();
  for (; c != chars.end(); c++) { _chars.push_back(*c); }
}

RandomTutor::~RandomTutor() {
  // pass...
}

QString
RandomTutor::summary() const {
  if (! _showSummary)
    return "";
  return tr("\n\nSent %1 chars in %2 words and %3 lines.")
      .arg(_chars_send).arg(_words_send).arg(_lines_send);
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
 * QSOTutor
 * ********************************************************************************************* */
QSOTutor::QSOTutor(QObject *parent)
  : Tutor(parent)
{
  // Init RNG
  srand(time(0));

  // Read
  QFile f(":qso/qso.txt");
  f.open(QFile::ReadOnly|QFile::Text);

  while (! f.atEnd()) {
    QString line(f.readLine());
    line.replace("^ar", QChar('+'));
    line.replace("^as", QChar('&'));
    line.replace("^bt", QChar('='));
    line.replace("^bk", QChar(0x2417));
    line.replace("^cl", QChar(0x2404));
    line.replace("^sk", QChar(0x2403));
    line.replace("^sn", QChar(0x2406));
    line.replace("^kl", QChar(0x2407));
    line = line.simplified().toLower();
    _qso.push_back(line);
  }
}

QSOTutor::~QSOTutor() {
  // pass...
}

QChar
QSOTutor::next() {
  // If there are no QSOs
  if (0 == _qso.size()) { return QChar(0); }
  // If no QSO is selected yet
  if (0 == _currentQSO.size()) {
    // Sample next QSO
    _currentQSO = _qso[ _qso.size()*(double(rand())/RAND_MAX) ];
  }
  // Get char from QSO
  QChar c = _currentQSO[0];
  // Update remaining text
  _currentQSO = _currentQSO.remove(0, 1);
  return c;
}

bool
QSOTutor::atEnd() {
  return 0 == _currentQSO.size();
}

void
QSOTutor::reset() {
  _currentQSO.clear();
}

bool
QSOTutor::needsDecoder() const {
  return false;
}


/* ********************************************************************************************* *
 * GenQSOTutor
 * ********************************************************************************************* */
GenTextTutor::GenTextTutor(const QString &filename, QObject *parent)
  : Tutor(parent), _generator(filename)
{
  // pass...
}

GenTextTutor::~GenTextTutor() {
  // pass...
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


/* ********************************************************************************************* *
 * TxTutor placeholder tutor
 * ********************************************************************************************* */
TXTutor::TXTutor(QObject *parent)
  : Tutor(parent)
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
ChatTutor::ChatTutor(QObject *parent)
  : Tutor(parent), _qhal(), _inputbuffer(), _outputbuffer()
{
  QString buffer, line;
  QTextStream txt;
  TextGen generator(":/qso/qsogen.xml");
  QHash<QString, QString> ctx;

  // Teach QHal:
  for (int i=0; i<5000; i++) {
    buffer.clear();
    txt.setString(&buffer);
    generator.generate(txt, ctx);
    while (txt.readLineInto(&line)) {
      _qhal.learn(line.simplified());
    }
  }
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
ChatTutor::reset() {
  _outputbuffer.clear();
  _inputbuffer.clear();
}

bool
ChatTutor::needsDecoder() const {
  return true;
}

void
ChatTutor::handle(const QChar &ch) {
  _inputbuffer.push_back(ch);
  if (_inputbuffer.endsWith(" k ")) {
    _outputbuffer.append(_qhal.reply(_inputbuffer.simplified()));
    _inputbuffer.clear();
  }
}
