#include "settings.hh"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QGroupBox>
#include <QPushButton>
#include <QFileInfo>
#include <QFileDialog>
#include "morseencoder.hh"
#include "globals.hh"
#include <QDebug>
#include <QAudioDeviceInfo>
#include <QDesktopServices>


/* ********************************************************************************************* *
 * Settings object
 * ********************************************************************************************* */
Settings::Settings()
  : QSettings("com.github.hmatuschek", "KochMorse")
{
  // pass...
}

double
Settings::volume() const {
  return this->value("volume", 1).toDouble();
}
void
Settings::setVolume(double factor) {
  factor = std::max(0., std::min(factor, 2.));
  this->setValue("volume", factor);
}

bool
Settings::checkForUpdates() const {
  return value("checkUpdates", true).toBool();
}
void
Settings::setCheckForUpdates(bool enabled) {
  setValue("checkUpdates", enabled);
}

QDateTime
Settings::lastCheckForUpdates() const {
  return value("lastCheckUpdates", QDateTime()).value<QDateTime>();
}
void
Settings::checkedForUpdates() {
  setValue("lastCheckUpdates", QDateTime::currentDateTime());
}



int
Settings::speed() const {
  return this->value("speed", 20).toInt();
}
void
Settings::setSpeed(int speed) {
  speed = std::max(5, std::min(speed, 100));
  this->setValue("speed", speed);
}

int
Settings::effSpeed() const {
  return this->value("effSpeed", 15).toInt();
}
void
Settings::setEffSpeed(int speed) {
  speed = std::max(5, std::min(speed, 100));
  this->setValue("effSpeed", speed);
}

int
Settings::tone() const {
  return this->value("tone", 750).toInt();
}
void
Settings::setTone(int freq) {
  freq = std::max(20, std::min(freq, 20000));
  this->setValue("tone", freq);
}

int
Settings::dashPitch() const {
  return this->value("dashPitch", 0).toInt();
}
void
Settings::setDashPitch(int pitch) {
  pitch = std::max(-1000, std::min(pitch, 1000));
  this->setValue("dashPitch", pitch);
}

MorseEncoder::Sound
Settings::sound() const {
  return MorseEncoder::Sound(this->value("sound", MorseEncoder::SOUND_SOFT).toUInt());
}
void
Settings::setSound(MorseEncoder::Sound sound) {
  this->setValue("sound", uint(sound));
}

MorseEncoder::Jitter
Settings::jitter() const {
  return MorseEncoder::Jitter(this->value("jitter", MorseEncoder::JITTER_EXACT).toUInt());
}
void
Settings::setJitter(MorseEncoder::Jitter jitter) {
  this->setValue("jitter", uint(jitter));
}

double
Settings::decoderLevel() const {
  return this->value("decoderlevel", 0).toDouble();
}
void
Settings::setDecoderLevel(double level) {
  this->setValue("decoderlevel", level);
}

QAudioDeviceInfo
Settings::outputDevice() const {
  QAudioDeviceInfo def(QAudioDeviceInfo::defaultOutputDevice());
  QList<QAudioDeviceInfo> devices(QAudioDeviceInfo::availableDevices(QAudio::AudioOutput));
  QString devname = this->value("output", def.deviceName()).toString();
  foreach (auto dev, devices) {
    if (dev.deviceName() == devname)
      return dev;
  }
  return def;
}
void
Settings::setOutputDevice(const QString &devicename) {
  this->setValue("output", devicename);
}

QAudioDeviceInfo
Settings::inputDevice() const {
  QAudioDeviceInfo def(QAudioDeviceInfo::defaultInputDevice());
  QList<QAudioDeviceInfo> devices(QAudioDeviceInfo::availableDevices(QAudio::AudioInput));
  QString devname = this->value("input", def.deviceName()).toString();
  foreach (auto dev, devices) {
    if (dev.deviceName() == devname)
      return dev;
  }
  return def;
}
void
Settings::setInputDevice(const QString &devicename) {
  this->setValue("input", devicename);
}

Settings::Tutor
Settings::tutor() const {
  return Tutor(this->value("tutor", TUTOR_KOCH).toUInt());
}
void
Settings::setTutor(Tutor tutor) {
  this->setValue("tutor", uint(tutor));
}

int
Settings::kochLesson() const {
  return this->value("koch/lesson", 2).toInt();
}
void
Settings::setKochLesson(int n) {
  n = std::max(2, std::min(n, 43));
  this->setValue("koch/lesson", n);
}

bool
Settings::kochPrefLastChars() const {
  return this->value("koch/prefLastChars", false).toBool();
}

void
Settings::setKochPrefLastChars(bool pref) {
  this->setValue("koch/prefLastChars", pref);
}

void Settings::setKochLastCharFrequencyFactor(int lastCharFrequencyFactor) {
  this->setValue("koch/lastCharFrequencyFactor", lastCharFrequencyFactor);
}

int Settings::KochLastCharFrequencyFactor() const {
  return this->value("koch/lastCharFrequencyFactor", 1).toInt();
}

bool
Settings::kochRepeatLastChar() const {
  return this->value("koch/repeatLastChar", false).toBool();
}

void
Settings::setKochRepeatLastChar(bool enable) {
  this->setValue("koch/repeatLastChar", enable);
}

int
Settings::kochMinGroupSize() const {
  return this->value("koch/minGroupSize", 5).toInt();
}

void
Settings::setKochMinGroupSize(int size) {
  this->setValue("koch/minGroupSize", size);
}

int
Settings::kochMaxGroupSize() const {
  return this->value("koch/maxGroupSize", 5).toInt();
}

void
Settings::setKochMaxGroupSize(int size) {
  this->setValue("koch/maxGroupSize", size);
}

bool
Settings::kochInfiniteLineCount() const {
  return this->value("koch/infinite", false).toBool();
}

void
Settings::setKochInifiniteLineCount(bool enable) {
  this->setValue("koch/infinite", enable);
}

int
Settings::kochLineCount() const {
  return this->value("koch/linecount", 5).toInt();
}
void
Settings::setKochLineCount(int lines) {
  this->setValue("koch/linecount", lines);
}

