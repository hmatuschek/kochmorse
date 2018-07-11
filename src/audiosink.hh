#ifndef __KOCHMORSE_AUDIOSINK_HH__
#define __KOCHMORSE_AUDIOSINK_HH__

#include <QObject>
#include <QByteArray>
//#include <portaudio.h>
#include <inttypes.h>
#include <QIODevice>
#include <QAudioOutput>
#include <QAudioInput>



/** Base for all audio processing or playback classes. */
class AudioSink
{
protected:
  /** Constructor. */
  AudioSink();

public:
  /** Destructor. */
  virtual ~AudioSink();

  /** Processes or plays the given PCA (16bit, signed integer). */
  virtual void process(const QByteArray &data) = 0;
};



/** Static methods to PortAudio. */
class PortAudio
{
public:
  /** Initializes PortAudio library. */
  static void init();
  /** Terminates PortAudio library. */
  static void finalize();
};



/** Wraps PortAudio for playback. It chooses the default playback device.*/
class PortAudioSink : public QObject, public AudioSink
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit PortAudioSink(QObject *parent = 0);
  /** Destructor. */
  virtual ~PortAudioSink();

  /** Plays the given PCA (16bit, signed integer) blocking. */
  void process(const QByteArray &data);
  /** Returns the current volume. */
  double volume() const;
  /** Sets the current volume. */
  void setVolume(double factor);

protected:
  /** The port-audio stream. */
  PaStream *_stream;
  /** The current volume-factor. */
  double _volumeFactor;
};



class QAudioSink: public QIODevice, public AudioSink
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit QAudioSink(QObject *parent = 0);
  /** Destructor. */
  virtual ~QAudioSink();

  /** Plays the given PCA (16bit, signed integer) blocking. */
  void process(const QByteArray &data);
  /** Returns the current volume. */
  double volume() const;
  /** Sets the current volume. */
  void setVolume(double factor);

protected:
  qint64 readData(char *data, qint64 maxlen);
  qint64 writeData(const char *data, qint64 len);

protected:
  /** The port-audio stream. */
  QAudioOutput *_output;
  /** The internal sample buffer. */
  QByteArray _buffer;
};



/** Wraps PortAudio for recording. It chooses the default recording device.*/
class PortAudioSource: public QObject
{
  Q_OBJECT

public:
  /** Constructor.
   * @param sink Specifies the audio sink, the sample will be send to.
   * @param parent Specified the QObject parent. */
  explicit PortAudioSource(AudioSink *sink, QObject *parent=0);
  /** Destructor. */
  virtual ~PortAudioSource();

  /** Starts processing. */
  void start();
  /** Stops processing. */
  void stop();
  /** Returns @c true if the input stream is running. */
  bool isRunning() const;

protected:
  /** The callback used by PortAudio. */
  static int _pa_callback(
      const void *in, void *out, unsigned long Nframes,
      const PaStreamCallbackTimeInfo *tinfo, PaStreamCallbackFlags flags, void *ctx);

protected:
  /** The port-audio stream. */
  PaStream *_stream;
  /** The first processing node. */
  AudioSink *_sink;
  /** Output buffer. */
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
  explicit QAudioSource(AudioSink *sink, QObject *parent=0);
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
  AudioSink *_sink;
  /** Output buffer. */
  QByteArray _buffer;
};

#endif // __KOCHMORSE_AUDIOSINK_HH__
