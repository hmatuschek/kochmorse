#ifndef __KOCHMORSE_AUDIOSINK_HH__
#define __KOCHMORSE_AUDIOSINK_HH__

#include <QObject>
#include <QByteArray>
#include <portaudio.h>
#include <inttypes.h>

/** Base for all audio processing and playback classes. */
class AudioSink: public QObject
{
  Q_OBJECT

protected:
  /** Constructor. */
  AudioSink(QObject *parent=0);

public:
  virtual ~AudioSink();

  /** Processes or plays the given PCA (16bit, signed integer). */
  virtual void play(const QByteArray &data) = 0;
};


/** Static methods to port audio. */
class PortAudio
{
public:
  /** Initializes PortAudio library. */
  static void init();
  /** Terminates PortAudio library. */
  static void finalize();
};


/** Wraps PortAudio for playback. It chooses the default playback device.*/
class PortAudioSink : public AudioSink
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit PortAudioSink(QObject *parent = 0);
  /** Destructor. */
  virtual ~PortAudioSink();

  /** Plays the given PCA (16bit, signed integer) blocking. */
  void play(const QByteArray &data);
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
  AudioSink *_sink;
  QByteArray _buffer;
};

#endif // __KOCHMORSE_AUDIOSINK_HH__
