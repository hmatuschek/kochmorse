#ifndef __KOCHMORSE_TRAINER_HH__
#define __KOCHMORSE_TRAINER_HH__

#include <QObject>
#include <QList>
#include <QVector>


/** Abstract base class of all trainer classes. */
class Trainer : public QObject
{
  Q_OBJECT

protected:
  explicit Trainer(QObject *parent = 0);
  virtual ~Trainer();

public:
  virtual QChar next() = 0;
  virtual bool atEnd() = 0;
  virtual void reset() = 0;
};


class KochTrainer: public Trainer
{
  Q_OBJECT

public:
  KochTrainer(int lesson=2, QObject *parent=0);
  virtual ~KochTrainer();

  QChar next();
  bool atEnd();
  void reset();

protected:
  int _lesson;
  QList<QChar> _text;

  static QVector<QChar> _lessons;
};

#endif // __KOCHMORSE_TRAINER_HH__
