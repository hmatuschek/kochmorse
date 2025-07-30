#include "mainwindow.hh"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QGroupBox>
#include <QFont>
#include <QToolBar>
#include <QLabel>

#include "settings.hh"
#include "aboutdialog.hh"
#include "globals.hh"


MainWindow::MainWindow(Application &app, QWidget *parent)
  : QMainWindow(parent), _app(app)
{
  // Window decoration
  this->setWindowTitle(tr("Koch Morse Tutor"));
  this->setWindowIcon(QIcon(":/icons/kochmorse.svg"));

  // Load persistent settings
  Settings settings;

  // Assemble text view
  _text = new QTextEdit();
  _text->setMinimumSize(640,230);
  _text->document()->setDefaultFont(settings.textFont());
  _text->setReadOnly(true);
  _text->setTextInteractionFlags(Qt::NoTextInteraction);
  if (_app.currentTutor() && _app.currentTutor()->isOutputHidden())
    _text->setVisible(false);

  _input = new QTextEdit();
  _input->setMinimumSize(640,230);
  _input->document()->setDefaultFont(settings.textFont());
  _input->setReadOnly(false);
  _input->setTextInteractionFlags(Qt::TextEditable);
  if (_app.currentTutor() && _app.currentTutor()->isVerifying())
    _input->setVisible(true);
  else
    _input->setVisible(false);

  // Play button
  _play = new QAction(
        QIcon::fromTheme("media-playback-start", QIcon(":/icons/play.svg")), "", this);
  _play->setShortcut(QKeySequence::Save);
  _play->setToolTip(tr("Start/Stop"));
  _play->setCheckable(true);
  _play->setChecked(false);

  _check = new QPushButton(tr("check"));
  _check->setEnabled(false);
  _check->setVisible(app.currentTutor() && app.currentTutor()->isVerifying());

  // Preferences button
  _pref = new QAction(QIcon(":/icons/preferences.svg"), "", this);
  _pref->setShortcut(QKeySequence::Preferences);
  _pref->setToolTip(tr("Settings"));

  // Info button
  _info = new QAction(
        QIcon::fromTheme("help-about", QIcon(":/icons/help.svg")), "", this);
  _info->setToolTip(tr("About Koch Morse"));

  // Volume slider
  _volume = new QSlider(Qt::Horizontal);
  _volume->setMinimum(0);
  _volume->setMaximum(100);
  _volume->setValue(int(100*settings.volume()));
  _volumeLabel = new QLabel(tr("Volume: %1 %").arg(int(100*settings.volume())));

  QWidget *volPanel = new QWidget();
  QVBoxLayout *volPanelLayout = new QVBoxLayout();
  volPanelLayout->setSpacing(5);
  volPanelLayout->setContentsMargins(3,3,5,3);
  volPanelLayout->addWidget(_volume);
  volPanelLayout->addWidget(_volumeLabel);
  volPanel->setLayout(volPanelLayout);

  // Quit button
  _quit = new QAction(
        QIcon::fromTheme("application-exit", QIcon(":/icons/exit.svg")), "", this);
  _quit->setToolTip(tr("Quit Koch Morse"));
  _quit->setShortcuts(QKeySequence::Quit);

  // Assemble tool bar
  QToolBar *tbox = new QToolBar();
  tbox->setAllowedAreas(Qt::TopToolBarArea | Qt::RightToolBarArea | Qt::LeftToolBarArea);
  tbox->addAction(_play);
  tbox->addSeparator();
  tbox->addAction(_pref);
  tbox->addAction(_info);
  tbox->addWidget(volPanel);
  tbox->addSeparator();
  tbox->addAction(_quit);
  this->addToolBar(tbox);

  QWidget *panel = new QWidget();
  QVBoxLayout *vbox = new QVBoxLayout();
  //vbox->setMargin(0);
  vbox->addWidget(_text);
  vbox->addWidget(_input);
  vbox->addWidget(_check);
  panel->setLayout(vbox);
  this->setCentralWidget(panel);

  //set shortcut
  playShortcut = new QShortcut(QKeySequence("Ctrl+X"), this);

  QObject::connect(&_app, SIGNAL(sessionFinished()), this, SLOT(onSessionFinished()));
  QObject::connect(&_app, SIGNAL(charSend(QString)), this, SLOT(onCharSend(QString)));
  QObject::connect(&_app, SIGNAL(charReceived(QString)), this, SLOT(onCharReceived(QString)));
  QObject::connect(&_app, SIGNAL(tutorChanged()), this, SLOT(onTutorChanged()));
  QObject::connect(_play, SIGNAL(triggered(bool)), this, SLOT(onPlayToggled(bool)));
  QObject::connect(_check, SIGNAL(clicked()), this, SLOT(onCheck()));
  QObject::connect(_pref, SIGNAL(triggered()), this, SLOT(onPrefClicked()));
  QObject::connect(_info, SIGNAL(triggered()), this, SLOT(onAboutClicked()));
  QObject::connect(_quit, SIGNAL(triggered()), this, SLOT(onQuit()));
  QObject::connect(_volume, SIGNAL(valueChanged(int)), this, SLOT(onVolumeChanged(int)));
  QObject::connect(playShortcut, SIGNAL(activated()), _play,SLOT(toggle()));
  connect(playShortcut, &QShortcut::activated, this, [this](){
    onPlayToggled(_play->isChecked());
  });

}

