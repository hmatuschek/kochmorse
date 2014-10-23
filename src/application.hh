#ifndef __KOCHMORSE_APPLICATION_HH__
#define __KOCHMORSE_APPLICATION_HH__

#include <QApplication>
#include "audiosink.hh"
#include "morseencoder.hh"
#include "tutor.hh"


class Application : public QApplication
{
  Q_OBJECT
public:
  explicit Application(int argc, char *argv[]);
  virtual ~Application();

  int sessionTime();

public slots:
  void startSession();
  void stopSession();
  /** Updates all object from the given settings. */
  void applySettings();

signals:
  void sessionFinished();
  void charSend(QChar ch);

protected slots:
  void onCharsSend();
  void onCharSend(QChar ch);

protected:
  AudioSink *_audio;
  MorseEncoder *_encoder;
  Tutor *_tutor;
};

#endif // __KOCHMORSE_APPLICATION_HH__