bool
Settings::kochSummary() const {
  return this->value("koch/summary", true).toBool();
}

void
Settings::setKochSummary(bool show) {
  this->setValue("koch/summary", show);
}

int
Settings::kochSuccessThreshold() const {
  return this->value("koch/thres", 90).toInt();
}

void
Settings::setKochSuccessThreshold(int thres) {
  this->setValue("koch/thres", thres);
}


QSet<QChar>
Settings::randomChars() const {
  QSet<QChar> chars;
  if (! this->contains("random/chars")) {
    chars << 'a' << 'b' << 'c' << 'd' << 'e' << 'f' << 'g' << 'h' << 'i' << 'j' << 'k' << 'l' << 'm'
          << 'n' << 'o' << 'p' << 'q' << 'r' << 's' << 't' << 'u' << 'v' << 'w' << 'x' << 'y' << 'z'
          << '0' << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8' << '9' << '.' << ',' << '?'
          << '/' << '&' << ':' << ';' << '=' << '+' << '-' << '@' << '(' << ')'
          << QChar(0x017a) /*ź*/ << QChar(0x00e4) /*ä*/ << QChar(0x0105) /*ą*/ << QChar(0x00f6) /*ö*/
          << QChar(0x00f8) /*ø*/ << QChar(0x00f3) /*ó*/ << QChar(0x00fc) /*ü*/ << QChar(0x016d) /*ŭ*/
          << QChar(0x03c7) /*χ*/ << QChar(0x0125) /*ĥ*/ << QChar(0x00e0) /*à*/ << QChar(0x00e5) /*å*/
          << QChar(0x00e8) /*è*/ << QChar(0x00e9) /*é*/ << QChar(0x0109) /*ę*/ << QChar(0x00f0) /*ð*/
          << QChar(0x00de) /*þ*/ << QChar(0x0109) /*ĉ*/ << QChar(0x0107) /*ć*/ << QChar(0x011d) /*ĝ*/
          << QChar(0x0125) /*ĵ*/ << QChar(0x015d) /*ŝ*/ << QChar(0x0142) /*ł*/ << QChar(0x0144) /*ń*/
          << QChar(0x00f1) /*ñ*/ << QChar(0x0107) /*ż*/ << QChar(0x00bf) /*¿*/ << QChar(0x00a1) /*¡*/
          << QChar(0x00df) /*ß*/ << QChar(0x0144) /*ś*/
          << QChar(0x2417) /* BK */ << QChar(0x2404) /* CL */ << QChar(0x2403) /* SK */
          << QChar(0x2406) /* SN */;
  } else {
    QString str = this->value("random/chars").toString();
    for (int i=0; i<str.size(); i++)
      chars.insert(str[i]);
  }
  return chars;
}

void
Settings::setRandomChars(const QSet<QChar> &chars) {
  QString str;
  QSet<QChar>::const_iterator c = chars.begin();
  for (; c != chars.end(); c++) { str.append(*c); }
  this->setValue("random/chars", str);
}

void
Settings::setRandomMinGroupSize(int size) {
  this->setValue("random/minGroupSize", size);
}

int
Settings::randomMinGroupSize() const {
  return this->value("random/minGroupSize", 5).toInt();
}

void
Settings::setRandomMaxGroupSize(int size) {
  this->setValue("random/maxGroupSize", size);
}

int
Settings::randomMaxGroupSize() const {
  return this->value("random/maxGroupSize", 5).toInt();
}

bool
Settings::randomInfiniteLineCount() const {
  return this->value("random/infinite", false).toBool();
}

void
Settings::setRandomInifiniteLineCount(bool enable) {
  this->setValue("random/infinite", enable);
}

int
Settings::randomLineCount() const {
  return this->value("random/linecount", 5).toInt();
}

void
Settings::setRandomLineCount(int lines) {
  this->setValue("random/linecount", lines);
}

bool
Settings::randomSummary() const {
  return this->value("random/summary", false).toBool();
}

void
Settings::setRandomSummary(bool show) {
  this->setValue("random/summary", show);
}

QString
Settings::textGenFilename() const {
  return this->value("textgen/filename").toString();
}

void
Settings::setTextGenFilename(const QString &filename) {
  this->setValue("textgen/filename", filename);
}

bool
Settings::noiseEnabled() const {
  return value("noise/enabled", false).toBool();
}
void
Settings::setNoiseEnabled(bool enabled) {
  setValue("noise/enabled", enabled);
}

float
Settings::noiseSNR() const {
  return value("noise/SNR", 10.0).toFloat();
}
void
Settings::setNoiseSNR(float snr) {
  snr = std::max(-60.f, std::min(snr, 60.f));
  setValue("noise/SNR", snr);
}

bool
Settings::noiseFilterEnabled() const {
  return value("noise/filter", false).toBool();
}
void
Settings::setNoiseFilterEnabled(bool enabled) {
  setValue("noise/filter", enabled);
}

float
Settings::noiseFilterBw() const {
  return value("noise/bw", 300).toFloat();
}
void
Settings::setNoiseFilterBw(float Bw) {
  setValue("noise/bw", Bw);
}

bool
Settings::fadingEnabled() const {
  return value("fading/enabled", false).toBool();
}
void
Settings::setFadingEnabled(bool enabled) {
  setValue("fading/enabled", enabled);
}

float
Settings::fadingRate() const {
  return value("fading/rate", 12).toFloat();
}
void
Settings::setFadingRate(float rate) {
  rate = std::max(1.0f, std::min(rate, 60.0f));
  setValue("fading/rate", rate);
}

float
Settings::fadingMaxDamp() const {
  return value("fading/maxDamp", -10).toFloat();
}
void
Settings::setFadingMaxDamp(float damp) {
  damp = std::max(-60.0f, std::min(damp, 0.0f));
  setValue("fading/maxDamp", damp);
}

bool
Settings::qrmEnabled() const {
  return value("qrm/enabled", false).toBool();
}
void
Settings::setQRMEnabled(bool enabled) {
  setValue("qrm/enabled", enabled);
}

int
Settings::qrmStations() const {
  return std::max(0, value("qrm/stations", 3).toInt());
}
void
Settings::setQRMStations(int num) {
  setValue("qrm/stations", num);
}

