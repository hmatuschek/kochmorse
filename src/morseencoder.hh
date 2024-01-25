#ifndef __KOCHMORSE_MORSEENCODER_HH__
#define __KOCHMORSE_MORSEENCODER_HH__

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QObject>
#include <QHash>
#include <QVector>
#include "audiosink.hh"


/** Implements the morse-encoder either running in a separate thread or performing the encoding and
 * playback blocking. */
class MorseEncoder : public QIODevice
{
  Q_OBJECT

public:
  /** Possible sound variants. */
  typedef enum {
    SOUND_SOFT = 0,
    SOUND_SHARP = 1,
    SOUND_CRACKING = 2
  } Sound;

  /** Possible jitter variants. */
  typedef enum {
    JITTER_EXACT   = 0,
    JITTER_BUG     = 1,
    JITTER_STRAIGHT = 2
  } Jitter;


public:
  /** Constructor.
   * @param ditFreq Specifies the tone frequency of a dot (dit).
   * @param daFreq Specifies the tone frequency of a dash (da).
   * @param speed Specifies the character speed (in WPM).
   * @param icpFac Specifies the inter-char pause factor (>=1).
   * @param iwpFac Specifies the inter-word pause factor (>=1).
   * @param jitter Specifies the dit/da length jitter.
   * @param parent Specifies the @c QObject parent. */
  explicit MorseEncoder(double ditFreq, double daFreq, double speed, double icpFac, double iwpFac,
      Sound sound, Jitter jitter=JITTER_EXACT, QObject *parent=nullptr);

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
  /** (Re-) Sets the inter-char pause factor. */
  void setICPFactor(double factor);
  /** (Re-) Sets the inter-word pause factor. */
  void setIWPFactor(double factor);
  /** (Re-) Sets the dot-tone (dit) frequency. */
  void setDotTone(double freq);
  /** (Re-) Sets the dash-tone (da) frequency. */
  void setDashTone(double freq);
  /** (Re-) Sets the sound. */
  void setSound(Sound sound);
  /** (Re-) Sets the jitter. */
  void setJitter(Jitter jitter);

  /** Returns the number of bytes available for reading.
   * Implements the QIODevice interface. */
  qint64 bytesAvailable() const;

signals:
  /** Signals that a char was send. */
  void charSend(QChar ch);
  /** Signals that all chars in the queue have been send. */
  void charsSend();

protected:
  /** Encodes and plays the next char. */
  void _send();
  /** (Re-) Creates the dit, da and pause samples. */
  void _createSamples();
  /** Implements the QIODevice interface. */
  qint64 readData(char *data, qint64 maxlen);
  /** Implements the QIODevice interface. */
  qint64 writeData(const char *data, qint64 len);

protected:
  /** The current dot-tone frequency. */
  double _ditFreq;
  /** The current dash-tone frequency. */
  double _daFreq;

  /** Char speed in WPM. */
  double _speed;
  /** Inter-char pause factor. */
  double _icpfac;
  /** Inter-word pause factor. */
  double _iwpfac;
  /** The sound variant. */
  Sound _sound;
  /** The jitter. */
  Jitter _jitter;

  /** Length of a "dit" in samples. */
  size_t _unitLength;

  /** A "dit" incl. inter-symbol pause. */
  QVector<QByteArray> _ditSamples;
  /** A "da" incl. inter-symbol pause. */
  QVector<QByteArray> _daSamples;
  /** Pause duration between chars. */
  QByteArray _icPause;
  /** Pause duration between words. */
  QByteArray _iwPause;

  /** The current char being send. */
  QChar          _current;
  /** The queue of chars to send. */
  QList<QChar>   _queue;
  /** Audio output buffer. */
  QByteArray     _buffer;
  /** The number of milliseconds send. */
  double _tsend;
  /** The number of milliseconds played. */
  double _played;
};


#endif // __KOCHMORSE_MORSEENCODER_HH__
