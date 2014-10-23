#ifndef __KOCHMORSE_TRAINER_HH__
#define __KOCHMORSE_TRAINER_HH__

#include <QObject>
#include <QList>
#include <QVector>
#include <QSet>


/** Abstract base class of all tutor classes. */
class Tutor : public QObject
{
  Q_OBJECT

protected:
  /** Hidden constructor. */
  explicit Tutor(QObject *parent = 0);

public:
  /** Destructor. */
  virtual ~Tutor();

  /** Gets the next char from a session. */
  virtual QChar next() = 0;
  /** Returns true if the last char of a session has be taken. */
  virtual bool atEnd() = 0;
  /** Reset the tutor (start a new session). */
  virtual void reset() = 0;
};


class KochTutor: public Tutor
{
  Q_OBJECT

public:
  KochTutor(int lesson=2, bool prefLastChars=false, QObject *parent=0);
  virtual ~KochTutor();

  QChar next();
  bool atEnd();
  void reset();

  int lesson() const;
  void setLesson(int lesson);

  bool prefLastChars() const;
  void setPrefLastChars(bool pref);

protected:
  int _lesson;
  bool _prefLastChars;
  QList<QChar> _text;

  static QVector<QChar> _lessons;
};


class RandomTutor: public Tutor
{
  Q_OBJECT

public:
  explicit RandomTutor(QObject *parent=0);
  explicit RandomTutor(const QSet<QChar> &chars, QObject *parent=0);
  virtual ~RandomTutor();

  QChar next();
  bool atEnd();
  void reset();

  QSet<QChar> chars() const;
  void setChars(const QSet<QChar> &chars);

protected:
  /** Text for the current session. */
  QList<QChar> _text;
  /** Vector of chars to choose from. */
  QVector<QChar> _chars;
};

#endif // __KOCHMORSE_TRAINER_HH__