double
Settings::qrmSNR() const {
  return std::max(0., value("qrm/snr", 3).toDouble());
}
void
Settings::setQRMSNR(double db) {
  setValue("qrm/snr", db);
}


/* ********************************************************************************************* *
 * Settings Dialog
 * ********************************************************************************************* */
SettingsDialog::SettingsDialog(QWidget *parent)
  : QDialog(parent)
{
  this->setModal(true);

  _tutor = new TutorSettingsView();
  _code  = new CodeSettingsView();
  _effects = new EffectSettingsView();
  _devices = new DeviceSettingsView();

  _tabs = new QTabWidget();
  _tabs->addTab(_tutor, tr("Tutor"));
  _tabs->addTab(_code, tr("Morse Code"));
  _tabs->addTab(_effects, tr("Effects"));
  _tabs->addTab(_devices, tr("Devices"));

  // populating device settings may takes a lot of time so only do if the tab becomes visible
  QObject::connect(_tabs, &QTabWidget::currentChanged, [this](){
    if(_tabs->currentIndex()==3) {
        _devices->populateDeviceSettingsView();
    }
  });


  QDialogButtonBox *bbox = new QDialogButtonBox();
  bbox->addButton(QDialogButtonBox::Ok);
  bbox->addButton(QDialogButtonBox::Cancel);
  bbox->addButton(QDialogButtonBox::Help);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(_tabs);
  layout->addWidget(bbox);
  this->setLayout(layout);

  QObject::connect(bbox, SIGNAL(accepted()), this, SLOT(accept()));
  QObject::connect(bbox, SIGNAL(rejected()), this, SLOT(reject()));
  QObject::connect(bbox, SIGNAL(helpRequested()), this, SLOT(showHelp()));
}

void
SettingsDialog::accept() {
  _tutor->save();
  _code->save();
  _effects->save();
  _devices->save();
  QDialog::accept();
}

void
SettingsDialog::showHelp() {
  QDesktopServices::openUrl(tr("https://github.com/hmatuschek/kochmorse/wiki/Settings"));
}


/* ********************************************************************************************* *
 * CodeSettings Widget
 * ********************************************************************************************* */
CodeSettingsView::CodeSettingsView(QWidget *parent)
  : QWidget(parent)
{
  Settings settings;

  _speed = new QSpinBox();
  _speed->setMinimum(5); _speed->setMaximum(100);
  _speed->setValue(settings.speed());
  _speed->setToolTip(tr("Specifies the speed (in WPM) for the symbols."));

  _effSpeed = new QSpinBox();
  _effSpeed->setMinimum(5); _speed->setMaximum(100);
  _effSpeed->setValue(settings.effSpeed());
  _effSpeed->setToolTip(tr("Specifies the pause lengths between symbols and words."));

  _tone = new QLineEdit(QString::number(settings.tone()));
  QIntValidator *tone_val = new QIntValidator(20,20000);
  _tone->setValidator(tone_val);
  _tone->setToolTip(tr("Specifies the freqency (in Hz) of the CW tone."));

  _daPitch = new QLineEdit(QString::number(settings.dashPitch()));
  QIntValidator *pitch_val = new QIntValidator(-1000,1000);
  _daPitch->setValidator(pitch_val);
  _daPitch->setToolTip(tr("Specifies the freqency offset (in Hz) for the 'dah' tone."));

  _sound = new QComboBox();
  _sound->addItem(tr("Soft"), uint(MorseEncoder::SOUND_SOFT));
  _sound->addItem(tr("Sharp"), uint(MorseEncoder::SOUND_SHARP));
  _sound->addItem(tr("Cracking"), uint(MorseEncoder::SOUND_CRACKING));
  _sound->setToolTip(tr("Selects the sound 'sharpness' for the CW tone."));
  switch (settings.sound()) {
    case MorseEncoder::SOUND_SOFT: _sound->setCurrentIndex(0); break;
    case MorseEncoder::SOUND_SHARP: _sound->setCurrentIndex(1); break;
    case MorseEncoder::SOUND_CRACKING: _sound->setCurrentIndex(2); break;
  }

  _jitter = new QComboBox();
  _jitter->addItem(tr("Exact"), uint(MorseEncoder::JITTER_EXACT));
  _jitter->addItem(tr("Bug"), uint(MorseEncoder::JITTER_BUG));
  _jitter->addItem(tr("Straight"), uint(MorseEncoder::JITTER_STRAIGHT));
  _jitter->setToolTip(tr("Selects the timeing jitter to simulate a semi-automatic key (bug) "
                         "or a straight key."));
  switch (settings.jitter()) {
    case MorseEncoder::JITTER_EXACT: _jitter->setCurrentIndex(0); break;
    case MorseEncoder::JITTER_BUG: _jitter->setCurrentIndex(1); break;
    case MorseEncoder::JITTER_STRAIGHT: _jitter->setCurrentIndex(2); break;
  }

  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("Speed (WPM)"), _speed);
  layout->addRow(tr("Eff. speed (WPM)"), _effSpeed);
  layout->addRow(tr("Tone (Hz)"), _tone);
  layout->addRow(tr("Dash pitch (Hz)"), _daPitch);
  layout->addRow(tr("Sound"), _sound);
  layout->addRow(tr("Jitter"), _jitter);

  this->setLayout(layout);
}

void
CodeSettingsView::save() {
  Settings settings;
  settings.setSpeed(_speed->value());
  settings.setEffSpeed(_effSpeed->value());
  settings.setTone(_tone->text().toInt());
  settings.setDashPitch(_daPitch->text().toInt());
  settings.setSound(MorseEncoder::Sound(_sound->currentIndex()));
  settings.setJitter(MorseEncoder::Jitter(_jitter->currentIndex()));
}


/* ********************************************************************************************* *
 * Tutor Settings Widget
 * ********************************************************************************************* */
