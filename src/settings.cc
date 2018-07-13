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
  return this->value("koch/summary", false).toBool();
}

void
Settings::setKochSummary(bool show) {
  this->setValue("koch/summary", show);
}


QSet<QChar>
Settings::randomChars() const {
  QSet<QChar> chars;
  if (! this->contains("random/chars")) {
    chars << 'a' << 'b' << 'c' << 'd' << 'e' << 'f' << 'g' << 'h' << 'i' << 'j' << 'k' << 'l' << 'm'
          << 'n' << 'o' << 'p' << 'q' << 'r' << 's' << 't' << 'u' << 'v' << 'w' << 'x' << 'y' << 'z'
          << '0' << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8' << '9' << '.' << ',' << '?'
          << '/' << '&' << ':' << ';' << '=' << '+' << '-' << '@' << '(' << ')'
          << QChar(0x2417) /* BK */ << QChar(0x2404) /* CL */ << QChar(0x2403) /* SK */
          << QChar(0x2406) /* SN */;
  } else {
    QString str = this->value("random/chars").toString();
    for (int i=0; i<str.size(); i++) { chars.insert(str[i]); }
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

  QDialogButtonBox *bbox = new QDialogButtonBox();
  bbox->addButton(QDialogButtonBox::Ok);
  bbox->addButton(QDialogButtonBox::Cancel);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(_tabs);
  layout->addWidget(bbox);
  this->setLayout(layout);

  QObject::connect(bbox, SIGNAL(accepted()), this, SLOT(accept()));
  QObject::connect(bbox, SIGNAL(rejected()), this, SLOT(reject()));
}

void
SettingsDialog::accept() {
  _tutor->save();
  _code->save();
  _effects->save();
  _devices->save();
  QDialog::accept();
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

  _effSpeed = new QSpinBox();
  _effSpeed->setMinimum(5); _speed->setMaximum(100);
  _effSpeed->setValue(settings.effSpeed());

  _tone = new QLineEdit(QString::number(settings.tone()));
  QIntValidator *tone_val = new QIntValidator(20,20000);
  _tone->setValidator(tone_val);

  _daPitch = new QLineEdit(QString::number(settings.dashPitch()));
  QIntValidator *pitch_val = new QIntValidator(-1000,1000);
  _daPitch->setValidator(pitch_val);

  _sound = new QComboBox();
  _sound->addItem(tr("Soft"), uint(MorseEncoder::SOUND_SOFT));
  _sound->addItem(tr("Sharp"), uint(MorseEncoder::SOUND_SHARP));
  _sound->addItem(tr("Cracking"), uint(MorseEncoder::SOUND_CRACKING));
  switch (settings.sound()) {
  case MorseEncoder::SOUND_SOFT: _sound->setCurrentIndex(0); break;
  case MorseEncoder::SOUND_SHARP: _sound->setCurrentIndex(1); break;
  case MorseEncoder::SOUND_CRACKING: _sound->setCurrentIndex(2); break;
  }

  _jitter = new QComboBox();
  _jitter->addItem(tr("Exact"), uint(MorseEncoder::JITTER_EXACT));
  _jitter->addItem(tr("Bug"), uint(MorseEncoder::JITTER_BUG));
  _jitter->addItem(tr("Straight"), uint(MorseEncoder::JITTER_STRAIGT));
  switch (settings.jitter()) {
    case MorseEncoder::JITTER_EXACT: _jitter->setCurrentIndex(0); break;
    case MorseEncoder::JITTER_BUG: _jitter->setCurrentIndex(1); break;
    case MorseEncoder::JITTER_STRAIGT: _jitter->setCurrentIndex(2); break;
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
  _tutor->addItem(tr("Koch method"));
  _tutor->addItem(tr("Random"));
  _tutor->addItem(tr("QSO"));
  _tutor->addItem(tr("Q-Codes"));
  _tutor->addItem(tr("Transmit"));
  _tutor->addItem(tr("Chat"));
  _tutor->addItem(tr("Text generator"));

  _kochSettings = new KochTutorSettingsView();
  _randSettings = new RandomTutorSettingsView();
  _qsoSettings  = new QSOTutorSettingsView();
  _qcodeSettings  = new QSOTutorSettingsView();
  _txSettings   = new TXTutorSettingsView();
  _chatSettings = new ChatTutorSettingsView();
  _textgetSettings = new TextGenTutorSettingsView();

  _tutorSettings = new QStackedWidget();
  _tutorSettings->addWidget(_kochSettings);
  _tutorSettings->addWidget(_randSettings);
  _tutorSettings->addWidget(_qsoSettings);
  _tutorSettings->addWidget(_qcodeSettings);
  _tutorSettings->addWidget(_txSettings);
  _tutorSettings->addWidget(_chatSettings);
  _tutorSettings->addWidget(_textgetSettings);

  Settings settings;
  if (Settings::TUTOR_KOCH == settings.tutor()) {
    _tutor->setCurrentIndex(0);
    _tutorSettings->setCurrentIndex(0);
  } else if (Settings::TUTOR_RANDOM == settings.tutor()) {
    _tutor->setCurrentIndex(1);
    _tutorSettings->setCurrentIndex(1);
  } else if (Settings::TUTOR_QSO == settings.tutor()) {
    _tutor->setCurrentIndex(2);
    _tutorSettings->setCurrentIndex(2);
  } else if (Settings::TUTOR_QCODE == settings.tutor()) {
    _tutor->setCurrentIndex(3);
    _tutorSettings->setCurrentIndex(3);
  } else if (Settings::TUTOR_TX == settings.tutor()) {
    _tutor->setCurrentIndex(4);
    _tutorSettings->setCurrentIndex(4);
  } else if (Settings::TUTOR_CHAT == settings.tutor()) {
    _tutor->setCurrentIndex(5);
    _tutorSettings->setCurrentIndex(5);
  } else if (Settings::TUTOR_TEXTGEN == settings.tutor()) {
    _tutor->setCurrentIndex(6);
    _tutorSettings->setCurrentIndex(6);
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
    settings.setTutor(Settings::TUTOR_QSO);
  } else if (3 == _tutor->currentIndex()) {
    settings.setTutor(Settings::TUTOR_QCODE);
  } else if (4 == _tutor->currentIndex()) {
    settings.setTutor(Settings::TUTOR_TX);
  } else if (5 == _tutor->currentIndex()) {
    settings.setTutor(Settings::TUTOR_CHAT);
  } else if (6 == _tutor->currentIndex()) {
    settings.setTutor(Settings::TUTOR_TEXTGEN);
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

  _prefLastChars = new QCheckBox();
  _prefLastChars->setChecked(settings.kochPrefLastChars());

  _repLastChar = new QCheckBox();
  _repLastChar->setChecked(settings.kochRepeatLastChar());

  _minGroupSize = new QSpinBox();
  _minGroupSize->setValue(settings.kochMinGroupSize());
  _minGroupSize->setMinimum(1);
  _minGroupSize->setMaximum(settings.kochMaxGroupSize());

  _maxGroupSize = new QSpinBox();
  _maxGroupSize->setValue(settings.kochMaxGroupSize());
  _maxGroupSize->setMinimum(settings.kochMinGroupSize());
  _maxGroupSize->setMaximum(20);

  _infinite = new QCheckBox();
  _infinite->setChecked(settings.kochInfiniteLineCount());

  _lineCount = new QSpinBox();
  _lineCount->setMinimum(1);
  _lineCount->setValue(settings.kochLineCount());
  if (settings.kochInfiniteLineCount())
    _lineCount->setEnabled(false);

  _summary = new QCheckBox();
  _summary->setChecked(settings.kochSummary());
  if (settings.kochInfiniteLineCount())
    _summary->setEnabled(false);

  // Cross connect min and max group size splin boxes to maintain
  // consistent settings
  connect(_minGroupSize, SIGNAL(valueChanged(int)), this, SLOT(onMinSet(int)));
  connect(_maxGroupSize, SIGNAL(valueChanged(int)), this, SLOT(onMaxSet(int)));
  connect(_infinite, SIGNAL(toggled(bool)), this, SLOT(onInfiniteToggled(bool)));

  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("Lesson"), _lesson);
  layout->addRow(tr("Prefer last chars"), _prefLastChars);
  layout->addRow(tr("Repeat last char"), _repLastChar);
  layout->addRow(tr("Min. group size"), _minGroupSize);
  layout->addRow(tr("Max. group size"), _maxGroupSize);
  layout->addRow(tr("Infinite lines"), _infinite);
  layout->addRow(tr("Line count"), _lineCount);
  layout->addRow(tr("Show summary"), _summary);
  this->setLayout(layout);
}

void
KochTutorSettingsView::save() {
  Settings settings;
  settings.setKochLesson(_lesson->value());
  settings.setKochPrefLastChars(_prefLastChars->isChecked());
  settings.setKochRepeatLastChar(_repLastChar->isChecked());
  settings.setKochMinGroupSize(_minGroupSize->value());
  settings.setKochMaxGroupSize(_maxGroupSize->value());
  settings.setKochInifiniteLineCount(_infinite->isChecked());
  settings.setKochLineCount(_lineCount->value());
  settings.setKochSummary(_summary->isChecked());
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
}


/* ********************************************************************************************* *
 * Random Tutor Settings Widget
 * ********************************************************************************************* */
RandomTutorSettingsView::RandomTutorSettingsView(QWidget *parent)
  : QGroupBox(tr("Random tutor settings"),parent)
{
  Settings settings;

  QSet<QChar> enabled_chars = settings.randomChars();
  QList<QChar> alpha, num, punct, prosign;
  alpha << 'a' << 'b' << 'c' << 'd' << 'e' << 'f' << 'g' << 'h' << 'i' << 'j' << 'k' << 'l' << 'm'
        << 'n' << 'o' << 'p' << 'q' << 'r' << 's' << 't' << 'u' << 'v' << 'w' << 'x' << 'y' << 'z';
  num << '0' << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8' << '9';
  punct << '.' << ',' << ':' << ';' << '?' << '-' << '@' << '(' << ')' << '/';
  prosign << '=' << '+' << '&' << QChar(0x2417) /* BK */ << QChar(0x2404) /* CL */
          << QChar(0x2403) /* SK */ << QChar(0x2406) /* SN */ << QChar(0x2407) /* KL */;

  // Assemble char table
  _alpha = new ListWidget();
  foreach (QChar c, alpha) {
    QString txt = QString("%1 (%2)").arg(c).arg(Globals::charTable[c]);
    _alpha->addItem(txt, c, enabled_chars.contains(c));
  }
  // Assemble char table
  _num = new ListWidget();
  foreach (QChar c, num) {
    QString txt = QString("%1 (%2)").arg(c).arg(Globals::charTable[c]);
    _num->addItem(txt, c, enabled_chars.contains(c));
  }
  // Assemble punc table
  _punct = new ListWidget();
  foreach (QChar c, punct) {
    QString txt = QString("%1 (%2)").arg(c).arg(Globals::charTable[c]);
    _punct->addItem(txt, c, enabled_chars.contains(c));
  }
  // Assemble prosig table
  _prosign = new ListWidget();
  foreach (QChar c, prosign) {
    QString txt = QString("%1 (%2)").arg(Globals::mapProsign(c)).arg(Globals::charTable[c]);
    _prosign->addItem(txt, c, enabled_chars.contains(c));
  }

  QTabWidget *tabs = new QTabWidget();
  tabs->addTab(_alpha, tr("Characters"));
  tabs->addTab(_num, tr("Numbers"));
  tabs->addTab(_punct, tr("Punctuations"));
  tabs->addTab(_prosign, tr("Prosigns"));

  _minGroupSize = new QSpinBox();
  _minGroupSize->setValue(settings.randomMinGroupSize());
  _minGroupSize->setMinimum(1);
  _minGroupSize->setMaximum(settings.randomMaxGroupSize());

  _maxGroupSize = new QSpinBox();
  _maxGroupSize->setValue(settings.randomMaxGroupSize());
  _maxGroupSize->setMinimum(settings.randomMinGroupSize());
  _maxGroupSize->setMaximum(20);

  _infinite = new QCheckBox();
  _infinite->setChecked(settings.randomInfiniteLineCount());

  _lineCount = new QSpinBox();
  _lineCount->setMinimum(1);
  _lineCount->setValue(settings.randomLineCount());
  if (settings.randomInfiniteLineCount())
    _lineCount->setEnabled(false);

  _summary = new QCheckBox();
  _summary->setChecked(settings.randomSummary());
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
 * QSO Tutor Settings Widget
 * ********************************************************************************************* */
QSOTutorSettingsView::QSOTutorSettingsView(QWidget *parent)
  : QGroupBox(tr("QSO tutor settings"), parent)
{
  QLabel *label = new QLabel(tr("<No settings for this tutor>"));
  label->setAlignment(Qt::AlignCenter);
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(label);
  setLayout(layout);
}

void
QSOTutorSettingsView::save() {
  // pass...
}


/* ********************************************************************************************* *
 * TextGen Tutor Settings Widget
 * ********************************************************************************************* */
TextGenTutorSettingsView::TextGenTutorSettingsView(QWidget *parent)
  : QGroupBox(tr("Generated text tutor settings"),parent)
{
  Settings settings;
  QString help = tr("Select a rule file (ending on .xml) "
                    "or a plain-text file (ending on .txt) to send.");

  _filename = new QLineEdit();
  _filename->setText(settings.textGenFilename());
  _filename->setToolTip(help);
  _filename->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

  QPushButton *select = new QPushButton("...");
  select->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  QHBoxLayout *llayout = new QHBoxLayout();
  llayout->addWidget(_filename, 1);
  llayout->addWidget(select);

  QFormLayout *layout = new QFormLayout();
  QLabel *label = new QLabel(help);
  label->setWordWrap(true);
  layout->addWidget(label);
  layout->addRow(tr("Rule/text file"), llayout);
  layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
  setLayout(layout);

  connect(select, SIGNAL(clicked(bool)), this, SLOT(onSelectFile()));
}

void
TextGenTutorSettingsView::save() {
  qDebug() << "Got" << _filename->text();
  QFileInfo info(_filename->text());
  if (info.exists())
    Settings().setTextGenFilename(info.absoluteFilePath());
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
  : QGroupBox(tr("Chat tutor settings"),parent)
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
  : QWidget(parent), _noiseEnabled(0), _noiseSNR(0)
{
  Settings settings;

  _noiseEnabled = new QCheckBox();
  _noiseEnabled->setChecked(settings.noiseEnabled());

  _noiseSNR = new QSpinBox();
  _noiseSNR->setMinimum(-60);
  _noiseSNR->setMaximum(60);
  _noiseSNR->setValue(settings.noiseSNR());

  QFormLayout *noiseLayout = new QFormLayout();
  noiseLayout->addRow(tr("Enabled"), _noiseEnabled);
  noiseLayout->addRow(tr("SNR (dB)"), _noiseSNR);
  QGroupBox *noiseBox = new QGroupBox(tr("Noise"));
  noiseBox->setLayout(noiseLayout);

  _fadingEnabled = new QCheckBox();
  _fadingEnabled->setChecked(settings.fadingEnabled());

  _fadingRate = new QSpinBox();
  _fadingRate->setMinimum(1);
  _fadingRate->setMaximum(60);
  _fadingRate->setValue(settings.fadingRate());

  _fadingMaxDamp = new QSpinBox();
  _fadingMaxDamp->setMinimum(-60);
  _fadingMaxDamp->setMaximum(0);
  _fadingMaxDamp->setValue(settings.fadingMaxDamp());

  QFormLayout *fadingLayout = new QFormLayout();
  fadingLayout->addRow(tr("Enabled"), _fadingEnabled);
  fadingLayout->addRow(tr("Rate [1/min]"), _fadingRate);
  fadingLayout->addRow(tr("max. Damping [dB]"), _fadingMaxDamp);
  QGroupBox *fadingBox = new QGroupBox(tr("Fading"));
  fadingBox->setLayout(fadingLayout);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(noiseBox);
  layout->addWidget(fadingBox);
  setLayout(layout);
}

void
EffectSettingsView::save() {
  Settings settings;

  settings.setNoiseEnabled(Qt::Checked == _noiseEnabled->checkState());
  settings.setNoiseSNR(_noiseSNR->value());

  settings.setFadingEnabled(Qt::Checked == _fadingEnabled->checkState());
  settings.setFadingRate(_fadingRate->value());
  settings.setFadingMaxDamp(_fadingMaxDamp->value());
}


/* ********************************************************************************************* *
 * Device Settings Widget
 * ********************************************************************************************* */
DeviceSettingsView::DeviceSettingsView(QWidget *parent)
  : QWidget(parent)
{
  Settings settings;

  _outputDevices = new QComboBox();
  QAudioDeviceInfo currentDevice = settings.outputDevice();
  QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
  foreach (auto device, devices) {
    _outputDevices->addItem(device.deviceName());
    if (device == currentDevice)
      _outputDevices->setCurrentIndex(_outputDevices->model()->rowCount()-1);
  }

  _inputDevices = new QComboBox();
  currentDevice = settings.inputDevice();
  devices = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
  foreach (auto device, devices) {
    _inputDevices->addItem(device.deviceName());
    if (device == currentDevice)
      _inputDevices->setCurrentIndex(_inputDevices->model()->rowCount()-1);
  }

  _decoderLevel = new QSpinBox();
  _decoderLevel->setMinimum(-60);
  _decoderLevel->setMaximum(0);
  _decoderLevel->setValue(settings.decoderLevel());

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
