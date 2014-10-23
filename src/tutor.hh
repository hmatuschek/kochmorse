#ifndef __KOCHMORSE_TRAINER_HH__
#define __KOCHMORSE_TRAINER_HH__

#include <QObject>
#include <QList>
#include <QVector>


/** Abstract base class of all trainer classes. */
class Tutor : public QObject
{
  Q_OBJECT

protected:
  explicit Tutor(QObject *parent = 0);

public:
  virtual ~Tutor();

  virtual QChar next() = 0;
  virtual bool atEnd() = 0;
  virtual void reset() = 0;
};


class KochTutor: public Tutor
{
  Q_OBJECT

public:
  KochTutor(int lesson=2, QObject *parent=0);
  virtual ~KochTutor();

  QChar next();
  bool atEnd();
  void reset();

  int lesson() const;
  void setLesson(int lesson);

protected:
  int _lesson;
  QList<QChar> _text;

  static QVector<QChar> _lessons;
};

#endif // __KOCHMORSE_TRAINER_HH__
