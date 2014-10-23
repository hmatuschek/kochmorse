#include "settings.hh"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QGroupBox>
#include <iostream>


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
  n = std::max(2, std::min(n, 42));
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
          << '/' << '&' << ':' << ';' << '=' << '+' << '-' << '@';
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


/* ********************************************************************************************* *
 * Settings Dialog
 * ********************************************************************************************* */
SettingsDialog::SettingsDialog(QWidget *parent)
  : QDialog(parent)
{
  this->setModal(true);

  _tutor = new TutorSettingsView();
  _code  = new CodeSettingsView();

  _tabs = new QTabWidget();
  _tabs->addTab(_tutor, tr("Tutor"));
  _tabs->addTab(_code, tr("Morse Code"));

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

  QFormLayout *layout = new QFormLayout();
  layout->addRow(tr("Speed (WPM)"), _speed);
  layout->addRow(tr("Eff. speed (WPM)"), _effSpeed);
  layout->addRow(tr("Tone (Hz)"), _tone);
  layout->addRow(tr("Dash pitch (Hz)"), _daPitch);

  this->setLayout(layout);
}

void
CodeSettingsView::save() {
  Settings settings;
  settings.setSpeed(_speed->value());
  settings.setEffSpeed(_effSpeed->value());
  settings.setTone(_tone->text().toInt());
  settings.setDashPitch(_daPitch->text().toInt());
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

  _kochSettings = new KochTutorSettingsView();
  _randSettings = new RandomTutorSettingsView();

  _tutorSettings = new QStackedWidget();
  _tutorSettings->addWidget(_kochSettings);
  _tutorSettings->addWidget(_randSettings);

  Settings settings;
  if (Settings::TUTOR_KOCH == settings.tutor()) {
    _tutor->setCurrentIndex(0);
    _tutorSettings->setCurrentIndex(0);
  } else if (Settings::TUTOR_RANDOM == settings.tutor()) {
    _tutor->setCurrentIndex(1);
    _tutorSettings->setCurrentIndex(1);
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
  _lesson->setMinimum(2); _lesson->setMaximum(42);
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
  QListWidgetItem *item = 0;

  QSet<QChar> enabled_chars = settings.randomChars();
  QList<QChar> alpha, num, punct, prosign;
  alpha << 'a' << 'b' << 'c' << 'd' << 'e' << 'f' << 'g' << 'h' << 'i' << 'j' << 'k' << 'l' << 'm'
        << 'n' << 'o' << 'p' << 'q' << 'r' << 's' << 't' << 'u' << 'v' << 'w' << 'x' << 'y' << 'z';
  num << '0' << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8' << '9';
  punct << '.' << ',' << ':' << ';' << '&' << '?' << '+' << '-' << '@';
  prosign << '/' << '=';

  // Assemble char table
  _alpha = new QListWidget();
  _alpha->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  foreach (QChar c, alpha) {
    item = new QListWidgetItem(c, _alpha);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    if (enabled_chars.contains(c)) {
      item->setCheckState(Qt::Checked);
    } else {
      item->setCheckState(Qt::Unchecked);
    }
  }
  // Assemble char table
  _num = new QListWidget();
  _num->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  foreach (QChar c, num) {
    item = new QListWidgetItem(c, _num);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    if (enabled_chars.contains(c)) {
      item->setCheckState(Qt::Checked);
    } else {
      item->setCheckState(Qt::Unchecked);
    }
  }
  // Assemble punc table
  _punct = new QListWidget();
  _punct->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  foreach (QChar c, punct) {
    item = new QListWidgetItem(c, _punct);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    if (enabled_chars.contains(c)) {
      item->setCheckState(Qt::Checked);
    } else {
      item->setCheckState(Qt::Unchecked);
    }
  }
  // Assemble prosig table
  _prosign = new QListWidget();
  _prosign->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  foreach (QChar c, prosign) {
    item = new QListWidgetItem(c, _prosign);
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    if (enabled_chars.contains(c)) {
      item->setCheckState(Qt::Checked);
    } else {
      item->setCheckState(Qt::Unchecked);
    }
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
  for (int i=0; i<_alpha->count(); i++) {
    if (Qt::Checked == _alpha->item(i)->checkState()) {
      enabled_chars.insert(_alpha->item(i)->text()[0]);
    }
  }
  for (int i=0; i<_num->count(); i++) {
    if (Qt::Checked == _num->item(i)->checkState()) {
      enabled_chars.insert(_num->item(i)->text()[0]);
    }
  }
  for (int i=0; i<_punct->count(); i++) {
    if (Qt::Checked == _punct->item(i)->checkState()) {
      enabled_chars.insert(_punct->item(i)->text()[0]);
    }
  }
  for (int i=0; i<_prosign->count(); i++) {
    if (Qt::Checked == _prosign->item(i)->checkState()) {
      enabled_chars.insert(_prosign->item(i)->text()[0]);
    }
  }

  Settings().setRandomChars(enabled_chars);
}
