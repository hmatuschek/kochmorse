#include "application.hh"
#include <iostream>
#include "settings.hh"


Application::Application(int argc, char *argv[])
 : QApplication(argc, argv), _audio(0), _encoder(0), _tutor(0)
{
  Settings settings;
  AudioSink::init();

  _audio = new AudioSink(this);
  _audio->setVolume(settings.volume());

  _encoder = new MorseEncoder(_audio, settings.tone(), settings.tone()+settings.dashPitch(),
                              settings.speed(), settings.effSpeed(), true, this);
  switch (settings.tutor()) {
  case Settings::TUTOR_KOCH: _tutor = new KochTutor(settings.kochLesson(), this); break;
  }

  // Connect singals
  QObject::connect(_encoder, SIGNAL(charsSend()), this, SLOT(onCharsSend()));
  QObject::connect(_encoder, SIGNAL(charSend(QChar)), this, SLOT(onCharSend(QChar)));
}

Application::~Application() {
  AudioSink::finalize();
}

int
Application::sessionTime() {
  return _encoder->time();
}

void
Application::setVolume(double factor) {
  _audio->setVolume(factor);
}

void
Application::startSession() {
  _tutor->reset();
  _encoder->start();
  _encoder->send(_tutor->next());
}

void
Application::stopSession() {
  _encoder->stop();
  _tutor->reset();
}

void
Application::applySettings()
{
  // Stop encoder if running
  _encoder->stop();

  // Get settings
  Settings settings;

  // Update audio settings
  _audio->setVolume(settings.volume());

  // Reconfigure encoder
  _encoder->setSpeed(settings.speed());
  _encoder->setEffSpeed(settings.effSpeed());
  _encoder->setTone(settings.tone());
  _encoder->setDashTone(settings.tone()+settings.dashPitch());

  // Reconfigure tutor
  delete _tutor;
  switch (settings.tutor()) {
  case Settings::TUTOR_KOCH: _tutor = new KochTutor(settings.kochLesson(), this); break;
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
  emit charSend(ch);
}