TutorSettingsView::TutorSettingsView(QWidget *parent)
  : QWidget(parent)
{
  _tutor = new QComboBox();
  _tutor->setToolTip(tr("Select a tutor. If you learn the code, start with 'Koch method'."));
  _tutor->addItem(tr("Koch method"));
  _tutor->addItem(tr("Random"));
  _tutor->addItem(tr("Rule based tutor"));
  _tutor->addItem(tr("Transmit"));
  _tutor->addItem(tr("QSO Chat"));

  _kochSettings = new KochTutorSettingsView();
  _randSettings = new RandomTutorSettingsView();
  _textgetSettings = new TextGenTutorSettingsView();
  _txSettings   = new TXTutorSettingsView();
  _chatSettings = new ChatTutorSettingsView();

  _tutorSettings = new QStackedWidget();
  _tutorSettings->addWidget(_kochSettings);
  _tutorSettings->addWidget(_randSettings);
  _tutorSettings->addWidget(_textgetSettings);
  _tutorSettings->addWidget(_txSettings);
  _tutorSettings->addWidget(_chatSettings);

  Settings settings;
  if (Settings::TUTOR_KOCH == settings.tutor()) {
    _tutor->setCurrentIndex(0);
    _tutorSettings->setCurrentIndex(0);
  } else if (Settings::TUTOR_RANDOM == settings.tutor()) {
    _tutor->setCurrentIndex(1);
    _tutorSettings->setCurrentIndex(1);
  } else if (Settings::TUTOR_TEXTGEN == settings.tutor()) {
    _tutor->setCurrentIndex(2);
    _tutorSettings->setCurrentIndex(2);
  } else if (Settings::TUTOR_TX == settings.tutor()) {
    _tutor->setCurrentIndex(3);
    _tutorSettings->setCurrentIndex(3);
  } else if (Settings::TUTOR_CHAT == settings.tutor()) {
    _tutor->setCurrentIndex(4);
    _tutorSettings->setCurrentIndex(4);
  }

  QFormLayout *sel = new QFormLayout();
  sel->addRow(tr("Current tutor"), _tutor);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(sel);
  layout->addWidget(_tutorSettings);
  this->setLayout(layout);

  QObject::connect(_tutor, SIGNAL(currentIndexChanged(int)), this, SLOT(onTutorSelected(int)));
}

void
TutorSettingsView::onTutorSelected(int idx) {
  _tutorSettings->setCurrentIndex(idx);
}

void
TutorSettingsView::save() {
  // Save all tutors
  _kochSettings->save();
  _randSettings->save();
  _textgetSettings->save();

  // Get tutor by index
  Settings settings;
  if (0 == _tutor->currentIndex()) {
    settings.setTutor(Settings::TUTOR_KOCH);
  } else if (1 == _tutor->currentIndex()) {
    settings.setTutor(Settings::TUTOR_RANDOM);
  } else if (2 == _tutor->currentIndex()) {
    settings.setTutor(Settings::TUTOR_TEXTGEN);
  } else if (3 == _tutor->currentIndex()) {
    settings.setTutor(Settings::TUTOR_TX);
  } else if (4 == _tutor->currentIndex()) {
    settings.setTutor(Settings::TUTOR_CHAT);
  }
}


/* ********************************************************************************************* *
 * Koch Tutor Settings Widget
 * ********************************************************************************************* */
KochTutorSettingsView::KochTutorSettingsView(QWidget *parent)
  : QGroupBox(tr("Koch tutor settings"), parent)
{
  Settings settings;

  _lesson = new QSpinBox();
  _lesson->setMinimum(2); _lesson->setMaximum(43);
  _lesson->setValue(settings.kochLesson());
  _lesson->setToolTip(tr("Specifies the number of symbols for this lesson."));

  _prefLastChars = new QCheckBox();
  _prefLastChars->setChecked(settings.kochPrefLastChars());
  _prefLastChars->setToolTip(tr("If enabled, increases the likelihood of newer symbols."));

  _lastCharFrequencyFactor = new QSpinBox();
  _lastCharFrequencyFactor->setMinimum(1); _lastCharFrequencyFactor->setMaximum(4);
  _lastCharFrequencyFactor->setValue(settings.KochLastCharFrequencyFactor());
  _lastCharFrequencyFactor->setToolTip(tr("Influence the increase in the likelihood of newer symbols."));

  _repLastChar = new QCheckBox();
  _repLastChar->setChecked(settings.kochRepeatLastChar());
  _repLastChar->setToolTip(tr("If enabled, repeats the new symbol before the lesson starts."));

  _minGroupSize = new QSpinBox();
  _minGroupSize->setValue(settings.kochMinGroupSize());
  _minGroupSize->setMinimum(1);
  _minGroupSize->setMaximum(settings.kochMaxGroupSize());
  _minGroupSize->setToolTip(tr("Specifies the minimum group size."));

  _maxGroupSize = new QSpinBox();
  _maxGroupSize->setValue(settings.kochMaxGroupSize());
  _maxGroupSize->setMinimum(settings.kochMinGroupSize());
  _maxGroupSize->setMaximum(20);
  _maxGroupSize->setToolTip(tr("Specifies the maximum group size. If equal to minimum group size, a fixed size is implied."));

  _infinite = new QCheckBox();
  _infinite->setChecked(settings.kochInfiniteLineCount());
  _infinite->setToolTip(tr("Sends an infinite number of lines."));

  _lineCount = new QSpinBox();
  _lineCount->setMinimum(1);
  _lineCount->setValue(settings.kochLineCount());
  _lineCount->setToolTip(tr("Specifies the number of lines to send."));
  if (settings.kochInfiniteLineCount())
    _lineCount->setEnabled(false);

  _summary = new QCheckBox();
  _summary->setChecked(settings.kochSummary());
  _summary->setToolTip(tr("If enabled, shows a summary statistics at the end."));
  if (settings.kochInfiniteLineCount())
    _summary->setEnabled(false);

  _threshold = new QSpinBox();
  _threshold->setMinimum(75);
  _threshold->setMaximum(100);
  _threshold->setValue(settings.kochSuccessThreshold());
  _threshold->setToolTip(tr("Specifies the success rate at which the lesson is completed."));
  _threshold->setEnabled(_summary->isEnabled() && _summary->isChecked());

  // Cross connect min and max group size splin boxes to maintain
  // consistent settings
  connect(_minGroupSize, SIGNAL(valueChanged(int)), this, SLOT(onMinSet(int)));
  connect(_maxGroupSize, SIGNAL(valueChanged(int)), this, SLOT(onMaxSet(int)));
  connect(_infinite, SIGNAL(toggled(bool)), this, SLOT(onInfiniteToggled(bool)));
  connect(_summary, SIGNAL(toggled(bool)), this, SLOT(onShowSummaryToggled(bool)));

  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("Lesson"), _lesson);
  layout->addRow(tr("Prefer last chars"), _prefLastChars);
  layout->addRow(tr("Set last char frequency factor"), _lastCharFrequencyFactor);
  layout->addRow(tr("Repeat last char"), _repLastChar);
  layout->addRow(tr("Min. group size"), _minGroupSize);
  layout->addRow(tr("Max. group size"), _maxGroupSize);
  layout->addRow(tr("Infinite lines"), _infinite);
  layout->addRow(tr("Line count"), _lineCount);
  layout->addRow(tr("Show summary"), _summary);
  layout->addRow(tr("Lesson target"), _threshold);

  this->setLayout(layout);
}

