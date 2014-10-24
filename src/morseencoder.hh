#ifndef __KOCHMORSE_MORSEENCODER_HH__
#define __KOCHMORSE_MORSEENCODER_HH__

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QObject>
#include <QHash>
#include "audiosink.hh"


/** Implements the morse-encoder either running in a separate thread or performing the encoding and
 * playback blocking. */
class MorseEncoder : public QThread
{
  Q_OBJECT

public:
  /** Constructor.
   * @param sink Specifies the audio-sink for the playback.
   * @param ditFreq Specifies the tone freqency of a dot (dit).
   * @param daFreq Specifies the tone frequency of a dash (da).
   * @param speed Specifies the character speed (in WPM).
   * @param effSpeed Specifies the pause speed (in WPM).
   * @param parallel If @c true, the encoding and playback is performed in a separate thread.
   * @param parent Specifies the @c QObject parent. */
  explicit MorseEncoder(
      AudioSink *sink, double ditFreq, double daFreq, double speed, double effSpeed,
      bool parallel=true, QObject *parent= 0);

  /** Sends the given text. */
  void send(const QString &text);
  /** Sends the given char. */
  void send(QChar ch);
  /** Start the encoding and playback. Has no effect if the encoder is not run in a separate
   * thread. */
  void start();
  /** Stops the encoding/playback and joins the encoder thread. Has no effect if the encoder is not
   * run in a separate thread. */
  void stop();
  /** Returns the duration of playback. */
  double time() const;
  /** Resets the playback time. */
  void resetTime();
  /** (Re-) Sets the character speed. */
  void setSpeed(int speed);
  /** (Re-) Sets the effective (pause) speed. */
  void setEffSpeed(int speed);
  /** (Re-) Sets the dot-tone (dit) frequency. */
  void setDotTone(double freq);
  /** (Re-) Sets the dash-tone (da) frequency. */
  void setDashTone(double freq);

signals:
  /** Signals that a char was send. */
  void charSend(QChar ch);
  /** Signals that all chars in the queue have been send. */
  void charsSend();

public:
  static QString mapProsign(QChar ch);

protected:
  /** The main function of the parallel thread. */
  virtual void run();
  /** Encodes and plays the given char (blocking). */
  void _send(QChar ch);
  /** (Re-) Creates the dit, da and pause samples. */
  void _createSamples();

protected:
  /** The static morse-code table. */
  static QHash<QChar, QString> _codeTable;
  /** Maps my prosign unicode chars to text repr. */
  static QHash<QChar, QString> _prosignTable;

protected:
  /** The current dot-tone frequency. */
  double _ditFreq;
  /** The current dash-tone frequency. */
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
  /** If true, the encoding thread is running. */
  bool _running;
  /** The queue lock. */
  QMutex         _queueLock;
  /** The queue condition (empty or not). */
  QWaitCondition _queueWait;
  /** The character queue. */
  QList<QChar>   _queue;
  /** The number of milliseconds played. */
  double _played;
};


#endif // __KOCHMORSE_MORSEENCODER_HH__
