#include "application.hh"
#include "settings.hh"
#include "globals.hh"
#include <cmath>


Application::Application(int &argc, char *argv[])
 : QApplication(argc, argv), _audio_sink(0), _noiseEffect(0), _encoder(0), _tutor(0)
{
  Settings settings;
  PortAudio::init();

  _audio_sink = new PortAudioSink(this);
  _audio_sink->setVolume(settings.volume());

  _noiseEffect = new NoiseEffect(_audio_sink, settings.noiseEnabled(), settings.noiseSNR(), this);
  _fadingEffect = new FadingEffect(_noiseEffect, settings.fadingEnabled(),
                                   settings.fadingMaxDamp(), settings.fadingRate(), this);

  _encoder = new MorseEncoder(_fadingEffect, settings.tone(), settings.tone()+settings.dashPitch(),
                              settings.speed(), settings.effSpeed(), settings.sound(), true, this);

  _decoder = new MorseDecoder(settings.speed(), this);
  _audio_src = new PortAudioSource(_decoder, this);

  switch (settings.tutor()) {
  case Settings::TUTOR_KOCH:
    _tutor = new KochTutor(settings.kochLesson(), settings.kochPrefLastChars(),
                           settings.kochMinGroupSize(), settings.kochMaxGroupSize(), this);
    break;
  case Settings::TUTOR_RANDOM:
    _tutor = new RandomTutor(settings.randomChars(), this);
    break;
  case Settings::TUTOR_QSO:
    _tutor = new QSOTutor(this);
    break;
  case Settings::TUTOR_TX:
    _tutor = 0;
    break;
  }

  // Connect singals
  QObject::connect(_encoder, SIGNAL(charsSend()), this, SLOT(onCharsSend()));
  QObject::connect(_encoder, SIGNAL(charSend(QChar)), this, SLOT(onCharSend(QChar)));

  QObject::connect(_decoder, SIGNAL(charReceived(QChar)), this, SLOT(onCharReceived(QChar)));
  QObject::connect(_decoder, SIGNAL(unknownCharReceived(QString)), this, SLOT(onUnknownCharReceived(QString)));
}

Application::~Application() {
  _audio_src->stop();
  PortAudio::finalize();
}

void
Application::setVolume(double factor) {
  _audio_sink->setVolume(factor);
  // factor is [0,2] -> mapped logarithmic on [-60, 0] db for decoder threshold
  double db = -60 + 60*(1-factor/2);
  _decoder->setThreshold(std::pow(10, db/10));
}

void
Application::startSession() {
  if (_tutor) {
    _tutor->reset();
    _encoder->start();
    _encoder->send(_tutor->next());
  } else {
    _audio_src->start();
  }
}

void
Application::stopSession() {
  if (_tutor) {
    _encoder->stop();
    _tutor->reset();
  } else {
    _audio_src->stop();
  }
}

void
Application::applySettings()
{
  // Stop encoder if running
  _encoder->stop();
  // Stop RX if running
  _audio_src->stop();

  // Get settings
  Settings settings;

  // Update audio settings
  _audio_sink->setVolume(settings.volume());
  // factor is [0,2] -> mapped logarithmic on [-60, 0] db for decoder threshold
  double db = -60 + 60*(1-settings.volume()/2);
  _decoder->setThreshold(std::pow(10, db/10));

  // Update effects
  _noiseEffect->setEnabled(settings.noiseEnabled());
  _noiseEffect->setSNR(settings.noiseSNR());
  _fadingEffect->setEnabled(settings.fadingEnabled());
  _fadingEffect->setMaxDamp(settings.fadingMaxDamp());
  _fadingEffect->setFadingRate(settings.fadingRate());

  // Reconfigure encoder
  _encoder->setSpeed(settings.speed());
  _encoder->setEffSpeed(settings.effSpeed());
  _encoder->setDotTone(settings.tone());
  _encoder->setDashTone(settings.tone()+settings.dashPitch());
  _encoder->setSound(settings.sound());

  // Reconfigure decoder
  _decoder->setSpeed(settings.speed());

  // Reconfigure tutor
  if (_tutor) { delete _tutor; }
  switch (settings.tutor()) {
  case Settings::TUTOR_KOCH:
    _tutor = new KochTutor(settings.kochLesson(), settings.kochPrefLastChars(),
                           settings.kochMinGroupSize(), settings.kochMaxGroupSize(), this);
    break;
  case Settings::TUTOR_RANDOM:
    _tutor = new RandomTutor(settings.randomChars(), this);
    break;
  case Settings::TUTOR_QSO:
    _tutor = new QSOTutor(this);
    break;
  case Settings::TUTOR_TX:
    _tutor = 0;
    break;
  }
}

void
Application::onCharsSend() {
  if (_tutor->atEnd()) {
    emit sessionFinished(); return;
  }
  _encoder->send(_tutor->next());
}

void
Application::onCharSend(QChar ch) {
  emit charSend(Globals::mapProsign(ch));
}

void
Application::onCharReceived(QChar ch) {
  emit charSend(Globals::mapProsign(ch));
}

void
Application::onUnknownCharReceived(QString ch) {
  emit charSend(QString("<%1>").arg(ch));
}
