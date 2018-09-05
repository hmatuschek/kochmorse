#include "audiosink.hh"
#include "globals.hh"
#include <iostream>
#include <QTimer>
#include <QEventLoop>
#include <QDebug>


/* ********************************************************************************************* *
 * QAudioSink, playback
 * ********************************************************************************************* */
QAudioSink::QAudioSink(QIODevice *src, QObject *parent)
  : QIODevice(parent), _output(nullptr), _source(src)
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

  if (_source) {
    connect(_source, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
    _source->open(QIODevice::ReadOnly);
  }

  _output = new QAudioOutput(fmt, this);
  this->open(QIODevice::ReadOnly);
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
  return _output->setVolume(std::max(0.0, std::min(factor,1.0)));
}

void
QAudioSink::setSource(QIODevice *source) {
  if (_source)
    disconnect(_source, SIGNAL(readyRead()), this, SIGNAL(readyRead()));

  _source = source;
  if (! _source)
    return;

  connect(_source, SIGNAL(readyRead()), this, SIGNAL(readyRead()));
  _source->open(QIODevice::ReadOnly);
}

bool
QAudioSink::isSequential() const {
  return true;
}

qint64
QAudioSink::bytesAvailable() const {
  if (! _source)
    return QIODevice::bytesAvailable();
  return _source->bytesAvailable() + QIODevice::bytesAvailable();
}

qint64
QAudioSink::readData(char *data, qint64 maxlen) {
  if (! _source)
    return 0;

  maxlen = _source->read(data, maxlen);
  return maxlen;
}

qint64
QAudioSink::writeData(const char *data, qint64 len) {
  Q_UNUSED(data);
  Q_UNUSED(len);
  return 0;
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
