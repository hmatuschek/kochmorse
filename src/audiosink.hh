#ifndef __KOCHMORSE_AUDIOSINK_HH__
#define __KOCHMORSE_AUDIOSINK_HH__

#include <QObject>
#include <QByteArray>
#include <inttypes.h>
#include <QIODevice>
#include <QAudioOutput>
#include <QAudioInput>


class QAudioSink: public QIODevice
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit QAudioSink(QObject *parent = 0);
  /** Destructor. */
  virtual ~QAudioSink();

  /** Returns the current volume. */
  double volume() const;
  /** Sets the current volume. */
  void setVolume(double factor);

  bool isSequential() const;
  bool waitForBytesWritten(int msecs);
  qint64 bytesAvailable() const;
  qint64 bytesToWrite() const;

protected:
  qint64 readData(char *data, qint64 maxlen);
  qint64 writeData(const char *data, qint64 len);

protected:
  /** The port-audio stream. */
  QAudioOutput *_output;
  /** The internal sample buffer. */
  QByteArray _buffer;
};


/** Wraps QAudioInput for recording. It chooses the default recording device.*/
class QAudioSource: public QIODevice
{
  Q_OBJECT

public:
  /** Constructor.
   * @param sink Specifies the audio sink, the sample will be send to.
   * @param parent Specified the QObject parent. */
  explicit QAudioSource(QIODevice *sink, QObject *parent=0);
  /** Destructor. */
  virtual ~QAudioSource();

  /** Starts processing. */
  void start();
  /** Stops processing. */
  void stop();
  /** Returns @c true if the input stream is running. */
  bool isRunning() const;

protected:
  qint64 readData(char *data, qint64 maxlen);
  qint64 writeData(const char *data, qint64 len);

protected:
  /** The Qt Audio stream. */
  QAudioInput *_stream;
  /** The first processing node. */
  QIODevice *_sink;
  /** Output buffer. */
  QByteArray _buffer;
};

#endif // __KOCHMORSE_AUDIOSINK_HH__
