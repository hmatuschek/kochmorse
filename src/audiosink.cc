#include "audiosink.hh"
#include "globals.hh"
#include <iostream>
#include <QTimer>
#include <QEventLoop>
#include <QDebug>


/* ********************************************************************************************* *
 * QAudioSink, playback
 * ********************************************************************************************* */
QAudioSink::QAudioSink(QObject *parent)
  : QIODevice(parent), _output(0)
{
  QAudioFormat fmt;
  fmt.setByteOrder(QAudioFormat::LittleEndian);
  fmt.setChannelCount(1);
  fmt.setSampleRate(int(Globals::sampleRate));
  fmt.setSampleSize(16);
  fmt.setSampleType(QAudioFormat::SignedInt);
  fmt.setCodec("audio/pcm");
  QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
  if (! info.isFormatSupported(fmt)) {
    qDebug() << "Warning: Audio format is not supported by device" << info.deviceName();
  }
  _output = new QAudioOutput(fmt, this);
  this->open(QIODevice::ReadWrite);
  _output->start(this);
}

QAudioSink::~QAudioSink() {
  _output->stop();
}

double
QAudioSink::volume() const {
  return _output->volume();
}

void
QAudioSink::setVolume(double factor) {
  return _output->setVolume(factor);
}

bool
QAudioSink::isSequential() const {
  return true;
}

bool
QAudioSink::waitForBytesWritten(int msecs) {
  QTimer timer;
  timer.setSingleShot(true);
  QEventLoop loop;
  connect(this, SIGNAL(bytesWritten(qint64)), &loop, SLOT(quit()));
  connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
  timer.start(msecs);
  loop.exec();
  return timer.isActive();
}

qint64
QAudioSink::bytesAvailable() const {
  return _buffer.size() + QIODevice::bytesAvailable();
}

qint64
QAudioSink::bytesToWrite() const {
  return _buffer.size() + QIODevice::bytesToWrite();
}

qint64
QAudioSink::readData(char *data, qint64 maxlen) {
  if (0 == _buffer.size())
    qDebug() << "underrun.";
  maxlen = std::min(int(maxlen), _buffer.size());
  if (0 >= maxlen)
    return maxlen;

  emit bytesWritten(maxlen);
  memcpy(data, _buffer.data_ptr(), maxlen);
  _buffer.remove(0, maxlen);
  return maxlen;
}

qint64
QAudioSink::writeData(const char *data, qint64 len) {
  _buffer.append(data, len);
  emit readyRead();
  return len;
}



/* ********************************************************************************************* *
 * QAudioSource, recording
 * ********************************************************************************************* */
QAudioSource::QAudioSource(QIODevice *sink, QObject *parent)
  : QIODevice(parent), _stream(0), _sink(sink)
{
  QAudioFormat fmt;
  fmt.setByteOrder(QAudioFormat::LittleEndian);
  fmt.setChannelCount(1);
  fmt.setSampleRate(int(Globals::sampleRate));
  fmt.setSampleType(QAudioFormat::SignedInt);
  fmt.setSampleSize(16);
  fmt.setCodec("audio/pcm");
  _stream = new QAudioInput(fmt, this);
  this->open(QIODevice::WriteOnly);
  _buffer.reserve(2048*sizeof(int16_t));
}

QAudioSource::~QAudioSource() {
  if (0 != _stream)
    this->stop();
  this->close();
}

void
QAudioSource::start() {
  if (QAudio::ActiveState != _stream->state())
    _stream->start(this);
}

void
QAudioSource::stop() {
  _stream->stop();
}

bool
QAudioSource::isRunning() const {
  return QAudio::ActiveState == _stream->state();
}

qint64
QAudioSource::readData(char *data, qint64 maxlen) {
  return 0;
}

qint64
QAudioSource::writeData(const char *data, qint64 len) {
  _buffer.append(data, len);
  _sink->write(_buffer);
  _buffer.clear();
  return len;
}
