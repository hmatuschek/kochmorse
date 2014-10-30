#include "decoderwindow.hh"
#include <QIntValidator>
#include <cmath>
#include <QToolBar>


DecoderWindow::DecoderWindow(QWidget *parent)
  : QMainWindow(parent), _decoder(20, 700), _audio_src(&_decoder), _lastChar('\0')
{
  _textView = new QPlainTextEdit();
  _textView->setMinimumSize(640,230);
  QFont f = _textView->document()->defaultFont();
  f.setFamily("Courier");
  f.setPointSize(14);
  f.setStyleHint(QFont::Monospace);
  _textView->document()->setDefaultFont(f);
  _textView->setReadOnly(true);

  this->setCentralWidget(_textView);

  _freq = new QLineEdit();
  _freq->setValidator(new QIntValidator(20, 2000));
  _freq->setText("750");

  _speed = new QSpinBox();
  _speed->setMinimum(5);
  _speed->setMaximum(35);
  _speed->setValue(20);

  _threshold = new QSlider(Qt::Horizontal);
  _threshold->setMinimum(-60);
  _threshold->setMaximum(0);
  _threshold->setValue(-4);

  QToolBar *toolbar = new QToolBar();
  toolbar->addWidget(_freq);
  toolbar->addWidget(_speed);
  toolbar->addSeparator();
  toolbar->addWidget(_threshold);
  this->addToolBar(toolbar);

  QObject::connect(&_decoder, SIGNAL(charReceived(QChar)), this, SLOT(onCharReceived(QChar)));
  QObject::connect(&_decoder, SIGNAL(unknownCharReceived(QString)), this, SLOT(onUnknownCharReceived(QString)));

  QObject::connect(_freq, SIGNAL(textEdited(QString)), this, SLOT(onFequencySet(QString)));
  QObject::connect(_speed, SIGNAL(valueChanged(int)), this, SLOT(onSpeedSet(int)));
  QObject::connect(_threshold, SIGNAL(valueChanged(int)), this, SLOT(onThresholdSet(int)));

  _audio_src.start();
}

DecoderWindow::~DecoderWindow() {
  _audio_src.stop();
}

void
DecoderWindow::onCharReceived(QChar ch) {
  // Ignore multiple white-spaces
  if ((' ' == ch) && (' ' == _lastChar)) { return; }
  _textView->insertPlainText(ch);
  _lastChar = ch;
}

void
DecoderWindow::onUnknownCharReceived(QString pattern)  {
  _textView->insertPlainText(QString("[%1]").arg(pattern));
  _lastChar = '\0';
}

void
DecoderWindow::onFequencySet(QString val) {
  _audio_src.stop();
  _decoder.setFreq(val.toDouble());
  _audio_src.start();
}

void
DecoderWindow::onSpeedSet(int val) {
  _audio_src.stop();
  _decoder.setSpeed(val);
  _audio_src.start();
}

void
DecoderWindow::onThresholdSet(int val) {
 _decoder.setThreshold(std::pow(10, double(val)/10));
}

void
DecoderWindow::closeEvent(QCloseEvent *evt) {
  _audio_src.stop();
  QMainWindow::closeEvent(evt);
}
