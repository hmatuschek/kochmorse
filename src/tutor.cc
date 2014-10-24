#include "tutor.hh"
#include <cmath>
#include <QFile>
#include <iostream>


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


/* ********************************************************************************************* *
 * KochTrainer
 * ********************************************************************************************* */
inline QVector<QChar> _initKochLessons() {
  QVector<QChar> chars;
  chars << 'k' << 'm' << 'r' << 's' << 'u' << 'a' << 'p' << 't' << 'l' << 'o' << 'w'
        << 'i' << '.' << 'n' << 'j' << 'e' << 'f' << '0' << 'y' << ',' << 'v' << 'g'
        << '5' << '/' << 'q' << '9' << 'z' << 'h' << '3' << '8' << 'b' << '?' << '4'
        << '2' << '7' << 'c' << '1' << 'd' << '6' << 'x' << '=' << QChar(0x2403) << '+';
  return chars;
}
// The vector of all chars ordered by lesson
QVector<QChar> KochTutor::_lessons = _initKochLessons();


KochTutor::KochTutor(int lesson, bool prefLastChars, QObject *parent)
  : Tutor(parent), _lesson(lesson), _prefLastChars(prefLastChars), _text()
{
  // Init random number generator
  srand(time(0));
}

KochTutor::~KochTutor() {
  // pass...
}

QChar
KochTutor::next() {
  if (0 == _text.size()) { reset(); }
  QChar ch = _text.first(); _text.pop_front();
  return ch;
}

bool
KochTutor::atEnd() {
  return 0 == _text.size();
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
  _prefLastChars=pref;
}

void
KochTutor::reset()
{
  // Empty current session
  _text.clear();
  // Insert "vvv\n"
  _text.push_back('v'); _text.push_back('v'); _text.push_back('v'); _text.push_back('\n');

  // For 5 lines
  for (int l=0; l<5; l++) {
    // and 5 groups
    for (int g=0; g<5; g++) {
      // of 5 chars
      for (int c=0; c<5; c++) {
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
      _text.push_back(' ');
    }
    _text.push_back('=');
    _text.push_back('\n');
  }
}



/* ********************************************************************************************* *
 * RandomTutor
 * ********************************************************************************************* */
RandomTutor::RandomTutor(QObject *parent)
  : Tutor(parent), _text(), _chars()
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

RandomTutor::RandomTutor(const QSet<QChar> &chars, QObject *parent)
  : Tutor(parent), _text(), _chars()
{
  // Init random number generator
  srand(time(0));
  _chars.reserve(chars.size());
  QSet<QChar>::const_iterator c = chars.begin();
  for (; c != chars.begin(); c++) { _chars.push_back(*c); }
}

RandomTutor::~RandomTutor() {
  // pass...
}

QChar
RandomTutor::next() {
  if (0 == _chars.size()) { return '\0'; }
  if (0 == _text.size()) { reset(); }
  QChar ch = _text.first(); _text.pop_front();
  return ch;
}

bool
RandomTutor::atEnd() {
  return (0 == _text.size()) || (0 == _chars.size());
}

void
RandomTutor::reset()
{
  // Empty current session
  _text.clear();
  // If empty char set -> done.
  if (0 == _chars.size()) { return; }
  // Insert "vvv\n"
  _text.push_back('v'); _text.push_back('v'); _text.push_back('v'); _text.push_back('\n');

  // For 5 lines
  for (int l=0; l<5; l++) {
    // and 5 groups
    for (int g=0; g<5; g++) {
      // of 5 chars
      for (int c=0; c<5; c++) {
        // Sample char from chars
        size_t idx = _chars.size()*double(rand())/RAND_MAX;
        _text.push_back(_chars[idx]);
      }
      _text.push_back(' ');
    }
    _text.push_back('=');
    _text.push_back('\n');
  }
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


/* ********************************************************************************************* *
 * QSOTrainer
 * ********************************************************************************************* */
QSOTutor::QSOTutor(QObject *parent)
  : Tutor(parent)
{
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
