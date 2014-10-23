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
  _volume->setMaximum(100);
  _volume->setValue(50);

  QWidget *volPanel = new QWidget();
  QVBoxLayout *volPanelLayout = new QVBoxLayout();
  volPanelLayout->setSpacing(5);
  volPanelLayout->setContentsMargins(3,3,5,3);
  volPanelLayout->addWidget(_volume);
  volPanelLayout->addWidget(new QLabel("Volume: 0 dB"));
  volPanel->setLayout(volPanelLayout);

  QToolBar *tbox = new QToolBar();
  tbox->setAllowedAreas(Qt::TopToolBarArea | Qt::RightToolBarArea | Qt::LeftToolBarArea);
  tbox->addAction(_play);
  tbox->addSeparator();
  tbox->addAction(_pref);
  tbox->addAction(_info);
  tbox->addWidget(volPanel);
  this->addToolBar(tbox);

  this->setCentralWidget(_text);

  QObject::connect(&_app, SIGNAL(sessionFinished()), this, SLOT(onSessionFinished()));
  QObject::connect(&_app, SIGNAL(charSend(QChar)), this, SLOT(onCharSend(QChar)));
  QObject::connect(_play, SIGNAL(triggered(bool)), this, SLOT(onPlayToggled(bool)));
  QObject::connect(_pref, SIGNAL(triggered()), this, SLOT(onPrefClicked()));
}

void
MainWindow::onSessionFinished() {
  _play->setChecked(false);
}

void
MainWindow::onCharSend(QChar ch) {
  // Update text-field
  _text->insertPlainText(ch);
  // Update time label
  /*int t = _app.sessionTime()/1000;
  int min = t/60, sec = t%60;
  QString text; text.sprintf("%02i:%02i", min, sec); */
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
