#include "application.hh"
#include "settings.hh"
#include "globals.hh"
#include <cmath>
#include <QTranslator>
#include <QDebug>


Application::Application(int &argc, char *argv[])
 : QApplication(argc, argv), _running(false), _audio_sink(0), _noiseEffect(0), _encoder(0), _tutor(0)
{
  Settings settings;

  QTranslator *translator = new QTranslator(this);
  translator->load(QLocale(), "kochmorse", "_", ":/lang/");
  //qDebug() << "UI Locales: " << QLocale().uiLanguages();
  installTranslator(translator);

  _audio_sink = new QAudioSink(0, this);
  _audio_sink->setVolume(settings.volume());

  _noiseEffect = new NoiseEffect(0, settings.noiseEnabled(), settings.noiseSNR(),
                                 settings.noiseFilterEnabled(), settings.tone(),
                                 settings.noiseFilterBw(), this);

  _qrm = new QRMGenerator(0, settings.qrmStations(), settings.qrmSNR(), this);
  _qrm->enable(settings.qrmEnabled());

  _fadingEffect = new FadingEffect(0, settings.fadingEnabled(),
                                   settings.fadingMaxDamp(), settings.fadingRate(), this);

  _encoder = new MorseEncoder(settings.tone(), settings.tone()+settings.dashPitch(),
                              settings.speed(), settings.effSpeed(), settings.sound(),
                              settings.jitter(), this);

  _audio_sink->setSource(_noiseEffect);
  _noiseEffect->setSource(_qrm);
  _qrm->setSource(_fadingEffect);
  _fadingEffect->setSource(_encoder);

  _decoder = new MorseDecoder(settings.speed(), std::pow(10.,settings.decoderLevel()/20), this);
  _audio_src = new QAudioSource(_decoder, this);

  switch (settings.tutor()) {
  case Settings::TUTOR_KOCH:
    _tutor = new KochTutor(_encoder, settings.kochLesson(), settings.kochPrefLastChars(), settings.kochRepeatLastChar(),
                           settings.kochMinGroupSize(), settings.kochMaxGroupSize(),
                           (settings.kochInfiniteLineCount() ? -1: settings.kochLineCount()),
                           settings.kochSummary(), this);
    break;

  case Settings::TUTOR_RANDOM:
    _tutor = new RandomTutor(_encoder, settings.randomChars(),
                             settings.randomMinGroupSize(), settings.randomMaxGroupSize(),
                             (settings.randomInfiniteLineCount() ? -1: settings.randomLineCount()),
                             settings.randomSummary(), this);
    break;

  case Settings::TUTOR_TX:
    _tutor = new TXTutor(this);
    break;

  case Settings::TUTOR_CHAT:
    _tutor = new ChatTutor(_encoder, this);
    break;

  case Settings::TUTOR_TEXTGEN:
    _tutor = new GenTextTutor(_encoder, settings.textGenFilename());
    break;
  }

  // Connect singals
  connect(_encoder, SIGNAL(charSend(QChar)), this, SLOT(onCharSend(QChar)));

  connect(_decoder, SIGNAL(charReceived(QChar)), this, SLOT(onCharReceived(QChar)));
  connect(_decoder, SIGNAL(unknownCharReceived(QString)), this, SLOT(onUnknownCharReceived(QString)));

  connect(_tutor, SIGNAL(sessionComplete()), this, SIGNAL(sessionComplete()));
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
  _tutor->start();
  if (_tutor->needsDecoder())
    _audio_src->start();
}

void
Application::stopSession() {
  /// @todo Reenable hybernation
  _running = false;
  _tutor->stop();
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
  _noiseEffect->setFiterEnabled(settings.noiseFilterEnabled());
  _noiseEffect->setFc(settings.tone());
  _noiseEffect->setBw(settings.noiseFilterBw());
  _fadingEffect->setEnabled(settings.fadingEnabled());
  _fadingEffect->setMaxDamp(settings.fadingMaxDamp());
  _fadingEffect->setFadingRate(settings.fadingRate());
  _qrm->enable(settings.qrmEnabled());
  _qrm->setStations(settings.qrmStations());
  _qrm->setSNR(settings.qrmSNR());

  // Reconfigure encoder
  _encoder->setSpeed(settings.speed());
  _encoder->setEffSpeed(settings.effSpeed());
  _encoder->setDotTone(settings.tone());
  _encoder->setDashTone(settings.tone()+settings.dashPitch());
  _encoder->setSound(settings.sound());
  _encoder->setJitter(settings.jitter());

  // Reconfigure decoder
  _decoder->setSpeed(settings.speed());

  // Reconfigure tutor
  if (_tutor)
    delete _tutor;

  switch (settings.tutor()) {
    case Settings::TUTOR_KOCH:
      _tutor = new KochTutor(_encoder, settings.kochLesson(), settings.kochPrefLastChars(),
                             settings.kochRepeatLastChar(), settings.kochMinGroupSize(), settings.kochMaxGroupSize(),
                             (settings.kochInfiniteLineCount() ? -1: settings.kochLineCount()),
                             settings.kochSummary(), this);
      break;

    case Settings::TUTOR_RANDOM:
      _tutor = new RandomTutor(_encoder, settings.randomChars(), settings.randomMinGroupSize(),
                               settings.randomMaxGroupSize(),
                               (settings.randomInfiniteLineCount() ? -1: settings.randomLineCount()),
                               settings.randomSummary(), this);
      break;

    case Settings::TUTOR_TEXTGEN:
      _tutor = new GenTextTutor(_encoder, settings.textGenFilename());
      break;

    case Settings::TUTOR_TX:
      _tutor = new TXTutor(this);
      break;

    case Settings::TUTOR_CHAT:
      _tutor = new ChatTutor(_encoder, this);
      break;
  }

  connect(_tutor, SIGNAL(sessionComplete()), this, SIGNAL(sessionComplete()));
}


void
Application::onCharSend(QChar ch) {
  emit charSend(Globals::mapProsign(ch));
}

void
Application::onCharReceived(QChar ch) {
  _tutor->handle(ch);
  emit charReceived(Globals::mapProsign(ch));
}

void
Application::onUnknownCharReceived(QString ch) {
  emit charReceived(QString("<%1>").arg(ch));
}
