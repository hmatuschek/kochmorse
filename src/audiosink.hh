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
  explicit AudioSink(QObject *parent = 0, double sampleRate=16000);
  virtual ~AudioSink();

  void play(const QByteArray &data);
  double rate() const;

public:
  static void init();
  static void finalize();

protected:
  PaStream *_stream;
  double _rate;
};


#endif // __KOCHMORSE_AUDIOSINK_HH__
