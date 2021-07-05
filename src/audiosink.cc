#include "audiosink.hh"
#include "globals.hh"
#include <iostream>
#include <QTimer>
#include <QEventLoop>
#include "logger.hh"


/* ********************************************************************************************* *
 * QAudioSink, playback
 * ********************************************************************************************* */
QAudioSink::QAudioSink(QIODevice *src, QObject *parent)
  : QIODevice(parent), _output(nullptr), _source(nullptr), _volume(1.0)
{
  this->open(QIODevice::ReadOnly);
  setSource(src);
}

QAudioSink::~QAudioSink() {
  if (this->isOpen())
    this->close();
  _output->stop();
}

double
QAudioSink::volume() const {
  return _volume;
}

void
QAudioSink::setVolume(double volume) {
  _volume = std::max(0.0, std::min(volume, 1.0));
  if (_output)
    _output->setVolume(_volume);
}

void
QAudioSink::setOutputDevice(const QAudioDeviceInfo &output_device) {
  if (_output && _output_device == output_device)
    return;

  if (_output) {
    _output->stop();
    delete _output;
    _output = nullptr;
  }

  QAudioFormat fmt;
  fmt.setByteOrder(QAudioFormat::LittleEndian);
  fmt.setChannelCount(1);
  fmt.setSampleRate(int(Globals::sampleRate));
  fmt.setSampleSize(16);
  fmt.setSampleType(QAudioFormat::SignedInt);
  fmt.setCodec("audio/pcm");
  QAudioDeviceInfo info(output_device);
  if (! info.isFormatSupported(fmt)) {
    logError() << "Audio format is not supported by device" << info.deviceName();
  }

  _output_device = output_device;
  _output = new QAudioOutput(_output_device, fmt, this);
  _output->setVolume(_volume);
  _output->start(this);
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
  : QIODevice(parent), _input(nullptr), _sink(sink)
{
}

QAudioSource::~QAudioSource() {
  this->stop();
  this->close();
}

void
QAudioSource::start() {
  if (_input && QAudio::ActiveState != _input->state())
    _input->start(this);
}

void
QAudioSource::stop() {
  if (_input)
    _input->stop();
}

void
QAudioSource::setInputDevice(const QAudioDeviceInfo &input_device) {
  if (_input && _input_device == input_device)
    return;

  if (_input) {
    this->stop();
    delete _input;
    _input = nullptr;
  }

  QAudioFormat fmt;
  fmt.setByteOrder(QAudioFormat::LittleEndian);
  fmt.setChannelCount(1);
  fmt.setSampleRate(int(Globals::sampleRate));
  fmt.setSampleType(QAudioFormat::SignedInt);
  fmt.setSampleSize(16);
  fmt.setCodec("audio/pcm");

  QAudioDeviceInfo info(input_device);
  if (! info.isFormatSupported(fmt)) {
    logWarn() << "Audio format is not supported by device" << info.deviceName();
  }

  _input_device = input_device;
  _input = new QAudioInput(_input_device, fmt, this);
  this->open(QIODevice::WriteOnly);
  _buffer.reserve(2048*sizeof(int16_t));
}

bool
QAudioSource::isRunning() const {
  return QAudio::ActiveState == _input->state();
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
