#ifndef __KOCHMORSE_APPLICATION_HH__
#define __KOCHMORSE_APPLICATION_HH__

#include <QApplication>
#include "audiosink.hh"
#include "morseencoder.hh"
#include "morsedecoder.hh"
#include "tutor.hh"
#include "effect.hh"


/** Central application class. Manages audio output, effects, morse encoding and the tutors. */
class Application : public QApplication
{
  Q_OBJECT
public:
  /** Constructor. */
  explicit Application(int &argc, char *argv[]);
  /** Destructor. */
  virtual ~Application();

  /** Sets the output volume. */
  void setVolume(double factor);

public slots:
  /** Starts a session. */
  void startSession();
  /** Stops a session. */
  void stopSession();
  /** Updates all objects from the persisten settings. */
  void applySettings();

signals:
  /** Gets emitted once a session is done. */
  void sessionFinished();
  /** Gets emitted once for each character send. */
  void charSend(QString ch);

protected slots:
  /** Gets called if the morse-encoder queue gets empty. */
  void onCharsSend();
  /** Gets called if a character was send by the encoder. */
  void onCharSend(QChar ch);
  /** Gets called if the morse-decoder received a char. */
  void onCharReceived(QChar ch);
  /** Gets called if the morse-decoder received an unknown char. */
  void onUnknownCharReceived(QString ch);

protected:
  /** The audio output device. */
  PortAudioSink *_audio_sink;
  /** The noise effect instance. */
  NoiseEffect *_noiseEffect;
  /** The "fading" effect instance. */
  FadingEffect *_fadingEffect;
  /** The actual morse encoder. */
  MorseEncoder *_encoder;
  /** The current tutor instance. */
  Tutor *_tutor;

  /** The audio source device. */
  PortAudioSource *_audio_src;
  /** The decoder device. */
  MorseDecoder *_decoder;
};

#endif // __KOCHMORSE_APPLICATION_HH__
