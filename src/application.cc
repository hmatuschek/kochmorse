#include "application.hh"
#include <iostream>


Application::Application(int argc, char *argv[])
 : QApplication(argc, argv), _audio(0), _encoder(0), _trainer(0)
{
  AudioSink::init();
  _audio = new AudioSink(this);
  _encoder = new MorseEncoder(_audio, 750, 750, 20, 15, true, this);
  _trainer = new KochTrainer(2, this);

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
Application::startSession() {
  _trainer->reset();
  _encoder->start();
  _encoder->send(_trainer->next());
}

void
Application::stopSession() {
  _encoder->stop();
  _trainer->reset();
}

void
Application::onCharsSend() {
  if (_trainer->atEnd()) {
    emit sessionFinished(); return;
  }
  _encoder->send(_trainer->next());
}

void
Application::onCharSend(QChar ch) {
  emit charSend(ch);
}
