#ifndef __KOCHMORSE_AUDIOSINK_HH__
#define __KOCHMORSE_AUDIOSINK_HH__

#include <QObject>
#include <QByteArray>
#include <inttypes.h>
#include <QIODevice>
#include <QAudioDevice>
#include <QAudioOutput>
#include <QAudioInput>
#include <QAudioSource>
#include <QAudioSink>


class KAudioSink: public QIODevice
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit KAudioSink(QIODevice *src = nullptr, QObject *parent = nullptr);
  /** Destructor. */
  virtual ~KAudioSink();

  /** Returns the current volume. */
  double volume() const;
  /** Sets the current volume. */
  void setVolume(double factor);

  /** (Re-) Sets the audio output device. */
  void setOutputDevice(const QAudioDevice &output_device);

  /** (Re-) Sets the audio source. */
  void setSource(QIODevice *source);

  /** Returns @c true. Implements the QIODevice interface. */
  bool isSequential() const;
  /** Returns the number of bytes available to reading.
   * Implements the QIODevice interface. */
  qint64 bytesAvailable() const;

protected:
  /** Implements the QIODevice interface. */
  qint64 readData(char *data, qint64 maxlen);
  /** Implements the QIODevice interface. */
  qint64 writeData(const char *data, qint64 len);

protected:
  /** The port-audio stream. */
  QAudioSink *_output;
  /** Holds the current output audio device of _output. */
  QAudioDevice _output_device;
  /** Holds the audio source. */
  QIODevice *_source;

  double _volume;
};


/** Wraps QAudioSource for recording. It chooses the default recording device.*/
class KAudioSource: public QIODevice
{
  Q_OBJECT

public:
  /** Constructor.
   * @param sink Specifies the audio sink, the sample will be send to.
   * @param parent Specified the QObject parent. */
  explicit KAudioSource(QIODevice *sink, QObject *parent=nullptr);
  /** Destructor. */
  virtual ~KAudioSource();

  /** (Re-) Sets the audio input device. */
  void setInputDevice(const QAudioDevice &input_device);

  /** Starts processing. */
  void start();
  /** Stops processing. */
  void stop();
  /** Returns @c true if the input stream is running. */
  bool isRunning() const;

protected:
  /** Implements the QIODevice interface. */
  qint64 readData(char *data, qint64 maxlen);
  /** Implements the QIODevice interface. */
  qint64 writeData(const char *data, qint64 len);

protected:
  /** The Qt Audio stream. */
  QAudioSource *_input;
  /** Holds the current input audio device of _input. */
  QAudioDevice _input_device;
  /** The first processing node. */
  QIODevice *_sink;
  /** Output buffer. */
  QByteArray _buffer;
};

#endif // __KOCHMORSE_AUDIOSINK_HH__
