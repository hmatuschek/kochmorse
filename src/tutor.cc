#include "tutor.hh"
#include <cmath>
#include <QFile>
#include <time.h>
#include <QDebug>
#include "textcompare.hh"
#include "settings.hh"


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

bool
Tutor::isVerifying() const {
  return false;
}

bool
Tutor::isOutputHidden() const {
  return false;
}

int
Tutor::verify(const QString & text, QString &summary) {
  Q_UNUSED(text); Q_UNUSED(summary);
  return 0;
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


KochTutor::KochTutor(MorseEncoder *encoder, int lesson, bool prefLastChars, bool repeatLastChar,
                     size_t minGroupSize, size_t maxGroupSize,
                     int lines, bool showSummary, bool verify, bool hideOutput,
                     int successThreshold, QObject *parent)
  : Tutor(encoder, parent), _lesson(lesson), _prefLastChars(prefLastChars),
    _repeatLastChar(repeatLastChar), _minGroupSize(std::min(minGroupSize, maxGroupSize)),
    _maxGroupSize(std::max(minGroupSize, maxGroupSize)), _lines(lines), _linecount(0),
    _showSummary(showSummary), _verify(verify), _hideOutput(hideOutput),
    _threshold(successThreshold), _text(), _chars_send(0), _words_send(0), _lines_send(0)
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

  QChar ch = _text.first();
  _text.pop_front();
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
  int threshold = int(_chars_send*(100-_threshold))/100;
  if (_lesson < (_lessons.size()-1))
    return tr("\n\nSent %1 chars in %2 words and %3 lines. "
              "If you have less than %4 mistakes, you can proceed to lesson %5.")
        .arg(_chars_send).arg(_words_send).arg(_lines_send).arg(threshold).arg(_lesson+1);
  else
    return tr("\n\nSent %1 chars in %2 words and %3 lines. "
              "If you have less than %4 mistakes, you completed the course!")
        .arg(_chars_send).arg(_words_send).arg(_lines_send).arg(threshold);
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
  _sendText.clear();
  _chars_send = 0;
  _words_send = 0;
  _lines_send = 0;
  // sample a line of text.
  _nextline();
}

bool
KochTutor::needsDecoder() const {
  return false;
}

bool
KochTutor::isVerifying() const {
  return _verify;
}

bool
KochTutor::isOutputHidden() const {
  return _hideOutput;
}

int
KochTutor::verify(const QString &text, QString &summary) {
  QVector<int> mistakes;
  int err = textCompare(_sendText.toLower(), text.toLower(), mistakes);
  // Format Send text
  QString tx, rx;
  QTextStream buffer(&tx);
  for (int i=0; i<_sendText.size(); i++) {
    if ('\n' == _sendText.at(i))
      buffer << "<br>";
    else if (mistakes.contains(i))
      buffer << "<span style=\"background-color:red;\">" << _sendText.at(i) << "</span>";
    else
      buffer << _sendText.at(i);
  }
  buffer.setString(&rx);
  for (int i=0; i<text.size(); i++) {
    if ('\n' == text.at(i))
      buffer << "<br>";
    else
      buffer << text.at(i);
  }
  int correct = 100*double(_chars_send-err)/_chars_send;
  summary = tr("<html><h3>Text send:</h3><p>%1</p>"
               "<h3>Text entered:</h3><p>%2</p>"
               "<h3>Summary:</h3><p>Characters/Words/Lines send: %3/%4/%5<br>"
               "Mistakes: %6<br>"
               "Accuracy: <b>%7%</b></p>").arg(tx).arg(rx).arg(_chars_send)
      .arg(_words_send).arg(_lines_send).arg(err).arg(correct);
  if (correct >= _threshold) {
    summary.append(tr("<p><b>You achieved an accuracy of %1% &gt;= %2%. "
                      "You may advance to the next lesson!</b></p></html>").arg(correct).arg(_threshold));
  } else {
    summary.append(tr("<p><b>You achieved an accuracy of %1% &lt; %2%. "
                      "Keep on practicing!</b> Have a look at the mistakes you made above. "
                      "If you confused some characters (e.g., s, h & 5) frequently, consider "
                      "using the Random tutor to practice only those characters you "
                      "confused.</p></html>").arg(correct).arg(_threshold));
  }

  emit sessionVerified("koch", _lesson, correct);
  return err;
}