void
KochTutorSettingsView::save() {
  Settings settings;
  settings.setKochLesson(_lesson->value());
  settings.setKochPrefLastChars(_prefLastChars->isChecked());
  settings.setKochLastCharFrequencyFactor(_lastCharFrequencyFactor->value());
  settings.setKochRepeatLastChar(_repLastChar->isChecked());
  settings.setKochMinGroupSize(_minGroupSize->value());
  settings.setKochMaxGroupSize(_maxGroupSize->value());
  settings.setKochInifiniteLineCount(_infinite->isChecked());
  settings.setKochLineCount(_lineCount->value());
  settings.setKochSummary(_summary->isChecked());
  settings.setKochSuccessThreshold(_threshold->value());
}

void
KochTutorSettingsView::onMinSet(int value) {
  _maxGroupSize->setMinimum(value);
}

void
KochTutorSettingsView::onMaxSet(int value) {
  _minGroupSize->setMaximum(value);
}

void
KochTutorSettingsView::onInfiniteToggled(bool enabled) {
  _lineCount->setEnabled(! enabled);
  _summary->setEnabled(! enabled);
  _threshold->setEnabled((! enabled) && _summary->isChecked());
}

void
KochTutorSettingsView::onShowSummaryToggled(bool enabled) {
  _threshold->setEnabled(enabled);
}


/* ********************************************************************************************* *
 * Random Tutor Settings Widget
 * ********************************************************************************************* */
RandomTutorSettingsView::RandomTutorSettingsView(QWidget *parent)
  : QGroupBox(tr("Random tutor settings"),parent)
{
  Settings settings;

  QSet<QChar> enabled_chars = settings.randomChars();
  QList<QChar> alpha, num, punct, prosign, special;
  alpha << 'a' << 'b' << 'c' << 'd' << 'e' << 'f' << 'g' << 'h' << 'i' << 'j' << 'k' << 'l' << 'm'
        << 'n' << 'o' << 'p' << 'q' << 'r' << 's' << 't' << 'u' << 'v' << 'w' << 'x' << 'y' << 'z';
  num << '0' << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8' << '9';
  punct << '.' << ',' << ':' << ';' << '?' << '-' << '@' << '(' << ')' << '/';
  prosign << '=' << '+' << '&' << QChar(0x2417) /* BK */ << QChar(0x2404) /* CL */
          << QChar(0x2403) /* SK */ << QChar(0x2406) /* SN */ << QChar(0x2407) /* KL */;
  special << QChar(0x017a) /*ź*/ << QChar(0x00e4) /*ä*/ << QChar(0x0105) /*ą*/ << QChar(0x00f6) /*ö*/
          << QChar(0x00f8) /*ø*/ << QChar(0x00f3) /*ó*/ << QChar(0x00fc) /*ü*/ << QChar(0x016d) /*ŭ*/
          << QChar(0x03c7) /*χ*/ << QChar(0x0125) /*ĥ*/ << QChar(0x00e0) /*à*/ << QChar(0x00e5) /*å*/
          << QChar(0x00e8) /*è*/ << QChar(0x00e9) /*é*/ << QChar(0x0109) /*ę*/ << QChar(0x00f0) /*ð*/
          << QChar(0x00de) /*þ*/ << QChar(0x0109) /*ĉ*/ << QChar(0x0107) /*ć*/ << QChar(0x011d) /*ĝ*/
          << QChar(0x0125) /*ĵ*/ << QChar(0x015d) /*ŝ*/ << QChar(0x0142) /*ł*/ << QChar(0x0144) /*ń*/
          << QChar(0x00f1) /*ñ*/ << QChar(0x0107) /*ż*/ << QChar(0x00bf) /*¿*/ << QChar(0x00a1) /*¡*/
          << QChar(0x00df) /*ß*/ << QChar(0x0144) /*ś*/;

  // Assemble char table
  _alpha = new ListWidget();
  foreach (QChar c, alpha) {
    QString code = Globals::charTable[c]; code.replace('.',QChar(0x00b7)).replace("-",QChar(0x2212));
    QString txt = QString("%1 (%2)").arg(c).arg(code);
    _alpha->addItem(txt, c, enabled_chars.contains(c));
  }
  // Assemble char table
  _num = new ListWidget();
  foreach (QChar c, num) {
    QString code = Globals::charTable[c]; code.replace('.',QChar(0x00b7)).replace("-",QChar(0x2212));
    QString txt = QString("%1 (%2)").arg(c).arg(code);
    _num->addItem(txt, c, enabled_chars.contains(c));
  }
  // Assemble punc table
  _punct = new ListWidget();
  foreach (QChar c, punct) {
    QString code = Globals::charTable[c]; code.replace('.',QChar(0x00b7)).replace("-",QChar(0x2212));
    QString txt = QString("%1 (%2)").arg(c).arg(code);
    _punct->addItem(txt, c, enabled_chars.contains(c));
  }
  // Assemble prosig table
  _prosign = new ListWidget();
  foreach (QChar c, prosign) {
    QString code = Globals::charTable[c]; code.replace('.',QChar(0x00b7)).replace("-",QChar(0x2212));
    QString txt = QString("%1 (%2)").arg(c).arg(code);
    _prosign->addItem(txt, c, enabled_chars.contains(c));
  }
  // Assemble special table
  _special = new ListWidget();
  foreach (QChar c, special) {
    QString code = Globals::charTable[c]; code.replace('.',QChar(0x00b7)).replace("-",QChar(0x2212));
    QString txt = QString("%1 (%2)").arg(c).arg(code);
    _special->addItem(txt, c, enabled_chars.contains(c));
  }

  QTabWidget *tabs = new QTabWidget();
  tabs->addTab(_alpha, tr("Characters"));
  tabs->addTab(_num, tr("Numbers"));
  tabs->addTab(_punct, tr("Punctuations"));
  tabs->addTab(_prosign, tr("Prosigns"));
  tabs->addTab(_special, tr("Special"));

  _minGroupSize = new QSpinBox();
  _minGroupSize->setValue(settings.randomMinGroupSize());
  _minGroupSize->setMinimum(1);
  _minGroupSize->setMaximum(settings.randomMaxGroupSize());
  _minGroupSize->setToolTip(tr("Specifies the minimum group size."));

  _maxGroupSize = new QSpinBox();
  _maxGroupSize->setValue(settings.randomMaxGroupSize());
  _maxGroupSize->setMinimum(settings.randomMinGroupSize());
  _maxGroupSize->setMaximum(20);
  _maxGroupSize->setToolTip(tr("Specifies the maximum group size. If equal to minimum group size, a fixed size is implied."));

  _infinite = new QCheckBox();
  _infinite->setChecked(settings.randomInfiniteLineCount());
  _infinite->setToolTip(tr("Sends an infinite number of lines."));

  _lineCount = new QSpinBox();
  _lineCount->setMinimum(1);
  _lineCount->setValue(settings.randomLineCount());
  _lineCount->setToolTip(tr("Specifies the number of lines to send."));
  if (settings.randomInfiniteLineCount())
    _lineCount->setEnabled(false);

  _summary = new QCheckBox();
  _summary->setChecked(settings.randomSummary());
  _summary->setToolTip(tr("If enabled, shows a summary statistics at the end."));
  if (settings.randomInfiniteLineCount())
    _summary->setEnabled(false);

  // Cross connect min and max group size splin boxes to maintain
  // consistent settings
  connect(_minGroupSize, SIGNAL(valueChanged(int)), this, SLOT(onMinSet(int)));
  connect(_maxGroupSize, SIGNAL(valueChanged(int)), this, SLOT(onMaxSet(int)));
  connect(_infinite, SIGNAL(toggled(bool)), this, SLOT(onInfiniteToggled(bool)));

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(tabs);
  QFormLayout *box = new QFormLayout();
  box->addRow(tr("Min. group size"), _minGroupSize);
  box->addRow(tr("Max. group size"), _maxGroupSize);
  box->addRow(tr("Infinite lines"), _infinite);
  box->addRow(tr("Line count"), _lineCount);
  box->addRow(tr("Show summary"), _summary);

  layout->addLayout(box);
  setLayout(layout);
}

