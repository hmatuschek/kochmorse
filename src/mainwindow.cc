#include "mainwindow.hh"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QGroupBox>
#include <QFont>
#include <QToolBar>
#include <QLabel>

#include "settings.hh"


MainWindow::MainWindow(Application &app, QWidget *parent)
  : QMainWindow(parent), _app(app)
{
  this->setWindowTitle(tr("Koch Morse Tutor"));
  Settings settings;

  _text = new QTextEdit();
  _text->setMinimumSize(640,230);
  _text->setFontFamily("Helvetica");
  _text->setFontPointSize(12);
  _text->setReadOnly(true);

  _play = new QAction(
        QIcon::fromTheme("media-playback-start", QIcon(":/icons/play.svg")), "", this);
  _play->setToolTip(tr("Start/Stop"));
  _play->setCheckable(true);
  _play->setChecked(false);

  _pref = new QAction(
        QIcon::fromTheme("system-preferences", QIcon(":/icons/preferences.svg")), "", this);
  _pref->setToolTip(tr("Settings"));

  _info = new QAction(
        QIcon::fromTheme("application-help", QIcon(":/icons/help.svg")), "", this);

  _volume = new QSlider(Qt::Horizontal);
  _volume->setMinimum(0);
  _volume->setMaximum(200);
  _volume->setValue(int(100*settings.volume()));
  _volumeLabel = new QLabel(tr("Volume: %1%").arg(int(100*settings.volume())));

  QWidget *volPanel = new QWidget();
  QVBoxLayout *volPanelLayout = new QVBoxLayout();
  volPanelLayout->setSpacing(5);
  volPanelLayout->setContentsMargins(3,3,5,3);
  volPanelLayout->addWidget(_volume);
  volPanelLayout->addWidget(_volumeLabel);
  volPanel->setLayout(volPanelLayout);

  _quit = new QAction(QIcon(":/icons/exit.svg"), "", this);

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

  this->setCentralWidget(_text);

  QObject::connect(&_app, SIGNAL(sessionFinished()), this, SLOT(onSessionFinished()));
  QObject::connect(&_app, SIGNAL(charSend(QChar)), this, SLOT(onCharSend(QChar)));
  QObject::connect(_play, SIGNAL(triggered(bool)), this, SLOT(onPlayToggled(bool)));
  QObject::connect(_pref, SIGNAL(triggered()), this, SLOT(onPrefClicked()));
  QObject::connect(_quit, SIGNAL(triggered()), this, SLOT(close()));
  QObject::connect(_volume, SIGNAL(valueChanged(int)), this, SLOT(onVolumeChanged(int)));
}

void
MainWindow::onSessionFinished() {
  _play->setChecked(false);
}

void
MainWindow::onCharSend(QChar ch) {
  // Update text-field
  _text->insertPlainText(ch);
}

void
MainWindow::onPlayToggled(bool play) {
  if (play) {
    _text->clear();
    _app.startSession();
  } else {
    _app.stopSession();
  }
}

void
MainWindow::onPrefClicked() {
  SettingsDialog d;
  if (QDialog::Accepted == d.exec()) {
    _app.applySettings();
  }
}

void
MainWindow::onVolumeChanged(int value)
{
  double factor = double(value)/100;
  factor = std::max(0., std::min(factor, 2.));

  _app.setVolume(factor);

  Settings settings;
  settings.setVolume(factor);
  _volumeLabel->setText(tr("Volume: %1%").arg(int(100*settings.volume())));
}