void KochTutor::onCharSend(QChar c) {
  Q_UNUSED(c);
  if ((! this->atEnd()) && _running)
    _encoder->send(next());
  else if (atEnd())
    emit sessionFinished();
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
        while ((v < 0) || (v > _lesson)) {
          v = (_lesson + _lesson*std::log(double(rand())/RAND_MAX)/4);
        }
        idx = size_t(v);
      } else {
        idx = _lesson*double(rand())/RAND_MAX;
      }
      _text.push_back(_lessons[idx]);
      _sendText.push_back(_lessons[idx]);
      _chars_send++;
    }
    i += n;
    _text.push_back(' ');
    _sendText.push_back(' ');
    _words_send++;
  }
  _text.push_back('=');
  _text.push_back(' ');
  _text.push_back(' ');
  _text.push_back('\n');
  _sendText.push_back('\n');
  _linecount++;
  _lines_send++;
}

const QVector<QChar> &
KochTutor::lessons() {
  return _lessons;
}


/* ********************************************************************************************* *
 * RandomTutor
 * ********************************************************************************************* */
RandomTutor::RandomTutor(MorseEncoder *encoder, size_t minGroupSize, size_t maxGroupSize, int lines, bool showSummary, bool verify, bool hideOutput, QObject *parent)
  : Tutor(encoder, parent), _minGroupSize(minGroupSize), _maxGroupSize(maxGroupSize),
    _lines(lines), _linecount(0), _showSummary(showSummary), _verify(verify),
    _hideOutput(hideOutput), _text(), _chars(), _chars_send(0), _words_send(0), _lines_send(0)
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

RandomTutor::RandomTutor(MorseEncoder *encoder, const QSet<QChar> &chars, size_t minGroupSize,
                         size_t maxGroupSize, int lines, bool showSummary, bool verify,
                         bool hideOutput, QObject *parent)
  : Tutor(encoder, parent), _minGroupSize(minGroupSize), _maxGroupSize(maxGroupSize), _lines(lines),
    _showSummary(showSummary), _verify(verify), _hideOutput(hideOutput), _text(), _chars(),
    _chars_send(0), _words_send(0), _lines_send(0)
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

  QChar ch = _text.first();
  _text.pop_front();
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
  _sendText.clear();
  // If empty char set -> done.
  if (0 == _chars.size()) { return; }
  // Reset linecount
  _linecount = 0;
  // reset char, word & line count
  _chars_send = _words_send = _lines_send = 0;
  // Insert "vvv\n"
  _text.push_back('v'); _text.push_back('v'); _text.push_back('v'); _text.push_back('\n');
  // sample a line
  _nextline();
}

void RandomTutor::onCharSend(QChar c) {
  Q_UNUSED(c);
  if ((! this->atEnd()) && _running)
    _encoder->send(next());
  else if (atEnd())
    emit sessionFinished();
}

bool
RandomTutor::needsDecoder() const {
  return false;
}

bool
RandomTutor::isVerifying() const {
  return _verify;
}

bool
RandomTutor::isOutputHidden() const {
  return _hideOutput;
}

int
RandomTutor::verify(const QString &text, QString &summary) {
  QVector<int> mistakes;
  int err = textCompare(_sendText.toLower(), text.toLower(), mistakes);
  // Format Send text
  QString tx, rx;
  QTextStream buffer(&tx);
  for (int i=0; i<_sendText.size(); i++) {
    if ('\n' == _sendText.at(i))
      buffer << "<br>";
    else if (mistakes.contains(i))
      buffer << "<span style=\"background-color:red;\">" << _sendText.at(i) << "</span>";
    else
      buffer << _sendText.at(i);
  }
  buffer.setString(&rx);
  for (int i=0; i<text.size(); i++) {
    if ('\n' == text.at(i))
      buffer << "<br>";
    else
      buffer << text.at(i);
  }
  int correct = 100*double(_chars_send-err)/_chars_send;
  summary = tr("<html><h3>Text send:</h3><p>%1</p>"
               "<h3>Text entered:</h3><p>%2</p>"
               "<h3>Summary:</h3><p>Characters/Words/Lines send: %3/%4/%5<br>"
               "Mistakes: %6<br>"
               "Accuracy: <b>%7%</b></p>").arg(tx).arg(rx).arg(_chars_send)
      .arg(_words_send).arg(_lines_send).arg(err).arg(correct);

  emit sessionVerified("rand", _chars.size(), correct);
  return err;
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
      _sendText.push_back(_chars[idx]);
      _chars_send++;
    }
    i += n;
    _text.push_back(' ');
    _sendText.push_back(' ');
    _words_send++;
  }
  _text.push_back('=');
  _text.push_back(' ');
  _text.push_back('\n');
  _sendText.push_back('\n');
  _linecount++;
  _lines_send++;
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
 * WordsworthTutor
 * ********************************************************************************************* */