void
RandomTutorSettingsView::save() {
  QSet<QChar> enabled_chars;
  for (int i=0; i<_alpha->numRows(); i++) {
    if (_alpha->isChecked(i)) {
      enabled_chars.insert(_alpha->itemData(i).toChar());
    }
  }
  for (int i=0; i<_num->numRows(); i++) {
    if (_num->isChecked(i)) {
      enabled_chars.insert(_num->itemData(i).toChar());
    }
  }
  for (int i=0; i<_punct->numRows(); i++) {
    if (_punct->isChecked(i)) {
      enabled_chars.insert(_punct->itemData(i).toChar());
    }
  }
  for (int i=0; i<_prosign->numRows(); i++) {
    if (_prosign->isChecked(i)) {
      enabled_chars.insert(_prosign->itemData(i).toChar());
    }
  }
  for (int i=0; i<_special->numRows(); i++) {
    if (_special->isChecked(i)) {
      enabled_chars.insert(_special->itemData(i).toChar());
    }
  }

  Settings().setRandomChars(enabled_chars);
  Settings().setRandomMinGroupSize(_minGroupSize->value());
  Settings().setRandomMaxGroupSize(_maxGroupSize->value());
  Settings().setRandomInifiniteLineCount(_infinite->isChecked());
  Settings().setRandomLineCount(_lineCount->value());
  Settings().setRandomSummary(_summary->isChecked());
}

void
RandomTutorSettingsView::onMinSet(int value) {
  _maxGroupSize->setMinimum(value);
}

void
RandomTutorSettingsView::onMaxSet(int value) {
  _minGroupSize->setMaximum(value);
}

void
RandomTutorSettingsView::onInfiniteToggled(bool enabled) {
  _lineCount->setEnabled(! enabled);
  _summary->setEnabled(! enabled);
}


/* ********************************************************************************************* *
 * TextGen Tutor Settings Widget
 * ********************************************************************************************* */
