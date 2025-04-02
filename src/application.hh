#ifndef __KOCHMORSE_APPLICATION_HH__
#define __KOCHMORSE_APPLICATION_HH__

#include <QApplication>
#include "audiosink.hh"
#include "morseencoder.hh"
#include "morsedecoder.hh"
#include "tutor.hh"
#include "effect.hh"
#include "qrm.hh"
#include "checkupdate.hh"
#include "highscore.hh"

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

  QString summary() const;

  Tutor *currentTutor() const;

public slots:
  /** Starts a session. */
  void startSession();
  /** Stops a session. */
  void stopSession();
  /** Updates all objects from the persisten settings. */
  void applySettings();

signals:
  /** Gets emitted whenever the current tutor has been changed. */
  void tutorChanged();
  /** Gets emitted once a session is done. */
  void sessionFinished();
  /** Gets emitted once for each character send. */
  void charSend(QString ch);
  /** Gets emitted once for each character received. */
  void charReceived(QString ch);

protected slots:
  /** Gets called if a character was send by the encoder. */
  void onCharSend(QChar ch);
  /** Gets called if the morse-decoder received a char. */
  void onCharReceived(QChar ch);
  /** Gets called if the morse-decoder received an unknown char. */
  void onUnknownCharReceived(QString ch);
  void onUpdateAvailable(QString version);
  void onSessionVerified(const QString &tutor, int lesson, int score);

protected:
  /** If @c true a session is running. */
  bool _running;
  /** The audio output device. */
  KAudioSink *_audio_sink;
  /** The noise effect instance. */
  NoiseEffect *_noiseEffect;
  /** QRM. */
  QRMGenerator *_qrm;
  /** The "fading" effect instance. */
  FadingEffect *_fadingEffect;
  /** The actual morse encoder. */
  MorseEncoder *_encoder;
  /** The current tutor instance. */
  Tutor *_tutor;

  /** The audio source device. */
  KAudioSource *_audio_src;
  /** The decoder device. */
  MorseDecoder *_decoder;

  CheckUpdate _checkUpdate;
  HighScore   *_highscore;
};

#endif // __KOCHMORSE_APPLICATION_HH__