void
MainWindow::onSessionFinished() {
  QTextCharFormat old = _text->currentCharFormat();
  QTextCharFormat fmt = old;
  fmt.setForeground(Settings().summaryTextColor());
  _text->setCurrentCharFormat(fmt);
  _text->insertPlainText(_app.summary());
  _text->setCurrentCharFormat(old);
  _play->setChecked(false);
  _check->setEnabled(true);
}

void
MainWindow::onCharSend(QString ch) {
  // Update text-field if tutor is not verifying
  QTextCharFormat old = _text->currentCharFormat();
  QTextCharFormat fmt = old;
  fmt.setForeground(Settings().rxTextColor());
  _text->setCurrentCharFormat(fmt);
  _text->insertPlainText(ch);
  _text->setCurrentCharFormat(old);
}

void
MainWindow::onCharReceived(QString ch) {
  // Update text-field
  QTextCharFormat old = _text->currentCharFormat();
  QTextCharFormat fmt = old;
  fmt.setForeground(Settings().txTextColor());
  _text->setCurrentCharFormat(fmt);
  _text->insertPlainText(ch);
  _text->setCurrentCharFormat(old);
}

void
MainWindow::onPlayToggled(bool play) {
  Settings settings;
  if (play) {
    _text->document()->clear();
    _text->document()->setDefaultFont(settings.textFont());
    _input->document()->clear();
    _input->document()->setDefaultFont(settings.textFont());
    _check->setEnabled(false);
    _app.startSession();
  } else {
    _check->setEnabled(false);
    _app.stopSession();
  }
}

void
MainWindow::onPrefClicked() {
  SettingsDialog d;
  if (QDialog::Accepted == d.exec()) {
    if (_play->isChecked()) {
      _play->setChecked(false);
    }
    _app.applySettings();
  }
}

void
MainWindow::onAboutClicked() {
  AboutDialog().exec();
}

void
MainWindow::onVolumeChanged(int value)
{
  double factor = double(value)/100;
  factor = std::max(0., std::min(factor, 1.));

  _app.setVolume(factor);

  Settings settings;
  settings.setVolume(factor);
  _volumeLabel->setText(tr("Volume: %1%").arg(int(100*settings.volume())));
}

void
MainWindow::onQuit() {
  _app.stopSession();
  this->close();
}

void
MainWindow::onCheck() {
  if (!_app.currentTutor())
    return;
  QString result;
  _app.currentTutor()->verify(_input->document()->toPlainText(), result);
  _input->document()->setHtml(result);
  _check->setEnabled(false);
}

void
MainWindow::onTutorChanged() {
  if (_app.currentTutor() && _app.currentTutor()->isOutputHidden())
    _text->setVisible(false);
  else
    _text->setVisible(true);
  if (_app.currentTutor() && _app.currentTutor()->isVerifying()) {
    _input->setVisible(true);
    _check->setVisible(true);
  } else {
    _input->setVisible(false);
    _check->setVisible(false);
  }
}
