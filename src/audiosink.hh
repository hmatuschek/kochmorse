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
  AudioSink(QObject *parent=0);

public:
  virtual ~AudioSink();

  /** Processes or plays the given PCA (16bit, signed integer). */
  virtual void play(const QByteArray &data) = 0;

  /** Returns the sample-rate of the output. */
  virtual double rate() const = 0;
};


/** Wraps PortAudio for playback. It chooses the default playback device.*/
class PortAudioSink : public AudioSink
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit PortAudioSink(double sampleRate=16000, QObject *parent = 0);
  /** Destructor. */
  virtual ~PortAudioSink();

  /** Plays the given PCA (16bit, signed integer) blocking. */
  void play(const QByteArray &data);
  /** Returns the sample-rate of the output. */
  double rate() const;
  /** Returns the current volume. */
  double volume() const;
  /** Sets the current volume. */
  void setVolume(double factor);

public:
  /** Initializes PortAudio. */
  static void init();
  /** Terminates PortAudio. */
  static void finalize();

protected:
  /** The port-audio stream. */
  PaStream *_stream;
  /** The sample-rate. */
  double _rate;
  /** The current volume-factor. */
  double _volumeFactor;
};


#endif // __KOCHMORSE_AUDIOSINK_HH__
