#include "trainer.hh"


/* ********************************************************************************************* *
 * Trainer interface
 * ********************************************************************************************* */
Trainer::Trainer(QObject *parent)
  : QObject(parent)
{
  // pass...
}

Trainer::~Trainer() {
  // pass...
}


/* ********************************************************************************************* *
 * KochTrainer
 * ********************************************************************************************* */
inline QVector<QChar> _initKochLessons() {
  QVector<QChar> chars;
  chars << 'm' << 'n' << 'o';
  return chars;
}
// The vector of all chars ordered by lesson
QVector<QChar> KochTrainer::_lessons = _initKochLessons();


KochTrainer::KochTrainer(int lesson, QObject *parent)
  : Trainer(parent), _lesson(lesson), _text()
{
  // Init random number generator
  srand(time(0));
}

KochTrainer::~KochTrainer() {
  // pass...
}

QChar
KochTrainer::next() {
  if (0 == _text.size()) { reset(); }
  QChar ch = _text.first(); _text.pop_front();
  return ch;
}

bool
KochTrainer::atEnd() {
  return 0 == _text.size();
}

void
KochTrainer::reset()
{
  // Empty current session
  _text.clear();
  // Insert "vvv "
  _text.push_back('v'); _text.push_back('v'); _text.push_back('v'); _text.push_back(' ');

  // For 5 lines
  for (int l=0; l<5; l++) {
    // and 5 groups
    for (int g=0; g<5; g++) {
      // of 5 chars
      for (int c=0; c<5; c++) {
        // Sample char from lesson
        size_t idx = _lesson*double(rand())/RAND_MAX;
        _text.push_back(_lessons[idx]);
      }
      _text.push_back(' ');
    }
    _text.push_back('=');
    _text.push_back('\n');
  }
}
