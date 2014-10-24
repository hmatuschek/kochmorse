#ifndef __KOCHMORSE_AUDIOSINK_HH__
#define __KOCHMORSE_AUDIOSINK_HH__

#include <QObject>
#include <QByteArray>
#include <portaudio.h>


/** Wraps PortAudio for playback. It chooses the default playback device.*/
class AudioSink : public QObject
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit AudioSink(double sampleRate=16000, QObject *parent = 0);
  /** Destructor. */
  virtual ~AudioSink();

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
