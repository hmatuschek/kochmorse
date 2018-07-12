#include "application.hh"
#include "settings.hh"
#include "globals.hh"
#include <cmath>


Application::Application(int &argc, char *argv[])
 : QApplication(argc, argv), _running(false), _audio_sink(0), _noiseEffect(0), _encoder(0), _tutor(0)
{
  Settings settings;
  //PortAudio::init();

  _audio_sink = new QAudioSink(this);
  _audio_sink->setVolume(settings.volume());

  _noiseEffect = new NoiseEffect(_audio_sink, settings.noiseEnabled(), settings.noiseSNR(), this);
  _fadingEffect = new FadingEffect(_noiseEffect, settings.fadingEnabled(),
                                   settings.fadingMaxDamp(), settings.fadingRate(), this);

  _encoder = new MorseEncoder(_fadingEffect, settings.tone(), settings.tone()+settings.dashPitch(),
                              settings.speed(), settings.effSpeed(), settings.sound(), true, this);

  _decoder = new MorseDecoder(settings.speed(), std::pow(10.,settings.decoderLevel()/20), this);
  _audio_src = new QAudioSource(_decoder, this);

  switch (settings.tutor()) {
  case Settings::TUTOR_KOCH:
    _tutor = new KochTutor(settings.kochLesson(), settings.kochPrefLastChars(), settings.kochRepeatLastChar(),
                           settings.kochMinGroupSize(), settings.kochMaxGroupSize(),
                           (settings.kochInfiniteLineCount() ? -1: settings.kochLineCount()),
                           settings.kochSummary(), this);
    break;

  case Settings::TUTOR_RANDOM:
    _tutor = new RandomTutor(settings.randomChars(),
                             settings.randomMinGroupSize(), settings.randomMaxGroupSize(),
                             (settings.randomInfiniteLineCount() ? -1: settings.randomLineCount()),
                             settings.randomSummary(), this);
    break;

  case Settings::TUTOR_QSO:
    _tutor = new GenTextTutor(":/qso/qsogen.xml", this);
    break;

  case Settings::TUTOR_QCODE:
    _tutor = new GenTextTutor(":/qso/qcodes.xml", this);
    break;

  case Settings::TUTOR_TX:
    _tutor = new TXTutor(this);
    break;

  case Settings::TUTOR_CHAT:
    _tutor = new ChatTutor(this);
    break;

  case Settings::TUTOR_TEXTGEN:
    _tutor = new GenTextTutor(settings.textGenFilename());
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
}

void
Application::setVolume(double factor) {
  _audio_sink->setVolume(factor);
}

QString
Application::summary() const {
  if (0 == _tutor)
    return "";
  return _tutor->summary();
}

void
Application::startSession() {
  /// @todo Disable hybernation
  _running = true;
  _tutor->reset();
  _encoder->start();
  _encoder->send(_tutor->next());
  if (_tutor->needsDecoder())
    _audio_src->start();
}

void
Application::stopSession() {
  /// @todo Reenable hybernation
  _running = false;
  _encoder->stop();
  _tutor->reset();
  if (_tutor->needsDecoder())
    _audio_src->stop();
}

void
Application::applySettings()
{
  stopSession();

  // Get settings
  Settings settings;

  // Update audio settings
  _audio_sink->setVolume(settings.volume());
  // factor is [0,2] -> mapped logarithmic on [-60, 0] db for decoder threshold
  _decoder->setThreshold(std::pow(10, settings.decoderLevel()/20));

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
  if (_tutor)
    delete _tutor;

  switch (settings.tutor()) {
    case Settings::TUTOR_KOCH:
      _tutor = new KochTutor(settings.kochLesson(), settings.kochPrefLastChars(), settings.kochRepeatLastChar(),
                             settings.kochMinGroupSize(), settings.kochMaxGroupSize(),
                             (settings.kochInfiniteLineCount() ? -1: settings.kochLineCount()),
                             settings.kochSummary(), this);
      break;

    case Settings::TUTOR_RANDOM:
      _tutor = new RandomTutor(settings.randomChars(), settings.randomMinGroupSize(),
                               settings.randomMaxGroupSize(),
                               (settings.randomInfiniteLineCount() ? -1: settings.randomLineCount()),
                               settings.randomSummary(), this);
      break;

    case Settings::TUTOR_QSO:
      _tutor = new GenTextTutor(":/qso/qsogen.xml",this);
      break;

    case Settings::TUTOR_QCODE:
      _tutor = new GenTextTutor(":/qso/qcodes.xml",this);
      break;

    case Settings::TUTOR_TX:
      _tutor = new TXTutor(this);
      break;

    case Settings::TUTOR_CHAT:
      _tutor = new ChatTutor(this);
      break;

    case Settings::TUTOR_TEXTGEN:
      _tutor = new GenTextTutor(settings.textGenFilename());
      break;
  }
}

void
Application::onCharsSend() {
  if (0 == _tutor)
    return;

  if (_tutor->atEnd()) {
    emit sessionFinished();
    return;
  }
  if (_running)
    _encoder->send(_tutor->next());
}

void
Application::onCharSend(QChar ch) {
  emit charSend(Globals::mapProsign(ch));
}

void
Application::onCharReceived(QChar ch) {
  _tutor->handle(ch);
  _encoder->send(_tutor->next());
  emit charReceived(Globals::mapProsign(ch));
}

void
Application::onUnknownCharReceived(QString ch) {
  emit charReceived(QString("<%1>").arg(ch));
}
