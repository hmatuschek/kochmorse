#ifndef __KOCHMORSE_APPLICATION_HH__
#define __KOCHMORSE_APPLICATION_HH__

#include <QApplication>
#include "audiosink.hh"
#include "morseencoder.hh"
#include "tutor.hh"
#include "effect.hh"


class Application : public QApplication
{
  Q_OBJECT
public:
  explicit Application(int &argc, char *argv[]);
  virtual ~Application();

  int sessionTime();
  void setVolume(double factor);

public slots:
  void startSession();
  void stopSession();
  /** Updates all objects from the persisten settings. */
  void applySettings();

signals:
  void sessionFinished();
  void charSend(QChar ch);

protected slots:
  void onCharsSend();
  void onCharSend(QChar ch);

protected:
  PortAudioSink *_audio;
  NoiseEffect *_noiseEffect;
  FadingEffect *_fadingEffect;
  MorseEncoder *_encoder;
  Tutor *_tutor;
};

#endif // __KOCHMORSE_APPLICATION_HH__
