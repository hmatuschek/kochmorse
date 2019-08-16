#include "application.hh"
#include "settings.hh"
#include "globals.hh"
#include <cmath>
#include <QTranslator>
#include <QDebug>
#include <QMessageBox>
#include "config.h"


Application::Application(int &argc, char *argv[])
 : QApplication(argc, argv), _running(false), _audio_sink(nullptr), _noiseEffect(nullptr),
   _encoder(nullptr), _tutor(nullptr), _checkUpdate(nullptr)
{
  Settings settings;

  QTranslator *translator = new QTranslator(this);
  translator->load(QLocale(), "kochmorse", "_", ":/lang/");
  //qDebug() << "UI Locales: " << QLocale().uiLanguages();
  installTranslator(translator);

  _audio_sink = new QAudioSink(nullptr, this);
  _audio_sink->setVolume(settings.volume());

  _noiseEffect = new NoiseEffect(nullptr, settings.noiseEnabled(), settings.noiseSNR(),
                                 settings.noiseFilterEnabled(), settings.tone(),
                                 settings.noiseFilterBw(), this);

  _qrm = new QRMGenerator(nullptr, settings.qrmStations(), settings.qrmSNR(), this);
  _qrm->enable(settings.qrmEnabled());

  _fadingEffect = new FadingEffect(nullptr, settings.fadingEnabled(),
                                   settings.fadingMaxDamp(), settings.fadingRate(), this);

  _encoder = new MorseEncoder(settings.tone(), settings.tone()+settings.dashPitch(),
                              settings.speed(), settings.icPauseFactor(), settings.iwPauseFactor(),
                              settings.sound(), settings.jitter(), this);

  _audio_sink->setSource(_noiseEffect);
  _noiseEffect->setSource(_qrm);
  _qrm->setSource(_fadingEffect);
  _fadingEffect->setSource(_encoder);

  _decoder = new MorseDecoder(settings.speed(), std::pow(10.,settings.decoderLevel()/20), this);
  _audio_src = new QAudioSource(_decoder, this);

  _highscore = new HighScore(this);

  applySettings();

  // Connect singals
  connect(_encoder, SIGNAL(charSend(QChar)), this, SLOT(onCharSend(QChar)));

  connect(_decoder, SIGNAL(charReceived(QChar)), this, SLOT(onCharReceived(QChar)));
  connect(_decoder, SIGNAL(unknownCharReceived(QString)), this, SLOT(onUnknownCharReceived(QString)));

  connect(&_checkUpdate, SIGNAL(updateAvailable(QString)), this, SLOT(onUpdateAvailable(QString)));

#ifdef CHECK_FOR_UPDATES
  QDateTime lastUpdate = settings.lastCheckForUpdates();
  qDebug() << "Last check for updates" << lastUpdate;
  if (settings.checkForUpdates() && ((! lastUpdate.isValid()) || (lastUpdate.daysTo(QDateTime::currentDateTime()) > 7))) {
    _checkUpdate.check();
    settings.checkedForUpdates();
  }
#endif
}

Application::~Application() {
  if (_audio_src)
    _audio_src->stop();
}

void
Application::setVolume(double factor) {
  if (_audio_sink)
    _audio_sink->setVolume(factor);
}

QString
Application::summary() const {
  if (nullptr == _tutor)
    return "";
  return _tutor->summary();
}

Tutor *
Application::currentTutor() const {
  return _tutor;
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
  if (_tutor) {
    _tutor->stop();
    if (_audio_src && _tutor->needsDecoder())
      _audio_src->stop();
  }
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
  _encoder->setICPFactor(settings.icPauseFactor());
  _encoder->setIWPFactor(settings.iwPauseFactor());
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
                             settings.kochSummary(), settings.kochSuccessThreshold(), this);
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
    default:
      _tutor = new ChatTutor(_encoder, this);
      break;
  }

  connect(_tutor, SIGNAL(sessionFinished()), this, SIGNAL(sessionFinished()));
  connect(_tutor, SIGNAL(sessionVerified(const QString &, int, int)), this, SLOT(onSessionVerified(const QString &, int, int)));
  emit tutorChanged();
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

void
Application::onSessionVerified(const QString &tutor, int lesson, int score) {
  qDebug() << "Application: Session verified. Send highscore if enabled...";
  Settings settings;
  double wpm = settings.speed();
  double icp = double(settings.icPauseFactor())/100;
  double iwp = double(settings.icPauseFactor())/100;
  int ewpm = 50.*wpm/(35+10*icp+5*iwp);
  _highscore->emitScore(tutor, wpm, ewpm, lesson, score);
}

void
Application::onUpdateAvailable(QString version) {
  QString text("<b>%1</b> of KochMorse is available for download. "
               "You are runnnig version <b>%2.%3.%4</b>.<br><br>"
               "Head to <a href=\"https://github.com/hmatuschek/kochmorse/releases\">"
               "github.com/hmatuschek/kochmorse/releases</a> for download.");

  QMessageBox mbox;
  mbox.setText(tr("New version available."));
  mbox.setInformativeText(text.arg(version).arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_PATCH));
  mbox.setTextFormat(Qt::RichText);
  mbox.setTextInteractionFlags(Qt::TextBrowserInteraction);
  mbox.setIcon(QMessageBox::Information);
  mbox.setStandardButtons(QMessageBox::Close);
  mbox.setDefaultButton(QMessageBox::Close);

  mbox.exec();
}