TextGenTutorSettingsView::TextGenTutorSettingsView(QWidget *parent)
  : QGroupBox(tr("Rule based tutor settings"),parent)
{
  Settings settings;
  QString help = tr("Select a build-in tutor or 'user defined'. The rule file (ending on .xml) or a "
                    "plain-text file (ending on .txt) can then be selected below.");

  _defined = new QComboBox();
  _defined->addItem(tr("Generated QSO"), ":/qso/qsogen.xml");
  _defined->addItem(tr("Generated QSO [german]"), ":/qso/qsogen-de.xml");
  _defined->addItem(tr("Q-Codes/Words"), ":/qso/qcodes.xml");
  _defined->addItem(tr("Q-Codes/Words [german]"), ":/qso/qcodes-de.xml");
  _defined->addItem(tr("Call signs"),    ":/qso/callsigns.xml");
  _defined->addItem(tr("User defined ..."));

  _filename = new QLineEdit();
  _filename->setToolTip(help);
  _filename->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  _filename->setEnabled(false);

  _selectFile = new QPushButton("...");
  _selectFile->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  _selectFile->setEnabled(false);

  QString selected = settings.textGenFilename();
  if (":/qso/qsogen.xml" == selected) {
    _defined->setCurrentIndex(0);
  } else if (":/qso/qsogen-de.xml" == selected) {
    _defined->setCurrentIndex(1);
  } else if (":/qso/qcodes.xml" == selected) {
    _defined->setCurrentIndex(2);
  } else if (":/qso/qcodes-de.xml" == selected) {
    _defined->setCurrentIndex(3);
  } else if (":/qso/callsigns.xml" == selected) {
    _defined->setCurrentIndex(4);
  } else {
    _defined->setCurrentIndex(5);
    _filename->setText(selected);
    _filename->setEnabled(true);
    _selectFile->setEnabled(true);
  }


  QHBoxLayout *llayout = new QHBoxLayout();
  llayout->addWidget(_filename, 1);
  llayout->addWidget(_selectFile);

  QFormLayout *layout = new QFormLayout();
  QLabel *label = new QLabel(help);
  label->setWordWrap(true);
  layout->addWidget(label);
  layout->addRow(tr("Build-in tutor"), _defined);
  layout->addRow(tr("Rule/text file"), llayout);
  layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
  setLayout(layout);

  connect(_defined, SIGNAL(currentIndexChanged(int)), this, SLOT(onPreDefinedSelected(int)));
  connect(_selectFile, SIGNAL(clicked(bool)), this, SLOT(onSelectFile()));
}

void
TextGenTutorSettingsView::save() {
  if (_filename->isEnabled()) {
    QFileInfo info(_filename->text());
    if (info.exists())
      Settings().setTextGenFilename(info.absoluteFilePath());
  } else {
    QFileInfo info(_defined->currentData().toString());
    if (info.exists())
      Settings().setTextGenFilename(info.absoluteFilePath());
  }
}

void
TextGenTutorSettingsView::onSelectFile() {
  QFileInfo info(_filename->text());
  QString filename = QFileDialog::getOpenFileName(this, tr("Select rule or text file."),
                                                  info.absoluteDir().path(),
                                                  tr("Rule file (*.xml);;Text file (*.txt)"));
  if (! filename.isEmpty())
    _filename->setText(filename);
}

void
TextGenTutorSettingsView::onPreDefinedSelected(int idx) {
  if (3 == idx) {
    _filename->setEnabled(true);
    _selectFile->setEnabled(true);
  } else {
    _filename->setEnabled(false);
    _selectFile->setEnabled(false);
  }
}


/* ********************************************************************************************* *
 * TX Tutor Settings Widget
 * ********************************************************************************************* */
TXTutorSettingsView::TXTutorSettingsView(QWidget *parent)
  : QGroupBox(tr("Transmit tutor settings"),parent)
{
  QLabel *label = new QLabel(tr("<No settings for this tutor>"));
  label->setAlignment(Qt::AlignCenter);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(label);
  setLayout(layout);
}

void
TXTutorSettingsView::save() {
  // pass...
}


/* ********************************************************************************************* *
 * Chat Tutor Settings Widget
 * ********************************************************************************************* */
ChatTutorSettingsView::ChatTutorSettingsView(QWidget *parent)
  : QGroupBox(tr("QSO Chat tutor settings"),parent)
{
  QLabel *label = new QLabel(tr("<No settings for this tutor>"));
  label->setAlignment(Qt::AlignCenter);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(label);
  setLayout(layout);
}

void
ChatTutorSettingsView::save() {
  // pass...
}


/* ********************************************************************************************* *
 * Effect Settings Widget
 * ********************************************************************************************* */
