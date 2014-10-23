#include "mainwindow.hh"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QIcon>
#include <QGroupBox>


MainWindow::MainWindow(Application &app, QWidget *parent)
  : QMainWindow(parent), _app(app)
{
  _text = new QPlainTextEdit();
  _text->setReadOnly(true);

  _time = new QLabel("00:00");
  QHBoxLayout *timeLayout = new QHBoxLayout();
  timeLayout->addWidget(_time); timeLayout->setContentsMargins(0,0,0,0);
  QGroupBox *timeBox = new QGroupBox(tr("Time"));
  timeBox->setLayout(timeLayout);

  _speed = new QSpinBox();
  _speed->setMinimum(5); _speed->setMaximum(30);
  _speed->setValue(20);
  QHBoxLayout *speedLayout = new QHBoxLayout();
  speedLayout->addWidget(_speed); speedLayout->setContentsMargins(0,0,0,0);
  QGroupBox *speedBox = new QGroupBox(tr("Speed"));
  speedBox->setLayout(speedLayout);

  _effSpeed = new QSpinBox();
  _effSpeed->setMinimum(5); _effSpeed->setMaximum(30);
  _effSpeed->setValue(15);
  QHBoxLayout *effSpeedLayout = new QHBoxLayout();
  effSpeedLayout->addWidget(_effSpeed); effSpeedLayout->setContentsMargins(0,0,0,0);
  QGroupBox *effSpeedBox = new QGroupBox(tr("Effective Speed"));
  effSpeedBox->setLayout(effSpeedLayout);

  _play = new QPushButton(QIcon::fromTheme("media-playback-start"), "");
  _play->setCheckable(true);
  _play->setChecked(false);

  QVBoxLayout *vbox = new QVBoxLayout();
  vbox->setContentsMargins(0,0,0,0);
  vbox->setSpacing(0);
  vbox->addWidget(_text);

  QHBoxLayout *hbox = new QHBoxLayout();
  hbox->setSpacing(5);
  hbox->addWidget(timeBox);
  hbox->addWidget(speedBox);
  hbox->addWidget(effSpeedBox);
  hbox->addWidget(_play);
  vbox->addLayout(hbox);

  QWidget *panel = new QWidget();
  panel->setLayout(vbox);
  this->setCentralWidget(panel);

  QObject::connect(&_app, SIGNAL(sessionFinished()), this, SLOT(onSessionFinished()));
  QObject::connect(&_app, SIGNAL(charSend(QChar)), this, SLOT(onCharSend(QChar)));
  QObject::connect(_play, SIGNAL(toggled(bool)), this, SLOT(onPlayToggled(bool)));
}

void
MainWindow::onSessionFinished() {
  _play->setChecked(false);
}

void
MainWindow::onCharSend(QChar ch) {
  _text->insertPlainText(ch);
  int t = _app.sessionTime()/1000;
  int min = t/60;
  int sec = t%60;
  QString text; text.sprintf("%02i:%02i", min, sec);
  _time->setText(text);
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
