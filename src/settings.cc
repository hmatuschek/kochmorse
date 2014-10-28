#include "settings.hh"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QGroupBox>
#include "morseencoder.hh"


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
  speed = std::max(5, std::min(speed, 35));
  this->setValue("speed", speed);
}

int
Settings::effSpeed() const {
  return this->value("effSpeed", 15).toInt();
}
void
Settings::setEffSpeed(int speed) {
  speed = std::max(5, std::min(speed, 35));
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

  _tabs = new QTabWidget();
  _tabs->addTab(_tutor, tr("Tutor"));
  _tabs->addTab(_code, tr("Morse Code"));
  _tabs->addTab(_effects, tr("Effects"));

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
  _speed->setMinimum(5); _speed->setMaximum(35);
  _speed->setValue(settings.speed());

  _effSpeed = new QSpinBox();
  _effSpeed->setMinimum(5); _speed->setMaximum(35);
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

  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("Speed (WPM)"), _speed);
  layout->addRow(tr("Eff. speed (WPM)"), _effSpeed);
  layout->addRow(tr("Tone (Hz)"), _tone);
  layout->addRow(tr("Dash pitch (Hz)"), _daPitch);
  layout->addRow(tr("Sound"), _sound);
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

  _kochSettings = new KochTutorSettingsView();
  _randSettings = new RandomTutorSettingsView();
  _qsoSettings  = new QSOTutorSettingsView();

  _tutorSettings = new QStackedWidget();
  _tutorSettings->addWidget(_kochSettings);
  _tutorSettings->addWidget(_randSettings);
  _tutorSettings->addWidget(_qsoSettings);

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

  // Get tutor by index
  Settings settings;
  if (0 == _tutor->currentIndex()) {
    settings.setTutor(Settings::TUTOR_KOCH);
  } else if (1 == _tutor->currentIndex()) {
    settings.setTutor(Settings::TUTOR_RANDOM);
  } else if (2 == _tutor->currentIndex()) {
    settings.setTutor(Settings::TUTOR_QSO);
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

  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("Lesson"), _lesson);
  layout->addRow(tr("Prefer last chars"), _prefLastChars);

  this->setLayout(layout);
}

void
KochTutorSettingsView::save() {
  Settings settings;
  settings.setKochLesson(_lesson->value());
  settings.setKochPrefLastChars(_prefLastChars->isChecked());
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
    _alpha->addItem(c, c, enabled_chars.contains(c));
  }
  // Assemble char table
  _num = new ListWidget();
  foreach (QChar c, num) {
    _num->addItem(c, c, enabled_chars.contains(c));
  }
  // Assemble punc table
  _punct = new ListWidget();
  foreach (QChar c, punct) {
    _punct->addItem(c, c, enabled_chars.contains(c));
  }
  // Assemble prosig table
  _prosign = new ListWidget();
  foreach (QChar c, prosign) {
    _prosign->addItem(MorseEncoder::mapProsign(c), c, enabled_chars.contains(c));
  }

  QTabWidget *tabs = new QTabWidget();
  tabs->addTab(_alpha, tr("Characters"));
  tabs->addTab(_num, tr("Numbers"));
  tabs->addTab(_punct, tr("Punctuations"));
  tabs->addTab(_prosign, tr("Prosigns"));

  QHBoxLayout *layout = new QHBoxLayout();
  layout->addWidget(tabs);
  this->setLayout(layout);
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
}


/* ********************************************************************************************* *
 * QSO Tutor Settings Widget
 * ********************************************************************************************* */
QSOTutorSettingsView::QSOTutorSettingsView(QWidget *parent)
  : QGroupBox(parent)
{
  QLabel *label = new QLabel(tr("<No settings for this tutor>"));
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(label);
  setLayout(layout);
}

void
QSOTutorSettingsView::save() {
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

