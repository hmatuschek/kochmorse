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
  explicit QAudioSink(QIODevice *src = nullptr, QObject *parent = nullptr);
  /** Destructor. */
  virtual ~QAudioSink();

  /** Returns the current volume. */
  double volume() const;
  /** Sets the current volume. */
  void setVolume(double factor);

  /** (Re-) Sets the audio output device. */
  void setOutputDevice(const QAudioDeviceInfo &output_device);

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
  QAudioOutput *_output;
  /** Holds the current output audio device of _output. */
  QAudioDeviceInfo _output_device;
  /** Holds the audio source. */
  QIODevice *_source;

  double _volume;
};


/** Wraps QAudioInput for recording. It chooses the default recording device.*/
class QAudioSource: public QIODevice
{
  Q_OBJECT

public:
  /** Constructor.
   * @param sink Specifies the audio sink, the sample will be send to.
   * @param parent Specified the QObject parent. */
  explicit QAudioSource(QIODevice *sink, QObject *parent=nullptr);
  /** Destructor. */
  virtual ~QAudioSource();

  /** (Re-) Sets the audio input device. */
  void setInputDevice(const QAudioDeviceInfo &input_device);

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
  QAudioInput *_input;
  /** Holds the current input audio device of _input. */
  QAudioDeviceInfo _input_device;
  /** The first processing node. */
  QIODevice *_sink;
  /** Output buffer. */
  QByteArray _buffer;
};

#endif // __KOCHMORSE_AUDIOSINK_HH__