inline QVector<QString> _initWordsworthLessons() {
  QVector<QString> words;
  words << "k"  << "cq" << "de" << "es" << "is" << "hi" << "hr" << "ur" << "vy" << "73" << "my"
        << "88" << "bk" << "cl" << "dx" << "wx" << "el" << "fb" << "om" << "no" << "op" << "tu"
        << "yl" << "gm" << "gd" << "ga" << "ge" << "gn"
        << "abt" << "age" << "agn" << "ant" << "btu" << "cpy" << "cul" << "gud" << "hw?" << "key"
        << "pkt" << "pse" << "pwr" << "qrm" << "qrn" << "qrp" << "qro" << "qrs" << "qrt" << "qrx"
        << "qrz" << "qsb" << "qsl" << "qso" << "qsy" << "qth" << "rig" << "rpt" << "rst" << "tks"
        << "tnx" << "yrs"
        << "beam" << "long" << "loop" << "name" << "runs" << "temp" << "test" << "vert" << "watt"
        << "wire" << "yagi"
        << "dipole";
  return words;
}
// The vector of all chars ordered by lesson
QVector<QString> WordsworthTutor::_lessons = _initWordsworthLessons();


WordsworthTutor::WordsworthTutor(
    MorseEncoder *encoder, int lesson, bool prefLastWords, bool repeatLastWord,
    int lines, bool showSummary, bool verify, bool hideOutput,
    int successThreshold, QObject *parent)
  : Tutor(encoder, parent), _lesson(lesson), _prefLastWords(prefLastWords),
    _repeatLastWord(repeatLastWord), _lines(lines), _linecount(0),
    _showSummary(showSummary), _verify(verify), _hideOutput(hideOutput),
    _threshold(successThreshold), _text(), _chars_send(0), _words_send(0), _lines_send(0)
{
  // Init random number generator
  srand(time(nullptr));

  connect(_encoder, SIGNAL(charSend(QChar)), this, SLOT(onCharSend(QChar)));
}

WordsworthTutor::~WordsworthTutor() {
  // pass...
}

QChar
WordsworthTutor::next() {
  if (atEnd())
    reset();
  else if (0 == _text.size())
    _nextline();

  QChar ch = _text.first();
  _text.pop_front();
  return ch;
}

bool
WordsworthTutor::atEnd() {
  return ((0 == _text.size()) && (_lines == _linecount));
}

int
WordsworthTutor::lesson() const {
  return _lesson;
}
void
WordsworthTutor::setLesson(int lesson) {
  _lesson = std::max(2, std::min(lesson, _lessons.size()));
}

bool
WordsworthTutor::prefLastWords() const {
  return _prefLastWords;
}

void
WordsworthTutor::setPrefLastWords(bool pref) {
  _prefLastWords = pref;
}

bool
WordsworthTutor::repeatLastWord() const {
  return _repeatLastWord;
}

void
WordsworthTutor::setRepeatLastWord(bool enable) {
  _repeatLastWord = enable;
}

int
WordsworthTutor::lines() const {
  return _lines;
}

void
WordsworthTutor::setLines(int lines) {
  _lines = lines;
}

QString
WordsworthTutor::summary() const {
  if (! _showSummary)
    return "";
  int threshold = int(_chars_send*(100-_threshold))/100;
  if (_lesson < (_lessons.size()-1))
    return tr("\n\nSent %1 chars in %2 words and %3 lines. "
              "If you have less than %4 mistakes, you can proceed to lesson %5.")
        .arg(_chars_send).arg(_words_send).arg(_lines_send).arg(threshold).arg(_lesson+1);
  else
    return tr("\n\nSent %1 chars in %2 words and %3 lines. "
              "If you have less than %4 mistakes, you completed the course!")
        .arg(_chars_send).arg(_words_send).arg(_lines_send).arg(threshold);
}

void
WordsworthTutor::setShowSummary(bool show) {
  _showSummary = show;
}

void
WordsworthTutor::start() {
  this->reset();
  Tutor::start();
  _encoder->start();
  _encoder->send(this->next());
}

void
WordsworthTutor::stop() {
  Tutor::stop();
  _encoder->stop();
}

