#ifndef __KOCHMORSE_MORSEENCODER_HH__
#define __KOCHMORSE_MORSEENCODER_HH__

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QObject>
#include <QHash>
#include "audiosink.hh"


class MorseEncoder : public QThread
{
  Q_OBJECT

public:
  explicit MorseEncoder(
      AudioSink *sink, double ditFreq, double daFreq, double speed, double effSpeed,
      bool parallel=true, QObject *parent= 0);

  void send(const QString &text);
  void send(QChar ch);

  void start();
  void stop();

  double time() const;
  void resetTime();

signals:
  /** Signals that a char was send. */
  void charSend(QChar ch);
  /** Signals that all chars in the queue have been send. */
  void charsSend();

protected:
  virtual void run();
  void _send(QChar ch);

protected:
  static QHash<QChar, QString> _codeTable;

protected:
  double _ditFreq;
  double _daFreq;

  /** Char speed in WPM. */
  double _speed;
  /** Effective speed (pauses) in WPM. */
  double _effSpeed;

  /** Length of a "dit" in samples. */
  size_t _unitLength;
  /** Pause length in samples. */
  size_t _effUnitLength;

  /** A "dit" incl. inter-symbol pause. */
  QByteArray _ditSample;
  /** A "da" incl. inter-symbol pause. */
  QByteArray _daSample;
  /** Pause duration between chars. */
  QByteArray _icPause;
  /** Pause duration between words. */
  QByteArray _iwPause;

  /** Audio backend. */
  AudioSink *_sink;

  /** If @c true, the morse-code generation and playback will happen in a separate thread. */
  bool _parallel;
  bool _running;
  QMutex         _queueLock;
  QWaitCondition _queueWait;
  QList<QChar>   _queue;

  double _played;
};

#endif // __KOCHMORSE_MORSEENCODER_HH__