EffectSettingsView::EffectSettingsView(QWidget *parent)
  : QWidget(parent), _noiseEnabled(nullptr), _noiseSNR(nullptr)
{
  Settings settings;

  _noiseEnabled = new QCheckBox();
  _noiseEnabled->setChecked(settings.noiseEnabled());
  _noiseEnabled->setToolTip(tr("Enables the noise effect."));

  _noiseSNR = new QSpinBox();
  _noiseSNR->setMinimum(-60);
  _noiseSNR->setMaximum(60);
  _noiseSNR->setValue(int(settings.noiseSNR()));
  _noiseSNR->setEnabled(settings.noiseEnabled());
  _noiseSNR->setToolTip(tr("Specifies the signal-to-noise ratio in dB for the noise effect (should be >10dB)."));

  _noiseFilter = new QCheckBox();
  _noiseFilter->setChecked(settings.noiseFilterEnabled());
  _noiseFilter->setEnabled(settings.noiseEnabled());
  _noiseFilter->setToolTip(tr("Enables a band-pass audio fiter around the CW tone freqency."));

  _noiseBw = new QSpinBox();
  _noiseBw->setMinimum(10);
  _noiseBw->setMaximum(4000);
  _noiseBw->setValue(int(settings.noiseFilterBw()));
  _noiseBw->setEnabled(settings.noiseEnabled() && settings.noiseFilterEnabled());
  _noiseBw->setToolTip(tr("Specifies the bandwidth in Hz of the audio filter."));

  connect(_noiseEnabled, SIGNAL(toggled(bool)), this, SLOT(onNoiseToggled(bool)));
  connect(_noiseFilter, SIGNAL(toggled(bool)), this, SLOT(onNoiseFilterToggled(bool)));

  QFormLayout *noiseLayout = new QFormLayout();
  noiseLayout->addRow(tr("Enabled"), _noiseEnabled);
  noiseLayout->addRow(tr("SNR (dB)"), _noiseSNR);
  noiseLayout->addRow(tr("Bandpass"), _noiseFilter);
  noiseLayout->addRow(tr("Bandwidth [Hz]"), _noiseBw);
  QGroupBox *noiseBox = new QGroupBox(tr("Noise"));
  noiseBox->setLayout(noiseLayout);

  _fadingEnabled = new QCheckBox();
  _fadingEnabled->setChecked(settings.fadingEnabled());
  _fadingEnabled->setToolTip(tr("Enables the fading effect of the CW signal to simulate QSB."));

  _fadingRate = new QSpinBox();
  _fadingRate->setMinimum(1);
  _fadingRate->setMaximum(60);
  _fadingRate->setValue(int(settings.fadingRate()));
  _fadingRate->setEnabled(settings.fadingEnabled());
  _fadingRate->setToolTip(tr("Specifies the average rate (per minute) at which the signal will fade away."));
  _fadingMaxDamp = new QSpinBox();
  _fadingMaxDamp->setMinimum(-60);
  _fadingMaxDamp->setMaximum(0);
  _fadingMaxDamp->setValue(int(settings.fadingMaxDamp()));
  _fadingMaxDamp->setEnabled(settings.fadingEnabled());
  _fadingMaxDamp->setToolTip(tr("Specifies the maximum damping in dB for the fading effect."));

  connect(_fadingEnabled, SIGNAL(toggled(bool)), this, SLOT(onFadingToggled(bool)));

  QFormLayout *fadingLayout = new QFormLayout();
  fadingLayout->addRow(tr("Enabled"), _fadingEnabled);
  fadingLayout->addRow(tr("Rate [1/min]"), _fadingRate);
  fadingLayout->addRow(tr("max. Damping [dB]"), _fadingMaxDamp);
  QGroupBox *fadingBox = new QGroupBox(tr("Fading"));
  fadingBox->setLayout(fadingLayout);

  _qrmEnabled = new QCheckBox();
  _qrmEnabled->setChecked(settings.qrmEnabled());
  _qrmEnabled->setToolTip(tr("Enables a QRM effect that simulates nearby QSOs"));

  _qrmStations = new QSpinBox();
  _qrmStations->setMinimum(0);
  _qrmStations->setValue(settings.qrmStations());
  _qrmStations->setEnabled(settings.qrmEnabled());
  _qrmStations->setToolTip(tr("Specifies the number of QRM stations."));

  _qrmSNR = new QSpinBox();
  _qrmSNR->setMinimum(0);
  _qrmSNR->setValue(int(settings.qrmSNR()));
  _qrmSNR->setEnabled(settings.qrmEnabled());
  _qrmSNR->setToolTip(tr("Specifies the relative power (in dB) of all QRM stations to the CW signal."));

  connect(_qrmEnabled, SIGNAL(toggled(bool)), this, SLOT(onQRMToggled(bool)));

  QFormLayout *qrmLayout = new QFormLayout();
  qrmLayout->addRow(tr("Enabled"), _qrmEnabled);
  qrmLayout->addRow(tr("Num QRM stations"), _qrmStations);
  qrmLayout->addRow(tr("SNR"), _qrmSNR);
  QGroupBox *qrmBox = new QGroupBox(tr("QRM"));
  qrmBox->setLayout(qrmLayout);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(noiseBox);
  layout->addWidget(fadingBox);
  layout->addWidget(qrmBox);
  setLayout(layout);
}

void
EffectSettingsView::save() {
  Settings settings;

  settings.setNoiseEnabled(_noiseEnabled->isChecked());
  settings.setNoiseSNR(_noiseSNR->value());
  settings.setNoiseFilterEnabled(_noiseFilter->isChecked());
  settings.setNoiseFilterBw(_noiseBw->value());

  settings.setFadingEnabled(_fadingEnabled->isChecked());
  settings.setFadingRate(_fadingRate->value());
  settings.setFadingMaxDamp(_fadingMaxDamp->value());

  settings.setQRMEnabled(_qrmEnabled->isChecked());
  settings.setQRMStations(_qrmStations->value());
  settings.setQRMSNR(_qrmSNR->value());
}

void
EffectSettingsView::onNoiseToggled(bool enabled) {
  _noiseSNR->setEnabled(enabled);
  _noiseFilter->setEnabled(enabled);
  _noiseBw->setEnabled(enabled && _noiseFilter->isChecked());
}

void
EffectSettingsView::onNoiseFilterToggled(bool enabled) {
  _noiseBw->setEnabled(enabled);
}

void
EffectSettingsView::onFadingToggled(bool enabled) {
  _fadingRate->setEnabled(enabled);
  _fadingMaxDamp->setEnabled(enabled);
}

void
EffectSettingsView::onQRMToggled(bool enabled) {
  _qrmSNR->setEnabled(enabled);
  _qrmStations->setEnabled(enabled);
}


/* ********************************************************************************************* *
 * Device Settings Widget
 * ********************************************************************************************* */
DeviceSettingsView::DeviceSettingsView(QWidget *parent)
  : QWidget(parent)
{
    _settings = new Settings();

  _outputDevices = new QComboBox();
  _outputDevices->setToolTip(tr("Select the audio output device."));

  _inputDevices = new QComboBox();
  _inputDevices->setToolTip(tr("Select the audio input device used for decoding CW send by you."));

  _decoderLevel = new QSpinBox();
  _decoderLevel->setMinimum(-60);
  _decoderLevel->setMaximum(0);
  _decoderLevel->setValue(int(_settings->decoderLevel()));

  _decoderLevel->setToolTip(tr("Specifies the detector threshold in dB for decoding CW."));

  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("Output device"), _outputDevices);
  layout->addRow(tr("Input device"), _inputDevices);
  layout->addRow(tr("Detector threshold (dB)"), _decoderLevel);
  setLayout(layout);
}

void
DeviceSettingsView::save() {
  Settings settings;

  settings.setOutputDevice(_outputDevices->currentText());
  settings.setInputDevice(_inputDevices->currentText());
  settings.setDecoderLevel(_decoderLevel->value());
}

void DeviceSettingsView::populateDeviceSettingsView() {

  QAudioDeviceInfo currentDevice = _settings->outputDevice();
  QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
  foreach (auto device, devices) {
    _outputDevices->addItem(device.deviceName());
    if (device == currentDevice)
      _outputDevices->setCurrentIndex(_outputDevices->model()->rowCount()-1);
  }

  currentDevice = _settings->inputDevice();
  devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
  foreach (auto device, devices) {
    _inputDevices->addItem(device.deviceName());
    if (device == currentDevice)
      _inputDevices->setCurrentIndex(_inputDevices->model()->rowCount()-1);
  }

}