void
WordsworthTutor::reset()
{
  // Empty current session
  _text.clear();
  // Reset line count
  _linecount = 0;
  // Insert "vvv\n"
  _text.push_back('v'); _text.push_back('v'); _text.push_back('v'); _text.push_back('\n');
  // Insert newest char if "repeat last word" is enabled
  if (_repeatLastWord) {
    for (int i=0; i<5; i++) {
      for (int j=0; j<_lessons[_lesson-1].size(); j++)
        _text.push_back(_lessons[_lesson-1][j]);
      _text.push_back(' ');
    }
    _text.push_back('\n');
  }
  _sendText.clear();
  _chars_send = 0;
  _words_send = 0;
  _lines_send = 0;
  // sample a line of text.
  _nextline();
}

bool
WordsworthTutor::needsDecoder() const {
  return false;
}

bool
WordsworthTutor::isVerifying() const {
  return _verify;
}

bool
WordsworthTutor::isOutputHidden() const {
  return _hideOutput;
}

int
WordsworthTutor::verify(const QString &text, QString &summary) {
  QVector<int> mistakes;
  int err = textCompare(_sendText.toLower(), text.toLower(), mistakes);
  // Format Send text
  QString tx, rx;
  QTextStream buffer(&tx);
  for (int i=0; i<_sendText.size(); i++) {
    if ('\n' == _sendText.at(i))
      buffer << "<br>";
    else if (mistakes.contains(i))
      buffer << "<span style=\"background-color:red;\">" << _sendText.at(i) << "</span>";
    else
      buffer << _sendText.at(i);
  }
  buffer.setString(&rx);
  for (int i=0; i<text.size(); i++) {
    if ('\n' == text.at(i))
      buffer << "<br>";
    else
      buffer << text.at(i);
  }
  int correct = 100*double(_chars_send-err)/_chars_send;
  summary = tr("<html><h3>Text send:</h3><p>%1</p>"
               "<h3>Text entered:</h3><p>%2</p>"
               "<h3>Summary:</h3><p>Characters/Words/Lines send: %3/%4/%5<br>"
               "Mistakes: %6<br>"
               "Accuracy: <b>%7%</b></p>").arg(tx).arg(rx).arg(_chars_send)
      .arg(_words_send).arg(_lines_send).arg(err).arg(correct);
  if (correct >= _threshold) {
    summary.append(tr("<p><b>You achieved an accuracy of %1% &gt;= %2%. "
                      "You may advance to the next lesson!</b></p></html>").arg(correct).arg(_threshold));
  } else {
    summary.append(tr("<p><b>You achieved an accuracy of %1% &lt; %2%. "
                      "Keep on practicing!</b> Have a look at the mistakes you made above. "
                      "If you confused some characters (e.g., s, h & 5) frequently, consider "
                      "using the Random tutor to practice only those characters you "
                      "confused.</p></html>").arg(correct).arg(_threshold));
  }

  emit sessionVerified("words", _lesson, correct);
  return err;
}

void
WordsworthTutor::onCharSend(QChar c) {
  Q_UNUSED(c);
  if ((! this->atEnd()) && _running)
    _encoder->send(next());
  else if (atEnd())
    emit sessionFinished();
}

void
WordsworthTutor::_nextline() {
  for (size_t i=0; i<10; i++) {
    // Sample word from lesson
    size_t idx = 0;
    if (_prefLastWords) {
      double v = -1;
      while ((v < 0) || (v > _lesson)) {
        v = (_lesson + _lesson*std::log(double(rand())/RAND_MAX)/4);
      }
      idx = size_t(v);
    } else {
      idx = _lesson*double(rand())/RAND_MAX;
    }
    for (int j=0; j<_lessons[idx].size(); j++)
      _text.push_back(_lessons[idx][j]);
    _sendText.push_back(_lessons[idx]);
    _chars_send += _lessons[idx].size();
    _text.push_back(' ');
    _sendText.push_back(' ');
    _words_send++;
  }
  _text.push_back('=');
  _text.push_back(' ');
  _text.push_back(' ');
  _text.push_back('\n');
  _sendText.push_back('\n');
  _linecount++;
  _lines_send++;
}

const QVector<QString> &
WordsworthTutor::lessons() {
  return _lessons;
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
  Q_UNUSED(ch);
  if ((! atEnd()) && _running)
    _encoder->send(next());
  else if (atEnd())
    emit sessionFinished();
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
